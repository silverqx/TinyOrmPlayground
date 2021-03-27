#ifndef TESTORM_H
#define TESTORM_H

#include "orm/databasemanager.hpp"

class TestOrm
{
public:
//    using KeyType = quint64;

    TestOrm &connectToDatabase();
    TestOrm &run();

private:
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
            const std::optional<qint64> elapsed = std::nullopt) const;
    /*! Log a one Query statement counters. */
    void logQueryCountersBlock(
            const QString &title, qint64 elapsed,
            const Orm::StatementsCounter &statementsCounter) const;

    /*! Path to the SQLite database file. */
    inline static const QString
    CHECK_DATABASE_EXISTS_FILE = "E:/SQLite/q_tinyorm_test-check_exists.sqlite3";

    /*! Connections to count on statements and execution times. */
    inline static const QStringList
    CONNECTIONS_TO_COUNT {"mysql", "mysql_alt", "sqlite", "sqlite_memory"};

    /*! Database manager instance. */
    std::unique_ptr<Orm::DatabaseManager> m_db;
};

#endif // TESTORM_H
