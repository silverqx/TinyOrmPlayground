#include "services/querycountersservice.hpp"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QLibraryInfo>

#include <orm/db.hpp>
#include <orm/exceptions/invalidargumenterror.hpp>
#include <orm/libraryinfo.hpp>
#include <orm/macros/compilerdetect.hpp>
#include <orm/utils/fs.hpp>
#include <orm/version.hpp>

#include "config.hpp"

#include "configuration.hpp"
#include "support/globals.hpp"
#include "support/utils.hpp"
#include "version.hpp"

using Orm::Constants::COMMA;

using Orm::Exceptions::InvalidArgumentError;
using Orm::Exceptions::LogicError;
using Orm::Exceptions::RuntimeError;

using Orm::LibraryInfo;
using Orm::Utils::Fs;

using TinyPlay::Support::Utils;

namespace TinyPlay::Services
{

QueryCountersService::QueryCountersService(Configuration &config)
    : Service(config)
{
    // Secure that the main thread will be first in all counters everytime
    newCountersForAppSummary("main");
}

void QueryCountersService::initConnsInThreadsTesting()
{
    // Throw when connections to run are empty
    throwIfConnsToRunEmpty();
    // Initialize thread logging to the file or string
    initThreadLogging();
    // Santize m_config.ConnectionsToRunInThread
    santizeConnectionsToRunInThread();
}

QStringList QueryCountersService::computeConnectionsToCount(const QString &connection) const
{
    Utils::throwIfNonEmptyConn(connection);
    Utils::throwIfEmptyConn(connection);

    // Called from a non-main/worker thread
    if (!connection.isEmpty())
        return computeConnectionsToCountForWorkerThread(connection);

    // Otherwise called from main thread
    return computeConnectionsToCountForMainThread();
}

void QueryCountersService::enableAllQueryLogCounters() const
{
    // Create connections eagerly, so I can enable counters
    for (const auto &connection : std::as_const(Configuration::ConnectionsToCount))
        DB::connection(connection);

    // Check if all connections to count were opened correctly
    throwIfClosedConnection(Configuration::ConnectionsToCount);

    // Enable counters on all database connections
    DB::enableElapsedCounters(Configuration::ConnectionsToCount);
    DB::enableStatementCounters(Configuration::ConnectionsToCount);
}

void QueryCountersService::resetAllQueryLogCounters() const
{
    DB::resetElapsedCounters(Configuration::ConnectionsToCount);
    DB::resetStatementCounters(Configuration::ConnectionsToCount);

    for (const auto &connection : std::as_const(Configuration::ConnectionsToCount))
        DB::forgetRecordModificationState(connection);
}

namespace
{
    /*! Log file for log messages from threads. */
    QFile logFile {Fs::resolveHome(Configuration::LogFilepath)};
    /*! Text stream for log messages from threads. */
    QTextStream logThreadStream;

    /*! All log messages from a given thread. */
    struct LogFromThreadItem
    {
        /*! Name of a thread to which log messages belong to. */
        QueryCountersService::ThreadName threadName;
        /*! All log messages. */
        std::vector<TinyPlay::Support::MessageLogItem> logMessages;
    };

    /*! Log messages from all threads. */
    std::vector<LogFromThreadItem> logsFromThreads;
    /*! Index of LogFromThreadItem for log messages from all threads. */
    using LogFromThreadIdx = std::size_t;
    /*! Fast lookup map for application counters. */
    std::unordered_map<QueryCountersService::ThreadName,
                       LogFromThreadIdx> logsFromThreadsMap;

