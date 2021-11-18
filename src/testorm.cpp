#include "testorm.hpp"

#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QLibraryInfo>

#include <orm/db.hpp>
#include <orm/exceptions/invalidargumenterror.hpp>
#include <orm/libraryinfo.hpp>
#include <orm/macros/compilerdetect.hpp>
#include <orm/utils/thread.hpp>
#include <orm/version.hpp>

#include "config.hpp"

#include "support/globals.hpp"
#include "support/utils.hpp"
#include "tests/testallothertests.hpp"
#include "tests/testconnection.hpp"
#include "tests/testforplay.hpp"
#include "tests/testquerybuilder.hpp"
#include "tests/testquerybuilderdbspecific.hpp"
#include "tests/testtinyorm.hpp"
#include "version.hpp"

using Orm::Constants::COMMA;
using Orm::Constants::H127001;
using Orm::Constants::P3306;
using Orm::Constants::P5432;
using Orm::Constants::QMYSQL;
using Orm::Constants::QPSQL;
using Orm::Constants::QSQLITE;
using Orm::Constants::SYSTEM;
using Orm::Constants::UTF8;
using Orm::Constants::UTF8MB4;
using Orm::Constants::charset_;
using Orm::Constants::check_database_exists;
using Orm::Constants::collation_;
using Orm::Constants::database_;
using Orm::Constants::driver_;
using Orm::Constants::foreign_key_constraints;
using Orm::Constants::host_;
using Orm::Constants::isolation_level;
using Orm::Constants::LOCAL;
using Orm::Constants::options_;
using Orm::Constants::password_;
using Orm::Constants::port_;
using Orm::Constants::prefix_;
using Orm::Constants::PUBLIC;
using Orm::Constants::schema_;
using Orm::Constants::strict_;
using Orm::Constants::timezone_;
using Orm::Constants::username_;

using Orm::Exceptions::InvalidArgumentError;
using Orm::Exceptions::LogicError;
using Orm::Exceptions::RuntimeError;

using Orm::LibraryInfo;
using Orm::Utils::Thread;

/*
   Notes:
   - text in [] names connection name, eg. [sqlite]

   Performance:
   ---
   - only on MySQL connection CONNECTIONS_TO_TEST {mysql} and ms are best values I saw
     - 13. jul 2021 ( 325 queries executed, TinyORM 749c4014 )
       - Qt 5.15.2 ; msvc 16.10 x64
         - debug build
           - under 945ms all functions
           - 273ms all queries
         - prod. build with disabled debug output ( QT_NO_DEBUG_OUTPUT )
           - under 413ms all functions
           - 247ms all queries
         - tst_model prod. build on MySQL connection only ( createConnections({MYSQL}) )
           - 181ms
     - 10. oct 2021 ( 621 queries executed, TinyORM 878bb1f0 )
       - Qt 5.15.2 ; msvc 16.11.4 x64
         - prod. build LTO with disabled debug output ( QT_NO_DEBUG_OUTPUT )
           - around 670ms / min. 630ms all functions
           - around 350ms / min. 305ms all queries
*/

