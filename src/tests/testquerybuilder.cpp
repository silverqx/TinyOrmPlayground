#include "tests/testquerybuilder.hpp"

#include <QDebug>
#include <QElapsedTimer>

#include <orm/db.hpp>
#include <orm/query/joinclause.hpp>
#include <orm/query/querybuilder.hpp>

#include "configuration.hpp"

// clazy:excludeall=unused-non-trivial-variable

using Orm::DB;

namespace TinyPlay::Tests
{

void TestQueryBuilder::run() const
{
    QElapsedTimer timer;
    timer.start();

    resetAllQueryLogCounters();

    qInfo().nospace()
            << "\n\n================"
            << "\n  QueryBuilder  "
            << "\n================";

//    /* RANGES range-v3 lib */
//    auto v = std::vector<std::string> {"apple", "banana", "kiwi"};
//    for (auto&& [first, second] : v | ranges::views::enumerate)
//         qDebug() << first << ", " << QString::fromStdString(second);
//    qt_noop();

////    std::vector<int> numbers = { 1, 2, 3, 4, 5, 6 };
//    QVector<int> numbers { 1, 2, 3, 4, 5, 6 };

//    auto results = numbers
//                   | ranges::views::filter([](int n){ return n % 2 == 0; })
//                   | ranges::views::transform([](int n){ return n * 2; });

//    for (const auto &v : results)
//        qDebug() << v;
//    qt_noop();

//    /* QT PROPERTY system */
//    MyClass *myinstance = new MyClass;
//    auto r1 = myinstance->setProperty("name", 10);
//    auto v1 = myinstance->property("name");
//    auto dp = myinstance->dynamicPropertyNames();
//    for (auto &prop : dp)
//        qDebug() << prop;
//    qt_noop();

    /* distinct, limit, offset and ordering  */
//    {
//        auto query = m_db.query()->from("torrents")
//                .distinct()
////                .where("progress", ">", 100)
//                .limit(5)
//                .offset(2)
////                .forPage(2, 5)
//                .orderBy("name")
//                .reorder()
//                .orderBy("id", "desc")
////                .orderByDesc("size")
//                .get();
//        qDebug() << "FIRST :" << query.executedQuery();
//        while (query.next()) {
//            qDebug() << "id :" << query.value("id")
//                     << "; name :" << query.value("name");
//        }
//        qt_noop();
//    }

//    auto [ok, b] = m_db.query()->from("torrents").get({"id, name"});
//    qDebug() << "SECOND :" << b.executedQuery();
//    while (b.next()) {
//        qDebug() << "id :" << b.value("id") << "; name :" << b.value("name");
//    }
//    qt_noop();

    /* WHERE */
//    auto [ok, c] = m_db.query()->from("torrents")
//             .where("name", "=", "test2", "and")
//             .get({"id", "name", "created_at"});
//    qDebug() << "THIRD :" << c.executedQuery();
//    while (c.next()) {
//        qDebug() << "id :" << c.value("id") << "; name :" << c.value("name")
//                 << "; created_at :" << c.value("created_at");
//    }
//    qt_noop();

    /* WHERE - like */
//    {
//        auto [ok, c] = m_db.query()->from("torrents")
//                .where("name", "like", "%no peers", "and")
//                .get({"id", "name"});
//        qDebug() << "THIRD :" << c.executedQuery();
//        while (c.next()) {
//            qDebug() << "id :" << c.value("id") << "; name :" << c.value("name");
//        }
//        qt_noop();
//    }

    /* also nested where */
//    auto [ok, d] = m_db.query()->from("torrents")
//             .where("name", "=", "aliens", "and")
////             .where("id", "=", 1, "and")
////             .where("id", "=", 262, "and")
////             .where("id", "=", 261, "or")
//             .where([](auto &query)
//    {
////        query.where("id", "=", 261, "or");
//        query.where("id", "=", 258, "or");
////        query.where("name", "=", "Most II", "or");
//        query.where([](auto &query)
//        {
//            query.where("id", "=", 5);
//            query.where("name", "=", "Transformers 2");
//        }, "or");
//    }, "or")
//             .get({"id, name"});
//    qDebug() << "FOURTH :" << d.executedQuery();
//    while (d.next()) {
//        qDebug() << "id :" << d.value("id") << "; name :" << d.value("name");
//    }
//    qt_noop();

    // whereIn
//    auto [ok_c1, c1] = m_db.query()->from("torrents")
//             .where("name", "=", "Internat - S1", "and")
//             .get({"id", "name"});
//    qDebug() << "whereIn :" << c1.executedQuery();
//    while (c1.next()) {
//        qDebug() << "id :" << c1.value("id") << "; name :" << c1.value("name");
//    }
//    qt_noop();

    /* where() - an array of basic where clauses */
//    {
//        auto query = m_db.query()->from("torrents")
//                .where({
//                    {"size", 13, ">="},
//                    {"progress", 500, ">="},
//                    {"progress", 700, "<"/*, "or"*/},
//                }, "and")
//                .get();

//        while (query.next()) {
//            qDebug() << "id :" << query.value("id")
//                     << "; name :" << query.value("name");
//        }

//        qt_noop();
//    }

    /* where() - an array of where clauses comparing two columns */
    {
//        auto query = m_db.query()->from("torrents")
//                .whereColumn({
//            {"size", "progress", ">"},
////            {"progress", "size", ">="},
////            {"progress", "size", "<", "or"},
//        }, "and")
//        .get();

//        while (query.next()) {
//            qDebug() << "id :" << query.value("id")
//                     << "; name :" << query.value("name");
//        }

//        qt_noop();
    }

    /* QueryBuilder::leftJoin() */
    {
        qInfo() << "\n\nQueryBuilder::leftJoin()\n---";

        auto torrents =
                DB::table("torrents")
                ->leftJoin("torrent_previewable_files",
                           "torrents.id", "=", "torrent_previewable_files.torrent_id")
                .where("torrents.id", "=", 2)
                .get({"torrents.id", "name", "file_index", "filepath"});

        while (torrents.next())
            qDebug() << "id :" << torrents.value("id")
                     << "; name :" << torrents.value("name")
                     << "file_index :" << torrents.value("file_index")
                     << "filepath :" << torrents.value("filepath");
    }

    /* QueryBuilder::join() - Advanced Join Clause */
    {
        qInfo() << "\n\nQueryBuilder::join() - Advanced Join Clause\n---";

        auto torrents = DB::table("torrents")
                        ->join("torrent_previewable_files", [](auto &join)
        {
            join.on("torrents.id", "=", "torrent_previewable_files.torrent_id")
                    .where("torrents.progress", "<", 350);
        })
//             .where("torrents.id", "=", 256)
            .get({"torrents.id", "name", "file_index", "torrents.progress", "filepath"});

        while (torrents.next())
            qDebug() << "id :" << torrents.value("id")
                     << "; name :" << torrents.value("name")
                     << "file_index :" << torrents.value("file_index")
                     << "progress :" << torrents.value("progress")
                     << "filepath :" << torrents.value("filepath");

        qt_noop();
    }

    /* QueryBuilder::join() - Advanced Join Clause with nested where */
    {
        qInfo() << "\n\nQueryBuilder::join() - Advanced Join Clause "
                   "with nested where\n---";

        auto torrents = DB::table("torrents")
                        ->join("torrent_previewable_files", [](auto &join)
        {
            join.on("torrents.id", "=", "torrent_previewable_files.torrent_id")
                    .where([](auto &query)
            {
                query.where("torrents.progress", "<", 550)
                        .where("torrents.progress", ">", 250);
            });
        })
            .get({"torrents.id", "name", "file_index", "torrents.progress", "filepath"});

        while (torrents.next())
            qDebug() << "id :" << torrents.value("id")
                     << "; name :" << torrents.value("name")
                     << "file_index :" << torrents.value("file_index")
                     << "progress :" << torrents.value("progress")
                     << "filepath :" << torrents.value("filepath");

        qt_noop();
    }

    /* QueryBuilder::first() */
    {
        qInfo() << "\n\nQueryBuilder::first()\n---";

        auto torrent = DB::table("torrents")
                       ->where("torrents.id", "=", 2)
                       .first({"id", "name"});

        qDebug() << "id :" << torrent.value("id").value<quint64>()
                 << "; name :" << torrent.value("name").value<QString>();

        qt_noop();
    }

    /* QueryBuilder::value() */
    {
        qInfo() << "\n\nQueryBuilder::value()\n---";

        auto name = DB::table("torrents")
                    ->where("torrents.id", "=", 2)
                    .value("name");

        qDebug() << "name:" << name.value<QString>();

        qt_noop();
    }

    /* QueryBuilder::find() */
    {
        qInfo() << "\n\nQueryBuilder::find()\n---";

        auto torrent = DB::table("torrents")->find(3, {"id", "name"});

        qDebug() << "id :" << torrent.value("id").value<quint64>()
                 << "; name :" << torrent.value("name").value<QString>();

        qt_noop();
    }

    /* QueryBuilder::groupBy()/having() without aggregate */
    {
        qInfo() << "\n\nQueryBuilder::groupBy()/having() without aggregate\n---";

        auto torrents = DB::table("torrent_previewable_files")
                        ->select("torrent_id")
                        .whereNotNull("torrent_id")
                        .groupBy("torrent_id")
                        .having("torrent_id", "<", 4)
                        .get();

        while (torrents.next())
            qDebug() << "torrent_id :" << torrents.value("torrent_id").value<quint64>();

        qt_noop();
    }

    /* INSERTs */
//    {
//        auto id = m_db.query()->from("torrents").insertGetId(
//                        {{"name", "first"}, {"size", 2048}, {"progress", 300},
//                         {"hash", "xxxx61defa3daecacfde5bde0214c4a439351d4d"},
//                         {"created_at", QDateTime().currentDateTime()}});
////                         {"created_at", QDateTime().currentDateTime()
////                                                   .toString(Qt::ISODate)}});
//        qDebug() << "TENTH";
//        qDebug() << "last id :" << id;

//        qt_noop();
//    }

    // Insert with boolean value, Qt converts it to int
//    {
//        auto q = DB::table("torrents")->insert(
//                     {{"name", "bool test"}, {"size", 2048}, {"progress", 300},
////                      {"added_on", QDateTime().currentDateTime()
////                                              .toString(Qt::ISODate)},
//                      {"hash", "xxxx61defa3daecacfde5bde0214c4a439351d4d"},
//                      {"bool", false}});
//        qt_noop();
//    }

    // Empty attributes
//    {
//        auto id = m_db.query()->from("settings").insertGetId({});
//        qDebug() << "Insert - empty attributes";
//        qDebug() << "last id :" << id;
//        qt_noop();
//    }

//    const auto id_i = 278;
//    auto j = m_db.query()->from("torrent_previewable_files").insert({
//        {{"torrent_id", id_i}, {"file_index", 0}, {"filepath", "abc.mkv"},
//         {"size", 2048}, {"progress", 10}},
//        {{"torrent_id", id_i}, {"file_index", 1}, {"filepath", "xyz.mkv"},
//         {"size", 1024}, {"progress", 15}}});
//    qDebug() << "ELEVEN :" << j->executedQuery();
//    if (ok_j) {
//        qDebug() << "last id :" << j->lastInsertId()
//                 << "; affected rows :" << j->numRowsAffected();
//    }
//    qt_noop();

//    auto k = m_db.query()->from("torrent_previewable_files").insert({
//        {"torrent_id", id_i}, {"file_index", 2}, {"filepath", "qrs.mkv"}, {"size", 3074},
//        {"progress", 20}});
//    qDebug() << "TWELVE :" << k->executedQuery();
//    if (ok_k) {
//        qDebug() << "last id :" << k->lastInsertId()
//                 << "; affected rows :" << k->numRowsAffected();
//    }
//    qt_noop();

    /* QueryBuilder::insertOrIgnore() */
    {
        qInfo() << "\n\nQueryBuilder::insertOrIgnore()\n---";

        const auto torrentId = 5;
        auto query1 = DB::table("torrent_previewable_files")->insert({
            {"torrent_id", torrentId}, {"file_index", 3}, {"filepath", "qrs.mkv"},
                {"size", 3074}, {"progress", 20}
        });

        auto [affected, query] = DB::table("torrent_previewable_files")
                ->insertOrIgnore(
        {
            {{"torrent_id", torrentId}, {"file_index", 3}, {"filepath", "qrs.mkv"},
                {"size", 3074}, {"progress", 20}},
            {{"torrent_id", torrentId}, {"file_index", 4}, {"filepath", "ghi.mkv"},
                {"size", 3074}, {"progress", 20}},
            {{"torrent_id", torrentId}, {"file_index", 5}, {"filepath", "def.mkv"},
                {"size", 3074}, {"progress", 20}},
        });
        Q_ASSERT(affected == 2);

        qt_noop();
    }

    /* Tests of update/delete with limit/join */
    {
        qInfo() << "\n\nTests of update/delete with limit/join\n---";

        auto qrsFileIdMysql = DB::table("torrent_previewable_files")
                              ->whereEq("filepath", "qrs.mkv").value("id");

        /* QueryBuilder::update() */
        {
            qInfo() << "\n\nQueryBuilder::update()\n---";

            auto [affected, _] = DB::table("torrent_previewable_files")
                    ->where("id", "=", qrsFileIdMysql)
                    .update({{"filepath", "qrs-update.mkv"}, {"progress", 890}});
            Q_ASSERT(affected == 1);

            qt_noop();
        }

        /* QueryBuilder::update() - with join */
        {
            qInfo() << "\n\nQueryBuilder::update() - with join\n---";

            auto query = DB::table("torrents")
                    ->join("torrent_previewable_files", "torrents.id", "=",
                           "torrent_previewable_files.torrent_id")
                    .where("torrents.id", "=", 5)
                    .where("torrent_previewable_files.id", "=", qrsFileIdMysql);

            if (DB::getDefaultConnection() == Mysql) {
                auto [affected, _] = query.update({
                    {"name", "test5 update join"}, {"torrents.progress", 503},
                    {"torrent_previewable_files.progress", 891}
                });
                Q_ASSERT(affected == 2);
            } else {
                auto [affected, _] = query.update({
                    {"name", "test5 update join"}, {"torrents.progress", 503},
                });
                Q_ASSERT(affected == 1);
            }

            qt_noop();
        }

        /* QueryBuilder::update() - with limit */
        {
            qInfo() << "\n\nQueryBuilder::update() - with limit\n---";

            auto [affected, _] = DB::table("torrent_previewable_files")
                    ->where("torrent_id", "=", 5)
                    .where("file_index", ">", 2)
                    .limit(2)
                    .update({{"progress", 892}});
            Q_ASSERT(affected == 2);

            qt_noop();
        }

        /* QueryBuilder::remove() - with join */
//        {
//            qInfo() << "\n\nQueryBuilder::remove() - with join\n---";

//            auto [affected, _] = DB::table("torrents")
//                    ->join("torrent_previewable_files", "torrents.id", "=",
//                           "torrent_previewable_files.torrent_id")
//                    .where("torrents.id", "=", 5)
//                    .where("torrent_previewable_files.id", "=", qrsFileIdMysql)
//                    .remove();
//            Q_ASSERT(affected == 1);

//            qt_noop();
//        }

        /* SQLite behaves differently here, while MySQL respects constrain ON DELETE
           CASCADE, SQLite only removes rows selected by the WHERE clause
           in the joined table.
           So I comment on these two examples and uncomment them when I will to want test
           this DELETE with join statements. */
        /* QueryBuilder::remove() [sqlite] - with join */
//        {
//            qInfo("\n\nQueryBuilder::remove() [%s] - with join\n---",
//                  qUtf8Printable(Sqlite));

//            auto [affected, _] = DB::table("torrents", Sqlite)
//                    ->join("torrent_previewable_files", "torrents.id", "=",
//                    "torrent_previewable_files.torrent_id")
//                    .where("torrents.id", "=", 5)
//                    .where("torrent_previewable_files.id", "=", qrsFileIdSqlite)
//                    .remove();
//            Q_ASSERT(affected == 1);

//            qt_noop();
//        }

        // Also part of the Restore
        /* QueryBuilder::remove() - with limit */
        {
            qInfo() << "\n\nQueryBuilder::remove() - with limit\n---";

            auto [affected, _] = DB::table("torrent_previewable_files")
                    ->where("torrent_id", "=", 5)
                    .where("file_index", ">", 2)
                    .limit(2)
                    .remove();
            Q_ASSERT(affected == 2);

            qt_noop();
        }

        // Restore
        {
            DB::table("torrent_previewable_files")
                    ->where("torrent_id", "=", 5)
                    .where("file_index", ">", 2)
                    .remove();
        }
    }

    /* QueryBuilder::update() - restore from previous updates */
    {
        qInfo() << "\n\nQueryBuilder::update() - restore from previous updates\n---";

        {
            auto [affected, _] = DB::table("torrents")
                    ->where("torrents.id", "=", 5)
                    .update({{"name", "test5"}, {"progress", 500}});
            Q_ASSERT(affected == 1);
        }

        qt_noop();
    }

    /* EXPRESSIONs */
//    Expression aa("first1");
//    QVariant x;
//    x.setValue(aa);
//    QVariant x {Expression("first1")};
//    QVariant x = Expression("first1");
//    QVariant x = QVariant::fromValue(Expression("first1"));
//    qDebug() << "text :" << x;
//    qDebug() << "type :" << x.type();
//    qDebug() << "typeName :" << x.typeName();
//    qDebug() << "userType :" << x.userType();
//    qt_noop();

//    auto [affected_o, o] = m_db.query()->from("torrents")
//            .where("id", "=", 277)
//            .update({{"name", QVariant::fromValue(Expression("first"))},
//                     {"progress", 350}});
////            .update({{"name", x}, {"progress", 350}});
//    qDebug() << "FIFTEEN :" << o.executedQuery();
//    qDebug() << "affected rows :" << affected_o;
//    qt_noop();

    /* INCREMENT / DECREMENT */
//    auto [affected_p, p] = m_db.query()->from("torrents")
//            .whereEq("id", 277)
////            .increment("progress", 1);
//            .decrement("progress", 1, {
//                {"seeds", 6}
//            });
//    qDebug() << "SIXTEEN :" << p.executedQuery();
//    qDebug() << "affected rows :" << affected_p;
//    qt_noop();

    /* DELETEs */
//    auto [affected_q, q] = m_db.query()->from("torrent_previewable_files")
//            .whereEq("id", 107)
//            .remove();
//    qDebug() << "SEVENTEEN :" << q.executedQuery();
//    qDebug() << "affected rows :" << affected_q;
//    qt_noop();

//    auto [affected_r, r] = m_db.query()->from("torrents")
//            .join("torrent_previewable_files", "torrents.id", "=",
//                  "torrent_previewable_files.torrent_id")
//            .whereEq("torrents.id", 7)
//            .whereEq("torrent_previewable_files.id", 1006)
//            .remove();
//    qDebug() << "EIGHTEEN :" << r.executedQuery();
//    qDebug() << "affected rows :" << affected_r;
//    qt_noop();

    /* TRUNCATE */
//    auto [ok_s, s] = m_db.query()->from("xxx")
//            .truncate();
//    qDebug() << "NINETEEN :" << s.executedQuery();
//    ok_s ? qDebug() << "truncate was successful"
//            : qDebug() << "truncate was unsuccessful";
    //    qt_noop();

    /* QueryBuilder::fromRaw() */
    {
        qInfo() << "\n\nQueryBuilder::fromRaw()\n---";

        auto query = DB::query()
                     ->fromRaw("(select id, name from torrents where id < ?) as t", {5})
                     .where("id", "<", 3)
                     .get();

        while (query.next())
            qDebug() << "id :" << query.value("id").value<quint64>() << ";"
                     << "name :" << query.value("name").value<QString>();

        qt_noop();
    }

    /* QueryBuilder::joinSub() */
    {
        qInfo() << "\n\nQueryBuilder::joinSub()\n---";

        auto query = DB::table("torrents")->joinSub([](auto &query_)
        {
            query_.from("torrent_previewable_files")
                    .select({"id as files_id", "torrent_id", "filepath",
                             "size as files_size"})
                    .where("size", "<", 2050);
        }, "files", "torrents.id", "=", "files.torrent_id", "inner")

                .where("progress", "<", 500)
                .get();

        while (query.next())
            qDebug() << "id :" << query.value("id").value<quint64>() << ";"
                     << "progress :" << query.value("progress").value<int>() << ";"
                     << "filepath :" << query.value("filepath").value<QString>() << ";"
                     << "files_size :" << query.value("files_size").value<qint64>();

        qt_noop();
    }

    /* QueryBuilder::leftJoinSub() */
    {
        qInfo() << "\n\nQueryBuilder::leftJoinSub()\n---";

        auto query = DB::table("torrents")->leftJoinSub([](auto &query_)
        {
            query_.from("torrent_previewable_files")
                    .select({"id as files_id", "torrent_id", "filepath",
                             "size as files_size"})
                    .where("size", "<", 2050);
        }, "files", "torrents.id", "=", "files.torrent_id")

                .where("progress", "<", 500)
                .get();

        while (query.next())
            qDebug() << "id :" << query.value("id").value<quint64>() << ";"
                     << "progress :" << query.value("progress").value<int>() << ";"
                     << "filepath :" << query.value("filepath").value<QString>() << ";"
                     << "files_size :" << query.value("files_size").value<qint64>();

        qt_noop();
    }

    /* QueryBuilder::join() - table as expression */
    {
        qInfo() << "\n\nQueryBuilder::join() - table as expression\n---";

        auto query = DB::table("torrents")
                ->join(DB::raw("(select id as files_id, torrent_id, filepath, "
                                 "size as files_size "
                               "from torrent_previewable_files "
                               "where size < 2050) as files"),
                       "torrents.id", "=", "files.torrent_id", "inner")
                .where("progress", "<", 500)
                .get();

        while (query.next())
            qDebug() << "id :" << query.value("id").value<quint64>() << ";"
                     << "progress :" << query.value("progress").value<int>() << ";"
                     << "filepath :" << query.value("filepath").value<QString>() << ";"
                     << "files_size :" << query.value("files_size").value<qint64>();

        qt_noop();
    }

    /* QueryBuilder::join() - with callback and table as expression */
    {
        qInfo() << "\n\nQueryBuilder::join() - with callback and table as "
                   "expression\n---";

        auto query = DB::table("torrents")
                     ->join(DB::raw("(select id as files_id, torrent_id, filepath, "
                                      "size as files_size "
                                    "from torrent_previewable_files "
                                    "where size < 2050) as files"),
                            [](auto &join)
        {
            join.on("torrents.id", "=", "files.torrent_id");
        }, "inner")
                     .where("progress", "<", 500)
                     .get();

        while (query.next())
            qDebug() << "id :" << query.value("id").value<quint64>() << ";"
                 << "progress :" << query.value("progress").value<int>() << ";"
                 << "filepath :" << query.value("filepath").value<QString>() << ";"
                 << "files_size :" << query.value("files_size").value<qint64>();

        qt_noop();
    }

    /* QueryBuilder::joinWhere() */
    {
        qInfo() << "\n\nQueryBuilder::joinWhere()\n---";

        /* This is strange query, but it works like this, select torrent.id = 2 and
           join to it every file with f_progress < 300. 👀 */
        auto query = DB::table("torrents")
                     ->joinWhere("torrent_previewable_files", "torrents.id", "=", 2)
                     .where("torrent_previewable_files.progress", "<", 300)
                     .get({"torrents.id as t_id",
                           "torrent_previewable_files.id as f_id",
                           "torrent_previewable_files.filepath",
                           "torrent_previewable_files.progress as f_progress"});

        while (query.next())
            qDebug() << "t_id :" << query.value("t_id").value<quint64>() << ";"
             << "f_id :" << query.value("f_id").value<quint64>() << ";"
             << "f_progress :" << query.value("f_progress").value<int>() << ";"
             << "filepath :" << query.value("filepath").value<QString>();

        qt_noop();
    }

    /* QueryBuilder::joinWhere() - table as expression */
    {
        qInfo() << "\n\nQueryBuilder::joinWhere() - table as expression\n---";

        /* This is strange query, but it works like this, select torrent.id = 2 and
           join to it every file (from subquery size < 2050) with f_progress < 300. 👀 */
        auto query = DB::table("torrents")
                     ->joinWhere(DB::raw("(select id as f_id, torrent_id, filepath, "
                                           "size as f_size, progress as f_progress "
                                         "from torrent_previewable_files "
                                         "where size < 2050) as files"),
                                 "torrents.id", "=", 2)
                     .where("files.f_progress", "<", 500)
                     .get({"torrents.id as t_id", "f_id", "files.filepath", "f_size",
                           "f_progress"});

        while (query.next())
            qDebug() << "t_id :" << query.value("t_id").value<quint64>() << ";"
                     << "f_id :" << query.value("f_id").value<quint64>() << ";"
                     << "f_progress :" << query.value("f_progress").value<int>() << ";"
                     << "f_size :" << query.value("f_size").value<qint64>() << ";"
                     << "filepath :" << query.value("filepath").value<QString>();

        qt_noop();
    }

    /* Column expressions in QueryBuilder::select() methods */
    {
        qInfo() << "\n\nColumn expressions in QueryBuilder::select() methods\n---";

        auto r1 = DB::table("torrents")->whereEq("id", 1).value("name");
        auto r2 = DB::table("torrents")->whereEq("id", 1).value(DB::raw("name"));
        auto r3 = DB::table("torrents")->whereEq("id", 1).get({"id", "name"});
        auto r4 = DB::table("torrents")->whereEq("id", 1).get({"id", DB::raw("name")});
        auto r5 = DB::table("torrents")->get({"id", "name"});
        auto r6 = DB::table("torrents")->get({"id", DB::raw("name")});

        auto r7 = DB::table("torrents")->select("id").whereEq("id", 1).get();
        auto r8 = DB::table("torrents")->select(DB::raw("id")).whereEq("id", 1).get();
        auto r9 = DB::table("torrents")->select({"id", "name"}).whereEq("id", 1).get();
        auto r10 = DB::table("torrents")->select({"id", DB::raw("name")}).whereEq("id", 1).get();
        auto r11 = DB::table("torrents")->addSelect("id").whereEq("id", 1).get();
        auto r12 = DB::table("torrents")->addSelect(DB::raw("id")).whereEq("id", 1).get();
        auto r13 = DB::table("torrents")->addSelect({"id", "name"}).whereEq("id", 1).get();
        auto r14 = DB::table("torrents")->addSelect({"id", DB::raw("name")}).whereEq("id", 1).get();

        qt_noop();
    }

    /* Aggregates */
    {
        qInfo() << "\n\nAggregates\n---";

        auto r1 = DB::table("torrents")->min("size");
        auto r2 = DB::table("torrents")->max("size");
        auto r3 = DB::table("torrents")->sum("size");
        auto r4 = DB::table("torrents")->avg("size");
        auto r5 = DB::table("torrents")->average("size");

        [[maybe_unused]]
        auto r6 = DB::table("torrents")->count("size");
        [[maybe_unused]]
        auto r7 = DB::table("torrents")->count({"size"});
        // MySQL only, I leave it here, I will not move it to testQueryBuilderDbSpecific()
        if (DB::getDefaultConnection() == Mysql)
            [[maybe_unused]]
            auto r8 = DB::table("torrent_previewable_files")
                      ->distinct().count({"torrent_id", "note"});
        [[maybe_unused]]
        auto r9 = DB::table("torrents")->distinct().count("size");
        [[maybe_unused]]
        auto r10 = DB::table("torrents")->distinct().count({"size"});

        qt_noop();
    }

    logQueryCounters(timer.elapsed());
}

} // namespace TinyPlay::Tests
