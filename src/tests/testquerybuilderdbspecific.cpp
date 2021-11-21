#include "tests/testquerybuilderdbspecific.hpp"

#include <QDebug>
#include <QElapsedTimer>

#include <orm/db.hpp>
#include <orm/query/querybuilder.hpp>

#include "configuration.hpp"

namespace TinyPlay::Tests
{

void TestQueryBuilderDbSpecific::run() const
{
    QElapsedTimer timer;
    timer.start();

    resetAllQueryLogCounters();

    qInfo().nospace()
            << "\n\n===================================="
            << "\n  QueryBuilder - database specific  "
            << "\n====================================";

    /* QueryBuilder::fromSub() - cross-database query */
    {
        qInfo() << "\n\nQueryBuilder::fromSub() - cross-database query\n---";

        auto query1 = DB::query(Configuration::Mysql_Laravel8)->from("users")
                     .select({"id", "name"});

        auto query = DB::query(m_config.MysqlMainThreadConnection)
                     ->fromSub(query1, "u")
                     .where("id", "<", 3)
                     .get();

        while (query.next())
            qDebug() << "id :" << query.value("id").value<quint64>() << ";"
                     << "name :" << query.value("name").value<QString>();

        qt_noop();
    }

    logQueryCounters(timer.elapsed());
}

} // namespace TinyPlay::Tests