namespace TinyPlay
{

TestOrm::TestOrm()
    : m_configurations(getConfigurations())
{
    // Secure that the main thread will be first in all counters everytime
    newCountersForAppSummary("main");
}

const TestOrm::OrmConfigurationsType &TestOrm::getConfigurations() const
{
    static const QVariantHash mysqlConnection {
        {driver_,    QMYSQL},
        {host_,      qEnvironmentVariable("DB_MYSQL_HOST", H127001)},
        {port_,      qEnvironmentVariable("DB_MYSQL_PORT", P3306)},
        {database_,  qEnvironmentVariable("DB_MYSQL_DATABASE", "")},
        {username_,  qEnvironmentVariable("DB_MYSQL_USERNAME", "")},
        {password_,  qEnvironmentVariable("DB_MYSQL_PASSWORD", "")},
        {charset_,   qEnvironmentVariable("DB_MYSQL_CHARSET", UTF8MB4)},
        {collation_, qEnvironmentVariable("DB_MYSQL_COLLATION",
                                          QStringLiteral("utf8mb4_0900_ai_ci"))},
        // CUR add timezone names to the MySQL server and test them silverqx
        {timezone_,       SYSTEM},
        {prefix_,         ""},
        {strict_,         true},
        {isolation_level, QStringLiteral("REPEATABLE READ")},
        {options_,        QVariantHash()},
    };

    static const OrmConfigurationsType cached {
        // Main MySQL connection in test loop
        {"mysql", mysqlConnection},

        // Used in the Torrent model as u_connection
        {"mysql_alt", mysqlConnection},

        /* Used as MySQL connection name in the main thread when connections in threads
           is enabled to avoid MySQL connection name collision. */
        {"mysql_mainthread", mysqlConnection},

        /* Used in the testQueryBuilderDbSpecific() only to test a cross-database query,
           a connection to the "laravel_8" database. */
        {"mysql_laravel8", {
            {driver_,    QMYSQL},
            {host_,      qEnvironmentVariable("DB_MYSQL_LARAVEL_HOST", H127001)},
            {port_,      qEnvironmentVariable("DB_MYSQL_LARAVEL_PORT", P3306)},
            {database_,  qEnvironmentVariable("DB_MYSQL_LARAVEL_DATABASE", "")},
            {username_,  qEnvironmentVariable("DB_MYSQL_LARAVEL_USERNAME", "")},
            {password_,  qEnvironmentVariable("DB_MYSQL_LARAVEL_PASSWORD", "")},
            {charset_,   qEnvironmentVariable("DB_MYSQL_LARAVEL_CHARSET", UTF8MB4)},
            {collation_, qEnvironmentVariable("DB_MYSQL_LARAVEL_COLLATION",
                                              QStringLiteral("utf8mb4_0900_ai_ci"))},
            {timezone_,       SYSTEM},
            {prefix_,         ""},
            {strict_,         true},
            {isolation_level, QStringLiteral("REPEATABLE READ")},
            {options_,        QVariantHash()},
        }},

        // Main SQLite connection in test loop
        {"sqlite", {
            {driver_,    QSQLITE},
            {database_,  qEnvironmentVariable("DB_SQLITE_DATABASE", "")},
            {prefix_,    ""},
            {options_,   QVariantHash()},
            {foreign_key_constraints, qEnvironmentVariable("DB_SQLITE_FOREIGN_KEYS",
                                                           QStringLiteral("true"))},
            {check_database_exists,   true},
        }},

        // Used in the testConnection() only to test SQLite :memory: driver
        {"sqlite_memory", {
            {driver_,    QSQLITE},
            {database_,  QStringLiteral(":memory:")},
            {prefix_,    ""},
            {options_,   QVariantHash()},
            {foreign_key_constraints, qEnvironmentVariable("DB_SQLITE_FOREIGN_KEYS",
                                                           QStringLiteral("true"))},
        }},

        /* Used in the testConnection() only to test behavior when the configuration
           option check_database_exists = true. */
        {"sqlite_check_exists_true", {
            {driver_,    QSQLITE},
            {database_,  m_config.CheckDatabaseExistsFile},
            {check_database_exists, true},
        }},

        /* Used in the testConnection() only to test behavior when the configuration
           option check_database_exists = true. */
        {"sqlite_check_exists_false", {
            {driver_,    QSQLITE},
            {database_,  m_config.CheckDatabaseExistsFile},
            {check_database_exists, false},
        }},

        // Main PostgreSQL connection in test loop
        {"postgres", {
            {driver_,   QPSQL},
            {host_,     qEnvironmentVariable("DB_PGSQL_HOST",     H127001)},
            {port_,     qEnvironmentVariable("DB_PGSQL_PORT",     P5432)},
            {database_, qEnvironmentVariable("DB_PGSQL_DATABASE", "")},
            {schema_,   qEnvironmentVariable("DB_PGSQL_SCHEMA",   PUBLIC)},
            {username_, qEnvironmentVariable("DB_PGSQL_USERNAME", "postgres")},
            {password_, qEnvironmentVariable("DB_PGSQL_PASSWORD", "")},
            {charset_,  qEnvironmentVariable("DB_PGSQL_CHARSET",  UTF8)},
            // I don't use timezone types in postgres anyway
            {timezone_, LOCAL},
            {prefix_,   ""},
            {options_,  QVariantHash(/*{{"requiressl", 1}}*/)},
        }},
    };

    return cached;
}

TestOrm &TestOrm::connectToDatabase()
{
    throwIfAlreadyCalled();

    /* Create database connections: mysql, sqlite and mysql_alt, and make
       mysql default database connection. */
    m_db = DB::create(computeConfigurationsToAdd(), "mysql");

    Configuration::CONNECTIONS_TO_COUNT = computeConnectionsToCount();

    // Enable counters on all database connections
    enableAllQueryLogCounters();

    return *this;
}

TestOrm &TestOrm::run()
{
    // Test intended for play
    Tests::TestForPlay(*this).run();

    // All other tests
    Tests::TestAllOtherTests(*this).run();

    // Test TinyORM's Database Connections
    Tests::TestConnection(*this).run();
    // Database-specific tests related to the QueryBuilder
    Tests::TestQueryBuilderDbSpecific(*this).run();

    /* Main test playground code */
    testAllConnections();

    // Add counters for main thread
    saveCountersForAppSummary("main");

    // Whole application Summary
    logQueryCountersBlock(QStringLiteral("Application Summary"),
                          m_threadQueriesElapsed, m_threadStatementsCounter,
                          m_threadRecordsHaveBeenModified);

    return *this;
}

namespace
{
    /*! Log file for log messages from threads. */
    QFile logFile {Configuration::LogFilepath};
    /*! Text stream for log messages from threads. */
    QTextStream logThreadStream;

