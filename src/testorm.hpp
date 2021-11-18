#pragma once
#ifndef TINYPLAY_TESTORM_H
#define TINYPLAY_TESTORM_H

#include <mutex>

#include <orm/databasemanager.hpp>
#include <orm/types/statementscounter.hpp>

#include "configuration.hpp"
#include "configurationsservice.hpp"
#include "connectionsservice.hpp"

namespace TinyPlay
{

    /*! Main playground class to test TinyORM library. */
    class TestOrm
    {
        Q_DISABLE_COPY(TestOrm)

    public:
//        using KeyType = quint64;
        /*! Thread name type. */
        using ThreadName = QString;

        /*! Default constructor. */
        TestOrm();

        /*! Create connections, connect to DB and enable connections counters. */
        TestOrm &connectToDatabase();
        /*! Fire up TinyOrmPlayground. */
        TestOrm &run();

        /*! Get TinyOrmPlayground Configuration. */
        inline const Configuration &config() const;

    private:
        /*! Executed statements counter type. */
        using StatementsCounter = Orm::StatementsCounter;
        /*! Index of an AppCounterItem for counter vectors. */
        using AppCounterItemIdx = std::size_t;

        /*! Main testing method. */
        void testAllConnections();
        /*! Testing method used when database connection will run in the main thread. */
        void testConnectionInMainThread(const QString &connection);
        /*! Testing method used when database connection will run in a worker thread. */
        void testConnectionInWorkerThread(const QString &connection);

    public:
        /*! Log queries execution time counter and Query statements counter. */
        void logQueryCounters(
                const QString &func,
                std::optional<qint64> functionElapsed = std::nullopt);
    private:
        /*! Log a one Query statement counters. */
        void logQueryCountersBlock(
                const QString &title, qint64 queriesElapsed,
                Orm::StatementsCounter statementsCounter,
                bool recordsHaveBeenModified) const;

    public:
        /*! Reset all counters on counted connections. */
        void resetAllQueryLogCounters() const;
    private:
        /*! Enable elapsed/statement counters, connects to the database eagerly. */
        void enableAllQueryLogCounters() const;

        /*! A common method, the main/worker thread version will be called based on
            whether the connection argument was passed. */
        QStringList computeConnectionsToCount(const QString &connection = "") const;
        /*! Compute connections to count based on whether threading is enabled. */
        QStringList computeConnectionsToCountForMainThread() const;
        /*! Compute connections to count for worker thread. */
        QStringList
        computeConnectionsToCountForWorkerThread(const QString &connection) const;

        /*! Obtain mapped connections for a given connection. */
        QStringList getMappedConnections(const QString &connection) const;

        /*! Allow to remove a connection from Configuration::CONNECTIONS_TO_TEST only and
            m_config.ConnectionsToRunInThread will be updated properly. */
        void santizeConnectionsToRunInThread();

        /*! Initialize thread logging to the file or string. */
        void initThreadLogging() const;
        /*! Open a log file to log output for connections in threads. */
        void openLogFile() const;
        /*! Save log messages for a current thread, to the file or vector. */
        void saveLogsFromThread(const ThreadName &threadName) const;
        /*! Replay saved log messages to the console. */
        void replayThrdLogToConsole();

        /*! Save counters from a current thread for Application Summary. */
        void saveCountersForAppSummary(const ThreadName &threadName);
        /*! Push a new counters for current thread for Application Summary. */
        void newCountersForAppSummary(const ThreadName &threadName);
        /*! Add a counters for current thread for Application Summary. */
        void addCountersForAppSummary(const ThreadName &threadName);

        /*! Obtain counted connections for Application/Test summary. */
        const QStringList &
        countedConnectionsPrintable(bool loggingAppSummary) const;
        /*! Obtain counted connections for Application summary. */
        QStringList appCountedConnectionsPrintable() const;

        /*! Throw when a connection parameter was passed in the main thread. */
        void throwIfNonEmptyConn(const QString &connection) const;
        /*! Throw when a connection parameter was not passed in a worker thread. */
        void throwIfEmptyConn(const QString &connection) const;
        /*! Throw when compute connections to count was called from a worker thread. */
        void throwIfInThread() const;
        /*! Throw when connections in thread is enabled and connections to run are
            empty. */
        void throwIfConnsToRunEmpty() const;
        /*! Throw when m_configurations hash does not contain a given connection. */
        void throwIfNoConfig(const QString &connection) const;
        /*! Throw if connectToDatabase() was already called. */
        void throwIfAlreadyCalled() const;

        /*! Configuration for TinyOrmPlayground. */
        Configuration m_config;

        /*! Database connection configurations service. */
        ConfigurationsService m_configurationsService;

        /*! Database connections service. */
        ConnectionsService m_connectionsManager;

        /*! Database manager instance. */
        std::unique_ptr<Orm::DatabaseManager> m_db = nullptr;

        /* Counters for the Application Summary for the current thread */
        /*! Functions execution time for the whole for the current thread. */
        thread_local
        inline static qint64 m_threadFunctionsElapsed = 0;
        /*! Queries execution time for the current thread. */
        thread_local
        inline static qint64 m_threadQueriesElapsed = 0;
        /*! Counts executed statements in the current thread. */
        thread_local
        inline static StatementsCounter m_threadStatementsCounter {0, 0, 0};
        /*! Indicates whether changes have been made on any counted database connection
            in the current thread. */
        thread_local
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

        /*! Obtain all counters in printable format. */
        CountersPrintable
        getAppCountersPrintable(
                bool isAppSummary, qint64 functionsElapsed, qint64 queriesElapsed,
                StatementsCounter statementsCounter, bool recordsHaveBeenModified) const;

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

    const Configuration &TestOrm::config() const
    {
        return m_config;
    }

} // namespace TinyPlay

#endif // TINYPLAY_TESTORM_H