    /*! Mutex to secure writes for log messages in threads. */
    std::mutex mx_dataLogStream;
} // namespace

QTextStream &operator<<(
        QTextStream &stream,
        const std::vector<TinyPlay::Support::MessageLogItem> &logFromThread)
{
    for (const auto &[type, message] : logFromThread)
        stream << message << "\n";

    return stream;
}

void QueryCountersService::saveLogsFromThread(const ThreadName &threadName) const
{
    std::scoped_lock lock(mx_dataLogStream);

    // Log to the file
    if constexpr (Configuration::IsLoggingToFile) {
        logThreadStream << Support::g_logFromThread;

        return;
    }

    /* Log to the std::vector, when logging to the file is disabled then
       the output will be logged to the console at the end of testAllConnections()
       to not pollute the console log. */
#ifdef __clang__
    logsFromThreads.push_back({threadName, Support::g_logFromThread});
#else
    logsFromThreads.emplace_back(threadName, Support::g_logFromThread);
#endif
    logsFromThreadsMap.emplace(threadName, logsFromThreads.size());
}

void QueryCountersService::replayThrdLogToConsole()
{
    throwIfInThread();

    for (const auto &[threadName, logsFromThread] : logsFromThreads) {
        qInfo().noquote().nospace()
                << QStringLiteral("\n\n-- Replaying log from %1 thread")
                   .arg(threadName);

        for (const auto &[type, message] : logsFromThread)
            QDebug(type).noquote().nospace() << message;
    }
}

void QueryCountersService::saveCountersForAppSummary(const ThreadName &threadName)
{
    std::scoped_lock lock(m_countersMutex);

    if (m_appCountersMap.contains(threadName))
        addCountersForAppSummary(threadName);
    else
        newCountersForAppSummary(threadName);
}

void QueryCountersService::logQueryCounters(const QString &func,
                                            const std::optional<qint64> functionElapsed)
{
    // Header with the function execution time
    const auto line = QString("-").repeated(13 + func.size());

    // Function elapsed execution time
    qInfo().noquote().nospace() << "\n" << line;
    qInfo().noquote().nospace() << "PlayTest - " << func << "()";
    qInfo().noquote().nospace() << line;

    // BUG emoji icons take two chars silverqx
    qInfo().nospace() << "\n⚡ Function Execution time : "
                      << (functionElapsed ? *functionElapsed : -1) << "ms";

    if (functionElapsed)
        m_threadFunctionsElapsed += *functionElapsed;

    // Total counters for the summary
    qint64 summaryQueriesElapsed = -1;
    StatementsCounter summaryStatementsCounter;
    bool summaryRecordsHaveBeenModified = false;

    /* If any connection count statements, then counters will be -1, set them
       to the zero values only if some connection count statements, the same is true
       for queries execution time counter. 🧹 */
    if (DB::anyCountingStatements())
        summaryStatementsCounter = {0, 0, 0};
    if (DB::anyCountingElapsed())
        summaryQueriesElapsed = 0;

    // Log all connections
    for (const auto &connectionName :
         std::as_const(Configuration::ConnectionsToCount)
    ) {
        auto &connection = DB::connection(connectionName);

        // Queries execution time
        const auto queriesElapsed = connection.takeElapsedCounter();
        // Don't count if counting is not enabled
        if (connection.countingElapsed()) {
            summaryQueriesElapsed += queriesElapsed;
            m_threadQueriesElapsed += queriesElapsed;
        }

        // Executed statements counter
        const auto statementsCounter = connection.takeStatementsCounter();
        // Count only when the counting is enabled
        if (connection.countingStatements()) {
            summaryStatementsCounter.normal        += statementsCounter.normal;
            summaryStatementsCounter.affecting     += statementsCounter.affecting;
            summaryStatementsCounter.transactional += statementsCounter.transactional;

            m_threadStatementsCounter.normal        += statementsCounter.normal;
            m_threadStatementsCounter.affecting     += statementsCounter.affecting;
            m_threadStatementsCounter.transactional += statementsCounter.transactional;
        }

        // Whether recods have been modified
        const auto recordsHaveBeenModified = connection.getRecordsHaveBeenModified();
        summaryRecordsHaveBeenModified |= recordsHaveBeenModified;
        m_threadRecordsHaveBeenModified |= recordsHaveBeenModified;

        // Log connection statistics
        logQueryCountersBlock(
                    QStringLiteral("Connection name - '%1'").arg(connectionName),
                    queriesElapsed, statementsCounter, recordsHaveBeenModified);
    }

    // Summary
    logQueryCountersBlock(QStringLiteral("Summary"),
                          summaryQueriesElapsed, summaryStatementsCounter,
                          summaryRecordsHaveBeenModified);

    qInfo().noquote() << line;
}

void QueryCountersService::logQueryCountersBlock(
            const QString &title, const qint64 queriesElapsed,
            const StatementsCounter statementsCounter,
            const bool recordsHaveBeenModified) const
{
    const auto loggingAppSummary = title.contains("Application");
    const auto &[
        functionsElapsedPrintable, queriesElapsedPrintable, statementsCounterPrintable,
        recordsHaveBeenModifiedPrintable
    ] = loggingAppSummary && Configuration::ConnectionsInThreads
            ? getAppCountersPrintable()
            : getAppCountersPrintable(m_threadFunctionsElapsed, queriesElapsed,
                                      statementsCounter, recordsHaveBeenModified);

    // Header
    if (loggingAppSummary) {
        qInfo() << "\n-----------------------";
        qInfo().noquote().nospace() << "  " << title;
        qInfo() << "-----------------------";
    } else {
        qInfo() << "";
        qInfo().noquote() << title;
        qInfo() << "---";
    }

    // General informations about the environment
    if (loggingAppSummary) {
        qInfo().nospace() << "⚙ Compiler version             : "
                          << TINYORM_COMPILER_STRING;
        qInfo().nospace() << "⚙ Qt version                   : "
                          << QT_VERSION_STR;
        qInfo().nospace() << "  Qt build type                : "
                          << (QLibraryInfo::isDebugBuild() ? "Debug" : "Release");
        qInfo().nospace() << "  Qt full build type           : "
                          << QLibraryInfo::build() << "\n";

        qInfo().nospace() << "⚙ TinyORM version              : "
                          << TINYORM_VERSION_STR;
        qInfo().nospace() << "  TinyORM build type           : "
                          << (LibraryInfo::isDebugBuild() ? "Debug" : "Release");
        qInfo().nospace() << "  TinyORM full build type      : "
                          << LibraryInfo::build();

        qInfo().nospace() << "⚙ TinyOrmPlayground version    : "
                          << TINYPLAY_VERSION_STR;
        qInfo().nospace() << "  TinyOrmPlayground build type : "
#ifdef TINYPLAY_NO_DEBUG
                          << "Release"
#else
                          << "Debug"
#endif
                          << "\n";

        // All Functions execution time
        qInfo().noquote().nospace() << "⚡ Functions Execution time : "
                                    << functionsElapsedPrintable << "\n";
    }

    // Threads mode (needed in the InvokeXTimes.ps1 script)
    if (loggingAppSummary) {
        constexpr const char *threadMode =
                Configuration::ConnectionsInThreads ? "multi-thread"
                                                    : "single-thread";

        qInfo() << "♆ Threads mode             :" << threadMode;
    }

    // Counted connections
    if (title.contains("Summary")) {
        const auto &countedConnections = countedConnectionsPrintable(loggingAppSummary);
        const auto countedSize = countedConnections.size();

        qInfo().nospace().noquote()
                << "∑ " << "Counted connections    "
                << (loggingAppSummary ? "  " : "")
                << ": "
                << countedSize
                << (countedSize > 1 || loggingAppSummary
                    ? QStringLiteral(" (%1)").arg(countedConnections.join(COMMA))
                    : "");
    }

    // Queries execution time
    qInfo().noquote().nospace() << "⚡ Queries execution time "
                                << (loggingAppSummary ? "  " : "")
                                << ": " << queriesElapsedPrintable;

    // Whether records have been modified on the current connection
    qInfo().noquote().nospace() << "✎ Records was modified   "
                                << (loggingAppSummary ? "  " : "")
                                << ": " << recordsHaveBeenModifiedPrintable;

    // Type of executed queries counters
    qInfo() << "⚖ Statements counters";
    qInfo().noquote() << "  Normal        :" << statementsCounterPrintable.normal;
    qInfo().noquote() << "  Affecting     :" << statementsCounterPrintable.affecting;
    qInfo().noquote() << "  Transaction   :" << statementsCounterPrintable.transactional;
    qInfo().noquote() << "  Total         :" << statementsCounterPrintable.total;
    qInfo() << "---";
}

QStringList QueryCountersService::computeConnectionsToCountForMainThread() const
{
    throwIfInThread();

    QStringList connectionsToCount;
    connectionsToCount.reserve(m_config.CountableConnections.size());

    std::copy_if(m_config.CountableConnections.cbegin(),
                 m_config.CountableConnections.cend(),
                 std::back_inserter(connectionsToCount),
                 [this](const auto &connection)
    {
        const auto &mappedConnection = m_config.ConnectionsMapReverse.contains(connection)
                                       ? m_config.ConnectionsMapReverse.at(connection)
                                       : connection;

        return !m_config.RemovableConnections.contains(mappedConnection) ||
                (!Configuration::ConnectionsInThreads &&
                 m_config.ConnectionsToTest.contains(mappedConnection)) ||
                (Configuration::ConnectionsInThreads &&
                 m_config.ConnectionsToTest.contains(mappedConnection) &&
                 !m_config.ConnectionsToRunInThread.contains(mappedConnection));
    });

    /* Do it as a separate operation and not in std::copy_if() above, to not pollute
       logic, this is much cleaner. */
    connectionsToCount.removeOne("mysql_mainthread");

    return connectionsToCount;
}

QStringList
QueryCountersService::computeConnectionsToCountForWorkerThread(const QString &connection) const
{
    return getMappedConnections(connection);
}

void QueryCountersService::santizeConnectionsToRunInThread()
{
    if constexpr (!Configuration::ConnectionsInThreads)
        return;

    const auto removeEnd = std::remove_if(m_config.ConnectionsToRunInThread.begin(), // clazy:exclude=detaching-member
                                          m_config.ConnectionsToRunInThread.end(), // clazy:exclude=detaching-member
                                          [this](const auto &connection)
    {
        return !m_config.ConnectionsToTest.contains(connection);
    });

    m_config.ConnectionsToRunInThread.erase(removeEnd,
                                            m_config.ConnectionsToRunInThread.end()); // clazy:exclude=detaching-member
}

void QueryCountersService::initThreadLogging() const
{
    logThreadStream.setDevice(&logFile);
    openLogFile();
}

void QueryCountersService::openLogFile() const
{
    if constexpr (!Configuration::ConnectionsInThreads || !Configuration::IsLoggingToFile)
        return;

    if (!logFile.open(QFile::WriteOnly | QFile::Truncate))
        throw RuntimeError(
                QStringLiteral(
                    "Can not open log file 'logFile(%1)': %2 "
                    "[QFileDevice::FileError(%3)].")
                .arg(logFile.fileName(), logFile.errorString())
                .arg(logFile.error()));
}

void QueryCountersService::newCountersForAppSummary(const ThreadName &threadName)
{
#ifdef __clang__
    m_appFunctionsElapsed.push_back({threadName, m_threadFunctionsElapsed});
    m_appQueriesElapsed.push_back({threadName, m_threadQueriesElapsed});
    m_appStatementsCounter.push_back({threadName, m_threadStatementsCounter});
    m_appRecordsHaveBeenModified.push_back({threadName,
                                            m_threadRecordsHaveBeenModified});
#else
    m_appFunctionsElapsed.emplace_back(threadName, m_threadFunctionsElapsed);
    m_appQueriesElapsed.emplace_back(threadName, m_threadQueriesElapsed);
    m_appStatementsCounter.emplace_back(threadName, m_threadStatementsCounter);
    m_appRecordsHaveBeenModified.emplace_back(threadName,
                                              m_threadRecordsHaveBeenModified);
#endif

    m_appCountersMap.emplace(threadName, m_appFunctionsElapsed.size() - 1);
}

void QueryCountersService::addCountersForAppSummary(const ThreadName &threadName)
{
    m_appFunctionsElapsed.at(m_appCountersMap[threadName]).value +=
            m_threadFunctionsElapsed;
    m_appQueriesElapsed.at(m_appCountersMap[threadName]).value +=
            m_threadQueriesElapsed;
    m_appRecordsHaveBeenModified.at(m_appCountersMap[threadName]).value |=
            m_threadRecordsHaveBeenModified;
    m_appStatementsCounter.at(m_appCountersMap[threadName]).value.normal +=
            m_threadStatementsCounter.normal;
    m_appStatementsCounter.at(m_appCountersMap[threadName]).value.affecting +=
            m_threadStatementsCounter.affecting;
    m_appStatementsCounter.at(m_appCountersMap[threadName]).value.transactional +=
            m_threadStatementsCounter.transactional;
}

const QStringList &
QueryCountersService::countedConnectionsPrintable(const bool loggingAppSummary) const
{
    if (!loggingAppSummary)
        return Configuration::ConnectionsToCount;

    static const QStringList cachedCountedConnections = appCountedConnectionsPrintable();
    return cachedCountedConnections;
}

QStringList QueryCountersService::appCountedConnectionsPrintable() const
{
    QStringList countedConnections;
    countedConnections.reserve(m_config.CountableConnections.size());

    std::copy_if(m_config.CountableConnections.cbegin(),
                 m_config.CountableConnections.cend(),
                 std::back_inserter(countedConnections),
                 [this](const auto &connection)
    {
        const auto &mappedConnection = m_config.ConnectionsMapReverse.contains(connection)
                                       ? m_config.ConnectionsMapReverse.at(connection)
                                       : connection;

        return !m_config.RemovableConnections.contains(mappedConnection) ||
                m_config.ConnectionsToTest.contains(mappedConnection);
    });

    return countedConnections;
}

void QueryCountersService::throwIfInThread() const
{
    if (Support::g_inThread)
        throw InvalidArgumentError(
                "This method can be called only from the main thread, it means "
                "g_inThread = false.");
}

void QueryCountersService::throwIfConnsToRunEmpty() const
{
    if (Configuration::ConnectionsInThreads &&
        m_config.ConnectionsToRunInThread.isEmpty()
    )
        throw LogicError(
                "Configuration::ConnectionsInThreads = true but "
                "m_config.ConnectionsToRunInThread is empty.");
}

void QueryCountersService::throwIfClosedConnection(const QStringList &connections) const
{
    const auto &openedConnections = DB::openedConnectionNames();

    for (const auto &connection : connections) {
        if (openedConnections.contains(connection))
            continue;

        throw RuntimeError(
                QStringLiteral("Database connection '%1' was not opened correctly.")
                .arg(connection));
    }
}

namespace
{
    /*! Common template for application counters. */
    const auto countersTmpl = QStringLiteral("%1 (%2)");
    /*! Common template for elapsed application counters. */
    const auto countersElapsedTmpl = QStringLiteral("%1ms (%2)");
    /*! Common template for application statement counters. */
    const auto statementsCounterTmpl = QStringLiteral("%1 [%2]");
} // namespace

QString QueryCountersService::appElapsedCounterPrintable(
        const std::vector<AppCounterItem<qint64>> &counters) const
{
    QStringList result;
    result.reserve(static_cast<int>(counters.size()));

    for (const auto &[threadName, value] : counters)
        result << countersElapsedTmpl.arg(value).arg(threadName);

    return result.join(COMMA);
}

QString QueryCountersService::appBoolCounterPrintable(
        const std::vector<AppCounterItem<bool>> &counters) const
{
    QStringList result;
    result.reserve(static_cast<int>(counters.size()));

    for (const auto &[threadName, value] : counters)
        result << countersTmpl.arg(value ? QStringLiteral("yes") : QStringLiteral("no"),
                                   threadName);

    return result.join(COMMA);
}

QueryCountersService::StatementsCounterPrintable
QueryCountersService::appStatementsCounterPrintable(
        const std::vector<AppCounterItem<StatementsCounter>> &counters) const
{
    StatementsCounterPrintable result;

    // Sum up normal/affected/transactional counters
    StatementsCounterTotal sumCounters;

    // Set counters to 0 when any of the counters is enabled/counting
    appZeroCounters(counters, sumCounters);

    // Sum up the total and also sum up individual statement counters
    appSumUpCounters(counters, sumCounters);

    // Counters by thread in printable format
    QStringList normal;
    QStringList affecting;
    QStringList transactional;
    QStringList total;

    const auto countersSize = static_cast<QList<QStringList>::size_type>(counters.size());
    normal.reserve(countersSize);
    affecting.reserve(countersSize);
    transactional.reserve(countersSize);
    total.reserve(countersSize);

    for (const auto &[threadName, statementsCounter] : counters) {
        normal << countersTmpl.arg(statementsCounter.normal).arg(threadName);
        affecting << countersTmpl.arg(statementsCounter.affecting).arg(threadName);
        transactional << countersTmpl
                         .arg(statementsCounter.transactional).arg(threadName);

        // Sum up total for a given thread
        {
            const auto &[
                normalThrd, affectingThrd, transactionalThrd
            ] = statementsCounter;
            qint64 sumThrd = -1;

            // Show -1 when any statements were not executed
            if (normalThrd > -1 || affectingThrd > -1 || transactionalThrd > -1)
                sumThrd = 0;

            // Sum up the total and also sum up individual statement counters
            if (normalThrd > -1)
                sumThrd += normalThrd;
            if (affectingThrd > -1)
                sumThrd += affectingThrd;
            if (transactionalThrd > -1)
                sumThrd += transactionalThrd;

            total << countersTmpl.arg(sumThrd).arg(threadName);
        }
    }

    // Result
    result.normal = sumCounters.normal < 1
                    ? QString::number(sumCounters.normal)
                    : statementsCounterTmpl.arg(sumCounters.normal)
                                           .arg(normal.join(COMMA));
    result.affecting = sumCounters.affecting < 1
                       ? QString::number(sumCounters.affecting)
                       : statementsCounterTmpl.arg(sumCounters.affecting)
                                              .arg(affecting.join(COMMA));
    result.transactional = sumCounters.transactional < 1
                           ? QString::number(sumCounters.transactional)
                           : statementsCounterTmpl.arg(sumCounters.transactional)
                                                  .arg(transactional.join(COMMA));
    result.total = sumCounters.total < 1 ? QString::number(sumCounters.total)
                                         : statementsCounterTmpl.arg(sumCounters.total)
                                                                .arg(total.join(COMMA));

    return result;
}

void QueryCountersService::appZeroCounters(
        const std::vector<AppCounterItem<StatementsCounter>> &counters,
        StatementsCounterTotal &sumCounters) const
{
    // Any of all statements counter is counting
    const auto anyOfCounters = [&counters](const auto &condition)
    {
        return std::any_of(counters.cbegin(), counters.cend(),
                           [&condition](const auto &counterItem)
        {
            const auto &[normal, affecting, transactional] = counterItem.value;

            return std::invoke(condition, normal, affecting, transactional);
        });
    };

    // Set counters to 0 when any of the counters below meet a given condition
    if (anyOfCounters([](auto normal, auto affecting, auto transactional)
            { return normal > -1 || affecting > -1 || transactional > -1; })
    )
        sumCounters.total = 0;
    if (anyOfCounters([](const auto normal, auto /*unused*/, auto /*unused*/)
            { return normal > -1; }))
        sumCounters.normal = 0;
    if (anyOfCounters([](auto /*unused*/, const auto affecting, auto /*unused*/)
            { return affecting > -1; }))
        sumCounters.affecting = 0;
    if (anyOfCounters([](auto /*unused*/, auto /*unused*/, const auto transactional)
            { return transactional > -1; }))
        sumCounters.transactional = 0;
}

void QueryCountersService::appSumUpCounters(
        const std::vector<AppCounterItem<StatementsCounter>> &counters,
        StatementsCounterTotal &sumCounters) const
{
    for (const auto &[_, statementsCounter] : counters) {
        const auto &[normal, affecting, transactional] = statementsCounter;

        if (normal > -1) {
            sumCounters.normal += normal;
            sumCounters.total += normal;
        }
        if (affecting > -1) {
            sumCounters.affecting += affecting;
            sumCounters.total += affecting;
        }
        if (transactional > -1) {
            sumCounters.transactional += transactional;
            sumCounters.total += transactional;
        }
    }
}

QueryCountersService::CountersPrintable
QueryCountersService::getAppCountersPrintable(
        const qint64 functionsElapsed, const qint64 queriesElapsed,
        const StatementsCounter statementsCounter,
        const bool recordsHaveBeenModified) const
{
    const auto &[normal, affecting, transactional] = statementsCounter;

    // Show -1 when any statements were not executed
    int total = (normal > -1 || affecting > -1 || transactional > -1) ? 0 : -1;

    if (normal > -1)
        total += normal;
    if (affecting > -1)
        total += affecting;
    if (transactional > -1)
        total += transactional;

    return {
        QStringLiteral("%1ms").arg(functionsElapsed),
        QStringLiteral("%1%2").arg(queriesElapsed)
                              .arg(queriesElapsed > -1 ? QStringLiteral("ms") : ""),
        {QString::number(normal), QString::number(affecting),
         QString::number(transactional), QString::number(total)},
        {recordsHaveBeenModified ? QStringLiteral("yes") : QStringLiteral("no")},
    };
}

QueryCountersService::CountersPrintable
QueryCountersService::getAppCountersPrintable() const
{
    return {
        appElapsedCounterPrintable(m_appFunctionsElapsed),
        appElapsedCounterPrintable(m_appQueriesElapsed),
        appStatementsCounterPrintable(m_appStatementsCounter),
        appBoolCounterPrintable(m_appRecordsHaveBeenModified),
    };
}

} // namespace TinyPlay::Services