    /*! All log messages from a given thread. */
    struct LogFromThreadItem
    {
        /*! Name of a thread to which log messages belong to. */
        TestOrm::ThreadName threadName;
        /*! All log messages. */
        std::vector<TinyPlay::Support::MessageLogItem> logMessages;
    };

    /*! Log messages from all threads. */
    std::vector<LogFromThreadItem> logsFromThreads;
    /*! Index of LogFromThreadItem for log messages from all threads. */
    using logFromThreadIdx = std::size_t;
    /*! Fast lookup map for application counters. */
    std::unordered_map<TestOrm::ThreadName, logFromThreadIdx> logsFromThreadsMap;

    /*! Mutex to secure writes for log messages in threads. */
    std::mutex mx_dataLogStream;
} // namespace

void TestOrm::testAllConnections()
{
    throwIfConnsToRunEmpty();
    initThreadLogging();
    santizeConnectionsToRunInThread();

    {
        // BUG QDateTime is not reentrant, this avoids crashes in threads during added calendar to the Registry silverqx
        [[maybe_unused]]
        auto _ = QDateTime::fromString("st jan 6 14:51:23 2021"); // clazy:exclude=unused-non-trivial-variable

        // To join threads at the current block
        std::vector<std::jthread> threads;

        for (const auto &connection : Configuration::CONNECTIONS_TO_TEST) {
            // Run connection in the worker thread
            if (m_config.ConnectionsInThreads &&
                m_config.ConnectionsToRunInThread.contains(connection)
            ) {
                threads.emplace_back(&TestOrm::testConnectionInWorkerThread, this,
                                     connection);
                continue;
            }

            // Otherwise run in the main thread
            testConnectionInMainThread(connection);
        }
    } // join threads

    // Restore default connection
    DB::setDefaultConnection("mysql");

    if (!m_config.IsLoggingToFile)
        replayThrdLogToConsole();
}

void TestOrm::testConnectionInMainThread(const QString &connection)
{
    DB::setDefaultConnection(connection);

    Tests::TestQueryBuilder(*this).run();
    Tests::TestTinyOrm(*this).run();
}


QTextStream &operator<<(
        QTextStream &stream,
        const std::vector<TinyPlay::Support::MessageLogItem> &logFromThread)
{
    for (const auto &[type, message] : logFromThread)
        stream << message << "\n";

    return stream;
}

void TestOrm::testConnectionInWorkerThread(const QString &connection)
{
    try {
        Thread::nameThreadForDebugging(connection);

        throwIfNoConfig(connection);

        Support::g_inThread = true;

        m_db->addConnections(computeConfigurationsToAdd(connection));

        DB::setDefaultConnection(connection);

        Configuration::CONNECTIONS_TO_COUNT = computeConnectionsToCount(connection);

        // Enable counters on all database connections to count
        enableAllQueryLogCounters();

        Tests::TestQueryBuilder(*this).run();
        Tests::TestTinyOrm(*this).run();

        // Save counters from a current thread for Application Summary
        saveCountersForAppSummary(connection);

        saveLogsFromThread(connection);

    } catch (const std::exception &e) {
        // Secure that an exception will be logged to the console
        Support::g_inThread = false;

        Utils::logException(e);
    }
}

void TestOrm::logQueryCounters(const QString &func,
                               const std::optional<qint64> functionElapsed)
{
    // Header with the function execution time
    const auto line = QString("-").repeated(13 + func.size());

    // Function elapsed execution time
    qInfo().noquote().nospace() << "\n" << line;
    qInfo().noquote().nospace() << "Function - " << func << "()";
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
         std::as_const(Configuration::CONNECTIONS_TO_COUNT)
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

void TestOrm::logQueryCountersBlock(
            const QString &title, const qint64 queriesElapsed,
            const StatementsCounter statementsCounter,
            const bool recordsHaveBeenModified) const
{
    const auto loggingAppSummary = title.contains("Application");
    const auto &[
        functionsElapsedPrintable, queriesElapsedPrintable, statementsCounterPrintable,
        recordsHaveBeenModifiedPrintable
    ] = getAppCountersPrintable(
            loggingAppSummary, m_threadFunctionsElapsed, queriesElapsed,
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
        qInfo().noquote().nospace() << "⚡ Functions execution time : "
                                    << functionsElapsedPrintable << "\n";
    }

    // Threads mode (needed in the InvokeXTimes.ps1 script)
    if (loggingAppSummary)
        qInfo() << "♆ Threads mode             :"
                << (m_config.ConnectionsInThreads ? "multi-thread" : "single-thread");

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

void TestOrm::resetAllQueryLogCounters() const
{
    DB::resetElapsedCounters(Configuration::CONNECTIONS_TO_COUNT);
    DB::resetStatementCounters(Configuration::CONNECTIONS_TO_COUNT);

    for (const auto &connection : std::as_const(Configuration::CONNECTIONS_TO_COUNT))
        DB::forgetRecordModificationState(connection);
}

void TestOrm::enableAllQueryLogCounters() const
{
    // Create connections eagerly, so I can enable counters
    for (const auto &connection : std::as_const(Configuration::CONNECTIONS_TO_COUNT))
        DB::connection(connection);

    // BUG also decide how to behave when connection is not created and user enable counters silverqx
    // Enable counters on all database connections
    DB::enableElapsedCounters(Configuration::CONNECTIONS_TO_COUNT);
    DB::enableStatementCounters(Configuration::CONNECTIONS_TO_COUNT);
}

TestOrm::OrmConfigurationsType
TestOrm::computeConfigurationsToAdd(const QString &connection)
{
    // Connections in threads are disabled
    if (!m_config.ConnectionsInThreads)
        return configurationsWhenSingleThread();

    // All below - Connections in threads are enabled

    throwIfNonEmptyConn(connection);

    // Called from main thread
    if (!Support::g_inThread)
        return configsForMainThrdWhenMultiThrd();

    throwIfEmptyConn(connection);

    // Otherwise called from a non-main/worker thread
    return configsForWorkerThrdWhenMultiThrd(connection);
}

TestOrm::OrmConfigurationsType TestOrm::configurationsWhenSingleThread() const
{
    OrmConfigurationsType configurations;

    auto itConfig = m_configurations.constBegin();
    while (itConfig != m_configurations.constEnd()) {
        const auto &key = itConfig.key();

        if (key != "mysql_mainthread")
            configurations.insert(key, itConfig.value());

        ++itConfig;
    }

    return configurations;
}

TestOrm::OrmConfigurationsType TestOrm::configsForMainThrdWhenMultiThrd() const
{
    OrmConfigurationsType configurationsForMainThread;
    configurationsForMainThread.reserve(m_configurations.size());

    auto itConfig = m_configurations.constBegin();
    while (itConfig != m_configurations.constEnd()) {
        const auto &key = itConfig.key();
        const auto &value = itConfig.value();

        if (!m_config.ConnectionsToRunInThread.contains(key))
            configurationsForMainThread.insert(key, value);

        ++itConfig;
    }

    return configurationsForMainThread;
}


TestOrm::OrmConfigurationsType
TestOrm::configsForWorkerThrdWhenMultiThrd(const QString &connection) const
{
    const QStringList mappedConnections = getMappedConnections(connection);

    OrmConfigurationsType configurationsForWorkerThread;
    configurationsForWorkerThread.reserve(mappedConnections.size());

    for (const auto &connection : mappedConnections)
        configurationsForWorkerThread.insert(connection,
                                             m_configurations[connection]);

    return configurationsForWorkerThread;
}

QStringList TestOrm::computeConnectionsToCount(const QString &connection) const
{
    // CUR duplicate silverqx
    throwIfNonEmptyConn(connection);
    throwIfEmptyConn(connection);

    // Called from a non-main/worker thread
    if (!connection.isEmpty())
        return computeConnectionsToCountForWorkerThread(connection);

    // Otherwise called from main thread
    return computeConnectionsToCountForMainThread();
}

QStringList TestOrm::computeConnectionsToCountForMainThread() const
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
                (!m_config.ConnectionsInThreads &&
                 Configuration::CONNECTIONS_TO_TEST.contains(mappedConnection)) ||
                (m_config.ConnectionsInThreads &&
                 Configuration::CONNECTIONS_TO_TEST.contains(mappedConnection) &&
                 !m_config.ConnectionsToRunInThread.contains(mappedConnection));
    });

