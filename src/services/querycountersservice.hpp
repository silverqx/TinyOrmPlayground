#pragma once
#ifndef TINYPLAY_SERVICES_QUERYCOUNTERSSERVICE_HPP
#define TINYPLAY_SERVICES_QUERYCOUNTERSSERVICE_HPP

#include <QHashFunctions>

#include <mutex>
#include <optional>
#include <unordered_map>

#include <orm/macros/threadlocal.hpp>
#include <orm/types/statementscounter.hpp>

#include "services/service.hpp"

namespace TinyPlay::Services
{
    /*! Service class for logging database connections query counters. */
    class QueryCountersService : public Service
    {
        Q_DISABLE_COPY(QueryCountersService)

    public:
        /*! Thread name type. */
        using ThreadName = QString;

        /*! Default constructor. */
        explicit QueryCountersService(Configuration &config);

        /*! Initialize the connections service for testing connections in threads. */
        void initConnsInThreadsTesting();

        /*! A common method, the main/worker thread version will be called based on
            whether the connection argument was passed. */
        QStringList computeConnectionsToCount(const QString &connection = "") const;

        /*! Enable elapsed/statement counters, connects to the database eagerly. */
        void enableAllQueryLogCounters() const;
        /*! Reset all counters on counted connections. */
        void resetAllQueryLogCounters() const;

        /*! Save log messages for a current thread, to the file or vector. */
        void saveLogsFromThread(const ThreadName &threadName) const;
        /*! Replay saved log messages to the console. */
        void replayThrdLogToConsole();

        /*! Save counters from a current thread for Application Summary. */
        void saveCountersForAppSummary(const ThreadName &threadName);

        /*! Log queries execution time counter and Query statements counter. */
        void logQueryCounters(
                const QString &func,
                std::optional<qint64> functionElapsed = std::nullopt);
        /*! Log whole application summary. */
        inline void logApplicationSummary() const;

    private:
        /*! Executed statements counter type. */
        using StatementsCounter = Orm::StatementsCounter;
        /*! Index of an AppCounterItem for counter vectors. */
        using AppCounterItemIdx = std::size_t;

        /*! Log a one Query statement counters. */
        void logQueryCountersBlock(
                const QString &title, qint64 queriesElapsed,
                StatementsCounter statementsCounter,
                bool recordsHaveBeenModified) const;

        /*! Compute connections to count based on whether threading is enabled. */
        QStringList computeConnectionsToCountForMainThread() const;
        /*! Compute connections to count for worker thread. */
        QStringList
        computeConnectionsToCountForWorkerThread(const QString &connection) const;

        /*! Allow to remove a connection from Configuration::ConnectionsToTest only and
            m_config.ConnectionsToRunInThread will be updated properly. */
        void santizeConnectionsToRunInThread();

        /*! Initialize thread logging to the file or string. */
        void initThreadLogging() const;
        /*! Open a log file to log output for connections in threads. */
        void openLogFile() const;

        /*! Push a new counters for current thread for Application Summary. */
        void newCountersForAppSummary(const ThreadName &threadName);
        /*! Add a counters for current thread for Application Summary. */
        void addCountersForAppSummary(const ThreadName &threadName);

        /*! Obtain counted connections for Application/Test summary. */
        const QStringList &
        countedConnectionsPrintable(bool loggingAppSummary) const;
        /*! Obtain counted connections for Application summary. */
        QStringList appCountedConnectionsPrintable() const;

        /*! Throw when compute connections to count was called from a worker thread. */
        void throwIfInThread() const;
        /*! Throw when connections in thread is enabled and connections to run are
            empty. */
        void throwIfConnsToRunEmpty() const;

        /* Counters for the Application Summary for the current thread */
        /*! Functions execution time for the whole for the current thread. */
        T_THREAD_LOCAL
        inline static qint64 m_threadFunctionsElapsed = 0;
        /*! Queries execution time for the current thread. */
        T_THREAD_LOCAL
        inline static qint64 m_threadQueriesElapsed = 0;
        /*! Counts executed statements in the current thread. */
        T_THREAD_LOCAL
        inline static StatementsCounter m_threadStatementsCounter {0, 0, 0};
        /*! Indicates whether changes have been made on any counted database connection
            in the current thread. */
        T_THREAD_LOCAL
        inline static bool m_threadRecordsHaveBeenModified = false;

