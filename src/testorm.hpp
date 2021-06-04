#ifndef TESTORM_H
#define TESTORM_H

#include <orm/databasemanager.hpp>
#include <orm/types/statementscounter.hpp>

class TestOrm
{
public:
//    using KeyType = quint64;

    TestOrm &connectToDatabase();
    TestOrm &run();

private:
    using StatementsCounter = Orm::StatementsCounter;

    void anotherTests();
    void testConnection();
    void testTinyOrm();
    void testQueryBuilder();
    void ctorAggregate();
    void jsonConfig();
    void standardPaths();

    /*! Log queries execution time counter and Query statements counter. */
    void logQueryCounters(
            const QString &func,
            const std::optional<qint64> functionElapsed = std::nullopt);
    /*! Log a one Query statement counters. */
    void logQueryCountersBlock(
            const QString &title, qint64 elapsed,
            const Orm::StatementsCounter statementsCounter,
            bool recordsHaveBeenModified) const;

    /*! Get the filepath to the SQLite database file, for testing the 'check_database_exists'
        configuration option. */
    static QString getCheckDatabaseExistsFile();

    /*! Reset all counters on counted connections. */
    void resetAllQueryLogCounters() const;

    /*! Path to the SQLite database file, for testing the 'check_database_exists'
        configuration option. */
    static const QString CHECK_DATABASE_EXISTS_FILE;

    /*! Connections to count on statements and execution times. */
    inline static const QStringList
    CONNECTIONS_TO_COUNT {"mysql", "mysql_alt", "sqlite", "sqlite_memory", "postgres"};
    /*! Connections, for which the testQueryBuilder() and testTinyOrm()
        will be ran on. */
    inline static const QStringList
    CONNECTIONS_TO_TEST {"mysql", "sqlite", "postgres"};

    /*! Database manager instance. */
    std::unique_ptr<Orm::DatabaseManager> m_db;

    /* Counters for the summary */
    /*! Queries execution time for the whole application. */
    int m_appFunctionsElapsed = 0;
    /*! Queries execution time for the whole application. */
    int m_appElapsed = 0;
    /*! Counts executed statements in the whole application. */
    StatementsCounter m_appStatementsCounter {0, 0, 0};
    /*! Indicates if changes have been made to the database for the whole application. */
    bool m_appRecordsHaveBeenModified = false;
};

#endif // TESTORM_H