    /* Do it as a separate operation and not in std::copy_if() above, to not pollute
       logic, this is much cleaner. */
    connectionsToCount.removeOne("mysql_mainthread");

    return connectionsToCount;
}

QStringList
TestOrm::computeConnectionsToCountForWorkerThread(const QString &connection) const
{
    return getMappedConnections(connection);
}

QStringList TestOrm::getMappedConnections(const QString &connection) const
{
    if (m_config.ConnectionsMap.contains(connection))
        return m_config.ConnectionsMap.find(connection)->second;

    return {connection};
}

void TestOrm::santizeConnectionsToRunInThread()
{
    if (!m_config.ConnectionsInThreads)
        return;

    const auto removeEnd = std::remove_if(m_config.ConnectionsToRunInThread.begin(), // clazy:exclude=detaching-member
                                          m_config.ConnectionsToRunInThread.end(), // clazy:exclude=detaching-member
                                          [](const auto &connection)
    {
        return !Configuration::CONNECTIONS_TO_TEST.contains(connection);
    });

    m_config.ConnectionsToRunInThread.erase(removeEnd,
                                            m_config.ConnectionsToRunInThread.end()); // clazy:exclude=detaching-member
}

void TestOrm::initThreadLogging() const
{
    logThreadStream.setDevice(&logFile);
    openLogFile();
}