        /* Whole application Counters for the Application Summary */
        /*! Application counter item. */
        template<typename T>
        struct AppCounterItem
        {
            /*! Determine to which thread counter belongs. */
            const ThreadName threadName {};
            /*! Counter value. */
            T value {};
        };

        /*! Printable executed statements counter. */
        struct StatementsCounterPrintable
        {
            /*! Normal select statements. */
            QString normal {};
            /*! Affecting statements (UPDATE, INSERT, DELETE). */
            QString affecting {};
            /*! Transactional statements (START TRANSACTION, ROLLBACK, COMMIT,
                SAVEPOINT). */
            QString transactional {};
            /*! Totals of all the above. */
            QString total {};
        };

        /*! All counters in printable format. */
        struct CountersPrintable
        {
            /*! Functions execution time for a given database connection. */
            QString functionsElapsed;
            /*! Queries execution time for a given database connection. */
            QString queriesElapsed;
            /*! Counts executed statements for a given database connection. */
            StatementsCounterPrintable statementsCounter;
            /*! Indicates whether changes have been made for a given database
                connection. */
            QString recordsHaveBeenModified;
        };

        /*! Executed statements counter with total sum up. */
        struct StatementsCounterTotal : public StatementsCounter
        {
            /*! Sum up of all counters. */
            qint64 total = -1;
        };

        /*! Obtain number counters in printable format. */
        QString appElapsedCounterPrintable(
                const std::vector<AppCounterItem<qint64>> &counters) const;
        /*! Obtain boolean counters in printable format. */
        QString appBoolCounterPrintable(
                const std::vector<AppCounterItem<bool>> &counters) const;
        /*! Obtain executed statements counter in printable format (for the whole
            application). */
        StatementsCounterPrintable
        appStatementsCounterPrintable(
                const std::vector<AppCounterItem<StatementsCounter>> &counters) const;
        /*! Set counters to 0 when any of the counters is enabled/counting. */
        void appZeroCounters(
                const std::vector<AppCounterItem<StatementsCounter>> &counters,
                StatementsCounterTotal &sumCounters) const;
        /*! Sum up the total and also sum up individual statement counters for the whole
            application. */
        void appSumUpCounters(
                const std::vector<AppCounterItem<StatementsCounter>> &counters,
                StatementsCounterTotal &sumCounters) const;

        /*! Obtain all counters in printable format for TinyPlay test summary. */
        CountersPrintable
        getAppCountersPrintable(
                qint64 functionsElapsed, qint64 queriesElapsed,
                StatementsCounter statementsCounter, bool recordsHaveBeenModified) const;
        /*! Obtain all counters in printable format for Application Summary. */
        CountersPrintable getAppCountersPrintable() const;

        /*! Functions execution time for the whole application. */
        std::vector<AppCounterItem<qint64>> m_appFunctionsElapsed {};
        /*! Queries execution time for the whole application. */
        std::vector<AppCounterItem<qint64>> m_appQueriesElapsed {};
        /*! Counts executed statements in the whole application. */
        std::vector<AppCounterItem<StatementsCounter>> m_appStatementsCounter {};
        /*! Indicates whether changes have been made on any counted database connection
            in the whole application. */
        std::vector<AppCounterItem<bool>> m_appRecordsHaveBeenModified {};
        /*! Fast lookup map for application counters. */
        std::unordered_map<ThreadName, AppCounterItemIdx> m_appCountersMap {};

        /*! Mutex used during counters processing. */
        mutable std::mutex m_countersMutex {};

        /*! Executed statements counter in printable format. */
        StatementsCounterPrintable m_appThreadCountersPrintable {};
    };

    void QueryCountersService::logApplicationSummary() const
    {
        // Whole application Summary
        logQueryCountersBlock(QStringLiteral("Application Summary"),
                              m_threadQueriesElapsed, m_threadStatementsCounter,
                              m_threadRecordsHaveBeenModified);
    }

} // namespace TinyPlay::Services

#endif // TINYPLAY_SERVICES_QUERYCOUNTERSSERVICE_HPP
