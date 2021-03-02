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
    void testQueryBuilder();
    void testTinyOrm();
    void ctorAggregate();
    void jsonConfig();
    void standardPaths();

    /*! Log queries execution time counter and Query statements counter. */
    void logQueryCounters(
            const QString &func,
            const std::optional<qint64> elapsed = std::nullopt) const;

    std::unique_ptr<Orm::DatabaseManager> m_db;
};

#endif // TESTORM_H