void TestOrm::openLogFile() const
{
    if (!m_config.ConnectionsInThreads || !m_config.IsLoggingToFile)
        return;

    if (!logFile.open(QFile::WriteOnly | QFile::Truncate))
        throw RuntimeError(
                QStringLiteral(
                    "Can not open log file 'logFile(%1)': %2 "
                    "[QFileDevice::FileError(%3)].")
                .arg(logFile.fileName(), logFile.errorString())
                .arg(logFile.error()));
}

void TestOrm::saveLogsFromThread(const ThreadName &threadName) const
{
    std::scoped_lock lock(mx_dataLogStream);

    /* Log to the file/std::vector, when logging to the file is disabled then
       the output will be logged to the console at the end of testAllConnections()
       to not pollute the console log. */
    if (m_config.IsLoggingToFile)
        logThreadStream << Support::g_logFromThread;
    else {
#ifdef __clang__
        logsFromThreads.push_back({threadName, Support::g_logFromThread});
#else
        logsFromThreads.emplace_back(threadName, Support::g_logFromThread);
#endif
        logsFromThreadsMap.emplace(threadName, logsFromThreads.size());
    }
}

void TestOrm::replayThrdLogToConsole()
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

void TestOrm::saveCountersForAppSummary(const ThreadName &threadName)
{
    std::scoped_lock lock(m_countersMutex);

    if (m_appCountersMap.contains(threadName))
        addCountersForAppSummary(threadName);
    else
        newCountersForAppSummary(threadName);
}

void TestOrm::newCountersForAppSummary(const ThreadName &threadName)
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

void TestOrm::addCountersForAppSummary(const ThreadName &threadName)
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
TinyPlay::TestOrm::countedConnectionsPrintable(const bool loggingAppSummary) const
{
    if (!loggingAppSummary)
        return Configuration::CONNECTIONS_TO_COUNT;

    static const QStringList cachedCountedConnections = appCountedConnectionsPrintable();
    return cachedCountedConnections;
}

QStringList TestOrm::appCountedConnectionsPrintable() const
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
                Configuration::CONNECTIONS_TO_TEST.contains(mappedConnection);
    });

    return countedConnections;
}

void TestOrm::throwIfNonEmptyConn(const QString &connection) const
{
    if (!Support::g_inThread && !connection.isEmpty())
        throw InvalidArgumentError(
                "The 'connection' argument is supported only when "
                "g_inThread = true.");
};

void TestOrm::throwIfEmptyConn(const QString &connection) const
{
    if (Support::g_inThread && connection.isEmpty())
        throw InvalidArgumentError(
                "You have to pass the 'connection' argument when g_inThread = true.");
};

void TestOrm::throwIfInThread() const
{
    if (Support::g_inThread)
        throw InvalidArgumentError(
                "This method can be called only from the main thread, when the "
                "g_inThread = false.");
}

void TestOrm::throwIfConnsToRunEmpty() const
{
    if (m_config.ConnectionsInThreads && m_config.ConnectionsToRunInThread.isEmpty())
        throw LogicError(
                "m_config.ConnectionsInThreads = true but "
                "m_config.ConnectionsToRunInThread is empty.");
}

void TestOrm::throwIfNoConfig(const QString &connection) const
{
    if (!m_configurations.contains(connection))
        throw InvalidArgumentError(
                QStringLiteral("m_configurations does not contain '%1' "
                               "configuration.")
                .arg(connection));
}

void TestOrm::throwIfAlreadyCalled() const
{
    if (m_db != nullptr)
        throw LogicError("TestOrm::connectToDatabase() can be called once only.");
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

QString TestOrm::appElapsedCounterPrintable(
        const std::vector<AppCounterItem<qint64>> &counters) const
{
    QStringList result;
    result.reserve(static_cast<int>(counters.size()));

    for (const auto &[threadName, value] : counters)
        result << countersElapsedTmpl.arg(value).arg(threadName);

    return result.join(COMMA);
}

QString TestOrm::appBoolCounterPrintable(
        const std::vector<AppCounterItem<bool>> &counters) const
{
    QStringList result;
    result.reserve(static_cast<int>(counters.size()));

    for (const auto &[threadName, value] : counters)
        result << countersTmpl.arg(value ? QStringLiteral("yes") : QStringLiteral("no"),
                                   threadName);

    return result.join(COMMA);
}

TestOrm::StatementsCounterPrintable
TestOrm::appStatementsCounterPrintable(
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

void TestOrm::appZeroCounters(
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

void TestOrm::appSumUpCounters(
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

TestOrm::CountersPrintable
TestOrm::getAppCountersPrintable(
        const bool isAppSummary, const qint64 functionsElapsed,
        const qint64 queriesElapsed, const StatementsCounter statementsCounter,
        const bool recordsHaveBeenModified) const
{
    // CUR improve this, it looks terrible, because function arguments are not used silverqx
    if (isAppSummary && m_config.ConnectionsInThreads)
        return {
            appElapsedCounterPrintable(m_appFunctionsElapsed),
            appElapsedCounterPrintable(m_appQueriesElapsed),
            appStatementsCounterPrintable(m_appStatementsCounter),
            appBoolCounterPrintable(m_appRecordsHaveBeenModified),
        };

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

} // namespace TinyPlay
