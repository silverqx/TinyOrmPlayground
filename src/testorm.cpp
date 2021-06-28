#include "testorm.hpp"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QLibraryInfo>
#include <QStandardPaths>

#ifdef _MSC_VER
#include <format>
#endif
#include <fstream>
#include <iomanip>
#include <iostream>
#include <typeindex>
#include <typeinfo>

#include <nlohmann/json.hpp>
#include <range/v3/all.hpp>

#include <orm/db.hpp>
#include <orm/invalidargumenterror.hpp>
#include <orm/mysqlconnection.hpp>
#include <orm/query/joinclause.hpp>
#include <orm/utils/type.hpp>

#include "common.hpp"
#include "models/filepropertyproperty.hpp"
#include "models/setting.hpp"
#include "models/torrent.hpp"
#include "models/torrenteager.hpp"
#include "models/user.hpp"

using namespace ranges;

using json = nlohmann::json;

using Orm::AttributeItem;
using Orm::MySqlConnection;
using Orm::InvalidArgumentError;
using Orm::One;
using Orm::Tiny::ModelNotFoundError;
using Orm::Tiny::Relations::Pivot;

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
*/

const QString TestOrm::CHECK_DATABASE_EXISTS_FILE = getCheckDatabaseExistsFile();

TestOrm &TestOrm::connectToDatabase()
{
    // Create a default database connection
//    m_db = DB::create({
//        {"driver",    "QMYSQL"},
//        {"host",      qEnvironmentVariable("DB_HOST", "127.0.0.1")},
//        {"port",      qEnvironmentVariable("DB_PORT", "3306")},
//        {"database",  qEnvironmentVariable("DB_DATABASE", "")},
//        {"username",  qEnvironmentVariable("DB_USERNAME", "")},
//        {"password",  qEnvironmentVariable("DB_PASSWORD", "")},
//        {"charset",   qEnvironmentVariable("DB_CHARSET", "utf8mb4")},
//        {"collation", qEnvironmentVariable("DB_COLLATION", "utf8mb4_0900_ai_ci")},
//        {"prefix",    ""},
//        {"strict",    true},
//        {"options",   QVariantHash()},
//    });

    /* Create database connections: mysql, sqlite and mysql_alt, and make
       mysql default database connection. */
    m_db = DB::create({
        {"mysql", {
            {"driver",    "QMYSQL"},
            {"host",      qEnvironmentVariable("DB_MYSQL_HOST", "127.0.0.1")},
            {"port",      qEnvironmentVariable("DB_MYSQL_PORT", "3306")},
            {"database",  qEnvironmentVariable("DB_MYSQL_DATABASE", "")},
            {"username",  qEnvironmentVariable("DB_MYSQL_USERNAME", "")},
            {"password",  qEnvironmentVariable("DB_MYSQL_PASSWORD", "")},
            {"charset",   qEnvironmentVariable("DB_MYSQL_CHARSET", "utf8mb4")},
            {"collation", qEnvironmentVariable("DB_MYSQL_COLLATION",
                                               "utf8mb4_0900_ai_ci")},
            // CUR add timezone names to the MySQL server and test them silverqx
            {"timezone",        "SYSTEM"},
            {"prefix",          ""},
            {"strict",          true},
            {"options",         QVariantHash()},
            {"isolation_level", "REPEATABLE READ"},
        }},

        {"mysql_alt", {
            {"driver",    "QMYSQL"},
            {"host",      qEnvironmentVariable("DB_MYSQL_HOST", "127.0.0.1")},
            {"port",      qEnvironmentVariable("DB_MYSQL_PORT", "3306")},
            {"database",  qEnvironmentVariable("DB_MYSQL_DATABASE", "")},
            {"username",  qEnvironmentVariable("DB_MYSQL_USERNAME", "")},
            {"password",  qEnvironmentVariable("DB_MYSQL_PASSWORD", "")},
            {"charset",   qEnvironmentVariable("DB_MYSQL_CHARSET", "utf8mb4")},
            {"collation", qEnvironmentVariable("DB_MYSQL_COLLATION",
                                               "utf8mb4_0900_ai_ci")},
            {"timezone",        "SYSTEM"},
            {"prefix",          ""},
            {"strict",          true},
            {"isolation_level", "REPEATABLE READ"},
            {"options",         QVariantHash()},
        }},

        {"mysql_laravel8", {
            {"driver",    "QMYSQL"},
            {"host",      qEnvironmentVariable("DB_MYSQL_LARAVEL_HOST", "127.0.0.1")},
            {"port",      qEnvironmentVariable("DB_MYSQL_LARAVEL_PORT", "3306")},
            {"database",  qEnvironmentVariable("DB_MYSQL_LARAVEL_DATABASE", "")},
            {"username",  qEnvironmentVariable("DB_MYSQL_LARAVEL_USERNAME", "")},
            {"password",  qEnvironmentVariable("DB_MYSQL_LARAVEL_PASSWORD", "")},
            {"charset",   qEnvironmentVariable("DB_MYSQL_LARAVEL_CHARSET", "utf8mb4")},
            {"collation", qEnvironmentVariable("DB_MYSQL_LARAVEL_COLLATION",
                                               "utf8mb4_0900_ai_ci")},
            {"timezone",        "SYSTEM"},
            {"prefix",          ""},
            {"strict",          true},
            {"isolation_level", "REPEATABLE READ"},
            {"options",         QVariantHash()},
        }},

        {"sqlite", {
            {"driver",    "QSQLITE"},
            {"database",  qEnvironmentVariable("DB_SQLITE_DATABASE", "")},
            {"prefix",    ""},
            {"options",   QVariantHash()},
            {"foreign_key_constraints", qEnvironmentVariable("DB_SQLITE_FOREIGN_KEYS",
                                                             "true")},
            {"check_database_exists",   true},
        }},

        {"sqlite_memory", {
            {"driver",    "QSQLITE"},
            {"database",  ":memory:"},
            {"prefix",    ""},
            {"options",   QVariantHash()},
            {"foreign_key_constraints", qEnvironmentVariable("DB_SQLITE_FOREIGN_KEYS",
                                                             "true")},
        }},

        {"sqlite_check_exists_true", {
            {"driver",    "QSQLITE"},
            {"database",  CHECK_DATABASE_EXISTS_FILE},
            {"check_database_exists", true},
        }},

        {"sqlite_check_exists_false", {
            {"driver",    "QSQLITE"},
            {"database",  CHECK_DATABASE_EXISTS_FILE},
            {"check_database_exists", false},
        }},

        {"postgres", {
            {"driver",   "QPSQL"},
            {"host",     qEnvironmentVariable("DB_PGSQL_HOST",     "127.0.0.1")},
            {"port",     qEnvironmentVariable("DB_PGSQL_PORT",     "5432")},
            {"database", qEnvironmentVariable("DB_PGSQL_DATABASE", "")},
            {"schema",   qEnvironmentVariable("DB_PGSQL_SCHEMA",   "public")},
            {"username", qEnvironmentVariable("DB_PGSQL_USERNAME", "postgres")},
            {"password", qEnvironmentVariable("DB_PGSQL_PASSWORD", "")},
            {"charset",  qEnvironmentVariable("DB_PGSQL_CHARSET",  "utf8")},
            // I don't use timezone types in postgres anyway
            {"timezone", "LOCAL"},
            {"prefix",   ""},
            {"options",  QVariantHash()},
        }},

    }, "mysql");

    // Create connections eagerly, so I can enable counters
    for (const auto &connection : CONNECTIONS_TO_COUNT)
        DB::connection(connection);

    // BUG also decide how to behave when connection is not created and user enable counters silverqx
    // Enable counters on all database connections
    DB::enableElapsedCounters(CONNECTIONS_TO_COUNT);
    DB::enableStatementCounters(CONNECTIONS_TO_COUNT);

    return *this;
}

TestOrm &TestOrm::run()
{
//    ctorAggregate();
    anotherTests();
    testConnection();

    testQueryBuilderDbSpecific();

    /* Main test playground code */
    for (const auto &connection : CONNECTIONS_TO_TEST) {
        DB::setDefaultConnection(connection);

        testQueryBuilder();
        testTinyOrm();
    }
    DB::setDefaultConnection("mysql");

    /* Other tests/tries */
//    jsonConfig();
//    standardPaths();

    // Whole application Summary
    logQueryCountersBlock(QStringLiteral("Application Summary"),
                          m_appElapsed, m_appStatementsCounter,
                          m_appRecordsHaveBeenModified);

    return *this;
}

void TestOrm::anotherTests()
{
    QElapsedTimer timer;
    timer.start();

    resetAllQueryLogCounters();

    qInfo().nospace()
            << "================="
            << "\n  Another Tests  "
            << "\n=================\n";

    {
        {

            qt_noop();
        }
        {

            qt_noop();
        }

        {

            qt_noop();
        }

        {

            qt_noop();
        }

        {

            qt_noop();
        }

        {

            qt_noop();
        }

        {

            qt_noop();
        }

//        while (users.next())
//            qDebug() << "id :" << users.value("id").value<quint64>() << ";"
//                     << "name :" << users.value("name").value<QString>();
////            qDebug() << "id :" << users.value("id").value<quint64>();

//        for (auto &user : users)
//            qDebug() << "id :" << user.getAttribute("id").value<quint64>();
////            qDebug() << "id :" << user.getAttribute("id").value<quint64>() << ";"
////                     << "name :" << user.getAttribute("name").value<QString>();

        // Log summary counters if needed
//        logQueryCounters(__FUNCTION__, timer.elapsed());
//        return;

        qt_noop();
    }

    /* Ranges */
    {
        QVector<AttributeItem> attributes {
            {"one", 1},
            {"two", 2},
            {"three", 3},
            {"four", 4},
        };

//        const auto itAttribute = ranges::find_if(attributes,
//                                                 [](const auto &attribute)
//        {
//            return attribute.key == "three";
//        });

        const auto itAttribute = ranges::find(attributes, true,
                                              [](const auto &attribute)
        {
            return attribute.key == "three";
        });

        // Not found
        if (itAttribute == ranges::end(attributes))
            qt_noop();
    }

    /* __FUNCTION__ */
    {
        qInfo().nospace()
                << "\n\n=================="
                << "\n   __FUNCTION__   "
                << "\n==================\n\n";

        printf("Function name __FUNCTION__ : %s\n", __FUNCTION__);
        printf("Function name __func__ : %s\n", __func__);
#ifdef _MSC_VER
        printf("Decorated function name __FUNCDNAME__ : %s\n", __FUNCDNAME__);
#endif
        // GCC : __PRETTY_FUNCTION__ ; MSVC : __FUNCSIG__
        printf("Function signature Q_FUNC_INFO: %s\n", Q_FUNC_INFO);
        qt_noop();
    }

    /* Formatting with std::format() */
#ifdef _MSC_VER
    {
        qInfo().nospace()
            << "\n\n=================="
            << "\n    std::format   "
            << "\n==================\n\n";

        int num = 100;
        std::string s = "hello";

        printf("before: %i\n", num);
//        printf("before: %f\n", num);
        printf("before: %s\n", s.c_str());

        std::string m1 = std::format("The answer is {}.", 42);
        std::string m2 = std::format("The answer is {}.", s);

        qDebug() << QString::fromStdString(m1);
        qDebug() << QString::fromStdString(m2);

        qt_noop();
    }
#endif

    logQueryCounters(__FUNCTION__, timer.elapsed());
}

void TestOrm::testConnection()
{
    QElapsedTimer timer;
    timer.start();

    resetAllQueryLogCounters();

    qInfo().nospace()
            << "\n\n======================"
            << "\n  DatabaseConnection  "
            << "\n======================";

    /* DatabaseConnection::isMaria() - on MySQL connection */
    {
        qInfo() << "\n\nDatabaseConnection::isMaria() - on MySQL connection\n---";

        [[maybe_unused]]
        const auto isMaria =
                dynamic_cast<MySqlConnection &>(DB::connection("mysql")).isMaria();
        Q_ASSERT(!isMaria);

        qt_noop();
    }

    /* SQLite :memory: driver [sqlite_memory] */
    {
        qInfo() << "\n\nSQLite :memory: driver [sqlite_memory]\n---";

        auto &conn = DB::connection("sqlite_memory");

        conn.statement("create table tbl1 (one varchar(10), two smallint)");
        conn.insert("insert into tbl1 values(?, ?)", {"hello!", 10});
        conn.insert("insert into tbl1 values(?, ?)", {"goodbye", 20});

        auto query = conn.selectOne("select * from tbl1 where two = ?", {10});

        Q_ASSERT(query.isValid());
        Q_ASSERT(query.value("one").value<QString>() == "hello!");
        Q_ASSERT(query.value("two").value<int>() == 10);

        qt_noop();
    }

    /* SQLite database - check_database_exists */
    {
        // Remove the SQLite database file
        QFile::remove(CHECK_DATABASE_EXISTS_FILE);
        Q_ASSERT(!QFile::exists(CHECK_DATABASE_EXISTS_FILE));

        /* SQLite database [sqlite_check_exists_true] - check_database_exists - true */
        {
            qInfo() << "\n\nSQLite database [sqlite_check_exists_true] - "
                       "check_database_exists - true\n---";

            TINY_VERIFY_EXCEPTION_THROWN(
                        DB::connection("sqlite_check_exists_true")
                        .statement("create table tbl1 (one varchar(10), two smallint)"),
                        InvalidArgumentError);

            qt_noop();
        }

        /* SQLite database [sqlite_check_exists_false] - check_database_exists - false */
        {
            qInfo() << "\n\nSQLite database [sqlite_check_exists_false] - "
                       "check_database_exists - false\n---";

            DB::connection("sqlite_check_exists_false")
                    .statement("create table tbl1 (one varchar(10), two smallint)");

            Q_ASSERT(QFile::exists(CHECK_DATABASE_EXISTS_FILE));

            qt_noop();
        }

        QSqlDatabase::database("sqlite_check_exists_true").close();
        QSqlDatabase::database("sqlite_check_exists_false").close();

        // Remove the SQLite database file
        QFile::remove(CHECK_DATABASE_EXISTS_FILE);
        Q_ASSERT(!QFile::exists(CHECK_DATABASE_EXISTS_FILE));
    }

    logQueryCounters(__FUNCTION__, timer.elapsed());
}

void TestOrm::testTinyOrm()
{
    QElapsedTimer timer;
    timer.start();

    resetAllQueryLogCounters();

    qInfo().nospace()
            << "\n\n========="
            << "\n  Model  "
            << "\n=========";

//    {
//        TorrentPreviewableFile a;
//        auto files = a.query()->where("torrent_id", "=", 2).get();
//////        auto files = a.query()->where("torrent_id", "=", 261).get();
//////        auto torrent1 = files.first().getRelation<Torrent, QVector>("torrent");
//        auto torrent1 = files.first().getRelation<Torrent, One>("torrent");
////        qDebug() << torrent1->getAttribute("name");
////        auto peer1 = torrent1->getRelation<TorrentPeer, One>("torrentPeer");
//////        auto torrent2 = files.first().getRelationValue<Torrent>("torrent");
////        auto torrent3 = files.first().getRelationValue<Torrent, One>("torrent");
////        auto torrent3 = a.getRelationValue<Torrent, One>("torrent");
//////        qDebug() << torrent2;
//////        qDebug() << torrent3.has_value();
//        qt_noop();
//    }

    /* Basic get all get() */
    {
        qInfo() << "\n\nBasic get all get()\n---";
        Torrent torrent;
        auto torrents = torrent.query()->get();

        for (auto &t : torrents)
            qDebug() << "id :" << t.getAttribute("id").value<quint64>() << ";"
                     << "name :" << t.getAttribute("name").value<QString>();

        qt_noop();
    }
    {
        qInfo() << "\n\nBasic get all all()\n---";
        for (auto &t : Torrent::all())
            qDebug() << "id :" << t.getAttribute("id").value<quint64>() << ";"
                     << "name :" << t.getAttribute("name").value<QString>();

        qt_noop();
    }

    /* Model::latest() */
    {
        qInfo() << "\n\nModel::latest()\n---";
        for (auto &t : Torrent::latest()->get())
            qDebug() << "id :" << t.getAttribute("id").value<quint64>() << ";"
                     << "name :" << t.getAttribute("name").value<QString>() << ";"
                     << "created_at :"
                     << t.getAttribute(Torrent::getCreatedAtColumn())
                        .toDateTime().toString();
        qt_noop();
    }

    /* Model::oldest() */
    {
        qInfo() << "\n\nModel::oldest()\n---";
        for (auto &t : Torrent::oldest()->get())
            qDebug() << "id :" << t.getAttribute("id").value<quint64>() << ";"
                     << "name :" << t.getAttribute("name").toString() << ";"
                     << "created_at :"
                     << t.getAttribute(Torrent::getCreatedAtColumn())
                        .toDateTime().toString();
        qt_noop();
    }

    /* Model::where() */
    {
        qInfo() << "\n\nModel::where()\n---";
        for (auto &t : Torrent::where("id", ">", 3)->get())
            qDebug() << "id :" << t.getAttribute("id").value<quint64>() << ";"
                     << "name :" << t.getAttribute("name").value<QString>();

        qt_noop();
    }

    /* Model::whereEq() and operator[]() */
    {
        qInfo() << "\n\nModel::whereEq() and operator[]()\n---";
        auto t = Torrent::whereEq("id", 4)->get().first();
        qDebug() << "id :" << t["id"]->value<quint64>() << ";"
                 << "name :" << t["name"]->value<QString>();
//        qDebug() << "id :" << t.getAttribute("id")->value<quint64>() << ";"
//                 << "name :" << t.getAttribute("name")->value<QString>();

        qt_noop();
    }

    /* Model::where() vector */
    {
        qInfo() << "\n\nModel::where() vector\n---";
        for (auto &t : Torrent::where({{"id", 4}})->get())
            qDebug() << "id :" << t.getAttribute("id").value<quint64>() << ";"
                     << "name :" << t.getAttribute("name").value<QString>();

        qt_noop();
    }

//    qMetaTypeId<KeyType>();
//    QVector<quint64> vec {12, 6, 8, 2, 7, 8, 8};
//    QVector<QVariant> var(vec.begin(), vec.end());

//    std::sort(vec.begin(), vec.end());
//    std::sort(var.begin(), var.end(), [](QVariant a, QVariant b) {
//        return a.value<KeyType>() < b.value<KeyType>();
//    });
////    std::unique(var.begin(), var.end());
//    auto last = std::unique(vec.begin(), vec.end());
//    vec.erase(last, vec.end());
//    qDebug() << vec;
//    auto last1 = std::unique(var.begin(), var.end(), [](QVariant a, QVariant b) {
//        return a.value<KeyType>() == b.value<KeyType>();
//    });
//    var.erase(last1, var.end());
//    qDebug() << var;

//    using namespace ranges;
//    vec |= actions::sort | actions::unique;
//    qDebug() << vec;
//    using namespace ranges;
////    var |= actions::sort([](QVariant a, QVariant b) {
////           return a.value<KeyType>() == b.value<KeyType>();
////       })/* | actions::unique*/;
//    auto nn = std::move(var) | actions::sort(less {}, &QVariant::value<KeyType>) | actions::unique;
////    var |= actions::sort(less {}, &QVariant::value<KeyType>) | actions::unique;
//    qDebug() << nn;
//    qt_noop();

//    Torrent b;
//    std::any x = b;
//    qDebug() << x.type().name();
//    std::any a = 1;
//    qDebug() << a.type().name() << ": " << std::any_cast<int>(a) << '\n';
//    auto aa = std::any_cast<Torrent>(x);

//    std::variant<int, QString> v {10};
//    auto &x = std::get<int>(v);
//    x = 11;
//    auto y = std::get<int>(v);
//    qDebug() << x;
//    qDebug() << y;

//    QVector<QVariantMap> values;
//    auto nums = views::iota(1, 1000);
//    for (const auto &i : nums)
//        values.append({{"id", (i + 6)}, {"torrent_id", 7}, {"file_index", 0},
//                       {"filepath", QStringLiteral("test7_file%1.mkv").arg(i)},
//                       {"size", i}, {"progress", 50}});

//    m_db.query()->from("torrent_previewable_files")
//            .insert(values);
//    qt_noop();

//    Torrent b;
//    auto torrents = b.query()->where("id", "=", 2).get();
//    auto peer = torrents.first().getRelation<TorrentPeer, One>("torrentPeer");
////////    auto peer = torrents.first().getRelationValue<TorrentPeer, One>("torrentPeer");
////////    auto peer = b.getRelationValue<TorrentPeer, One>("torrentPeer");
//////    qDebug() << "peers :" << !!peer;
////    auto files = torrents.first().getRelation<TorrentPreviewableFile>("torrentFiles");
////////    auto files = b.getRelationValue<TorrentPreviewableFile>("torrentFiles");
//    qt_noop();

    /* Model::remove() */
//    {
//        TorrentPreviewableFile a;
//        auto files = a.query()->where("id", "=", 1005).get();
//        auto torrent1 = files.first().getRelation<Torrent, One>("torrent");
//        auto peer1 = torrent1->getRelation<TorrentPeer, One>("torrentPeer");
//        auto file1 = files.first();
//        auto res = file1.remove();
//        qt_noop();
//    }

    /* Model::destroy() */
    {
        qInfo() << "\n\nModel::destroy()\n---";

        Tag tag500({{"name", "tag500"}});
        tag500.save();
        Tag tag501({{"name", "tag501"}});
        tag501.save();

        auto count = Tag::destroy({tag500["id"], tag501["id"]});
        qDebug() << "Destroyed :" << count;

        qt_noop();
    }

    /* Model::save() insert */
//    {
//        TorrentPreviewableFile torrentFiles;
//        torrentFiles.setAttribute("torrent_id", 7);
//        torrentFiles.setAttribute("file_index", 0);
//        torrentFiles.setAttribute("filepath", "test7_file1003.mkv");
//        torrentFiles.setAttribute("size", 1000);
//        torrentFiles.setAttribute("progress", 50);
//        auto result = torrentFiles.save();
//        qt_noop();
//    }

    /* Model::save() insert - table without autoincrement key */
//    {
//        Setting setting;
//        setting.setAttribute("name", "first");
//        setting.setAttribute("value", "first value");
//        auto result = setting.save();
//        qt_noop();
//    }

    /* Model::save() insert - empty attributes */
//    {
//        Setting setting;
//        auto result = setting.save();
//        qDebug() << "save :" << result;
//        qt_noop();
//    }

    /* Model::save() update - success */
//    {
//        auto torrentFile = TorrentPreviewableFile().query()->where("id", "=", 1012).first();

//        torrentFile->setAttribute("file_index", 10);
//        torrentFile->setAttribute("filepath", "test1_file10.mkv");
//        torrentFile->setAttribute("progress", 251);

//        auto result = torrentFile->save();
//        qDebug() << "save :" << result;
//        qt_noop();
//    }

    /* Model::save() update - null value */
//    {
//        auto torrentPeer = TorrentPeer().query()->where("id", "=", 4).first();

//        torrentPeer->setAttribute("seeds", {});

//        auto result = torrentPeer->save();
//        qDebug() << "save :" << result;
//        qt_noop();
//    }

    /* Model::save() update - failed */
//    {
//        auto torrentFile = TorrentPreviewableFile().query()->where("id", "=", 8).first();

//        torrentFile->setAttribute("file_indexx", 11);

//        auto result = torrentFile->save();
//        qDebug() << "save :" << result;
//        qt_noop();
//    }

    /* Model::find(id) */
    {
        qInfo() << "\n\nModel::find(id)\n---";
        const auto id = 3;
        auto torrentFile = TorrentPreviewableFile::find(id);

        qDebug() << "torrentFile id :" << id;
        qDebug() << torrentFile->getAttribute("filepath").value<QString>();

        qt_noop();
    }

    /* Model::findOrNew() - found */
    {
        qInfo() << "\n\nModel::findOrNew() - found\n---";

        auto torrent = Torrent::findOrNew(3, {"id", "name"});

        Q_ASSERT(torrent.exists);
        Q_ASSERT(torrent["name"] == QVariant("test3"));

        qt_noop();
    }

    /* Model::findOrNew() - not found */
    {
        qInfo() << "\n\nModel::findOrNew() - not found\n---";

        auto torrent = Torrent::findOrNew(999999, {"id", "name"});

        Q_ASSERT(!torrent.exists);
        Q_ASSERT(torrent["id"] == QVariant());
        Q_ASSERT(torrent["name"] == QVariant());
        Q_ASSERT(torrent.getAttributes().isEmpty());

        qt_noop();
    }

    /* Model::findOrFail() - found */
    {
        qInfo() << "\n\nModel::findOrFail() - found\n---";

        auto torrent = Torrent::findOrFail(3, {"id", "name"});

        Q_ASSERT(torrent.exists);
        Q_ASSERT(torrent["id"] == QVariant(3));
        Q_ASSERT(torrent["name"] == QVariant("test3"));

        qt_noop();
    }

    /* Model::findOrFail() - not found, fail */
    {
        qInfo() << "\n\nModel::findOrFail() - not found, fail\n---";

        TINY_VERIFY_EXCEPTION_THROWN(Torrent::findOrFail(999999), ModelNotFoundError);

        qt_noop();
    }

    /* Model::whereEq()/firstWhere()/firstWhereEq() */
    {
        qInfo() << "\n\nModel::whereEq()/firstWhere()/firstWhereEq()\n---";

        auto torrentFile3 = TorrentPreviewableFile::whereEq("id", 3)->first();
        Q_ASSERT(torrentFile3->exists);
        Q_ASSERT((*torrentFile3)["id"] == QVariant(3));
        Q_ASSERT((*torrentFile3)["filepath"] == QVariant("test2_file2.mkv"));

        auto torrentFile4 = TorrentPreviewableFile::firstWhere("id", "=", 4);
        Q_ASSERT(torrentFile4->exists);
        Q_ASSERT((*torrentFile4)["id"] == QVariant(4));
        Q_ASSERT((*torrentFile4)["filepath"] == QVariant("test3_file1.mkv"));

        auto torrentFile5 = TorrentPreviewableFile::firstWhereEq("id", 5);
        Q_ASSERT(torrentFile5->exists);
        Q_ASSERT((*torrentFile5)["id"] == QVariant(5));
        Q_ASSERT((*torrentFile5)["filepath"] == QVariant("test4_file1.mkv"));

        qt_noop();
    }

    /* Model::where() - vector */
    {
        qInfo() << "\n\nModel::where() - vector\n---";

        auto torrentFile3 = TorrentPreviewableFile::where(
                                {{"id", 3},
                                 {"filepath", "test2_file2.mkv"}})
                            ->first();

        Q_ASSERT(torrentFile3->exists);
        Q_ASSERT((*torrentFile3)["id"] == QVariant(3));
        Q_ASSERT((*torrentFile3)["filepath"] == QVariant("test2_file2.mkv"));

        qt_noop();
    }

    /* Model::firstOrNew() - found */
    {
        qInfo() << "\n\nModel::firstOrNew() - found\n---";
        auto torrent = Torrent::firstOrNew(
                           {{"id", 3}},

                           {{"name", "test3"},
                            {"size", 13},
                            {"progress", 300}
                           });

        Q_ASSERT(torrent.exists);
        Q_ASSERT(torrent["name"] == QVariant("test3"));

        qt_noop();
    }

    /* Model::firstOrNew() - not found */
    {
        qInfo() << "\n\nModel::firstOrNew() - not found\n---";
        auto torrent = Torrent::firstOrNew(
                           {{"id", 10}},

                           {{"name", "test10"},
                            {"size", 20},
                            {"progress", 800}
                           });

        Q_ASSERT(!torrent.exists);
        Q_ASSERT(!torrent.getAttributesHash().contains("id"));
        Q_ASSERT(torrent["name"] == QVariant("test10"));
        Q_ASSERT(torrent["size"] == QVariant(20));
        Q_ASSERT(torrent["progress"] == QVariant(800));

        qt_noop();
    }

    /* Model::firstOrCreate() - found */
    {
        qInfo() << "\n\nModel::firstOrCreate() - found\n---";
        auto torrent = Torrent::firstOrCreate(
                           {{"name", "test3"}},

                           {{"size", 13},
                            {"progress", 300},
                            {"hash", "3579e3af2768cdf52ec84c1f320333f68401dc6e"},
                           });

        Q_ASSERT(torrent.exists);
        Q_ASSERT(torrent["name"] == QVariant("test3"));

        qt_noop();
    }

    /* Model::firstOrCreate() - not found */
    {
        qInfo() << "\n\nModel::firstOrCreate() - not found\n---";
        auto torrent = Torrent::firstOrCreate(
                           {{"name", "test50"}},

                           {{"size", 50},
                            {"progress", 550},
                            {"hash", "5979e3af2768cdf52ec84c1f320333f68401dc6e"},
                           });

        Q_ASSERT(torrent.exists);
        Q_ASSERT(torrent["id"] != QVariant());
        Q_ASSERT(torrent["name"] == QVariant("test50"));
        Q_ASSERT(torrent["size"] == QVariant(50));
        Q_ASSERT(torrent["progress"] == QVariant(550));
        Q_ASSERT(torrent["hash"] == QVariant("5979e3af2768cdf52ec84c1f320333f68401dc6e"));

        torrent.remove();

        qt_noop();
    }

    /* TinyBuilder::firstOrFail() - found */
    {
        qInfo() << "\n\nTinyBuilder::firstOrFail() - found\n---";

        auto torrent = Torrent::whereEq("id", 3)->firstOrFail({"id", "name"});

        Q_ASSERT(torrent.exists);
        Q_ASSERT(torrent["id"] == QVariant(3));
        Q_ASSERT(torrent["name"] == QVariant("test3"));
        Q_ASSERT(torrent["size"] == QVariant());
        Q_ASSERT(torrent.getAttributes().size() == 2);

        qt_noop();
    }

    /* TinyBuilder::firstOrFail() - not found, fail */
    {
        qInfo() << "\n\nTinyBuilder::firstOrFail() - not found, fail\n---";

        TINY_VERIFY_EXCEPTION_THROWN(
                    Torrent::whereEq("id", 999999)->firstOrFail(), ModelNotFoundError);

        qt_noop();
    }

    /* hash which manages insertion order */
//    {
//        QHash<QString *, std::reference_wrapper<QVariant>> hash;
//        QVector<AttributeItem> vec;

//        using namespace Orm;
//        AttributeItem a {"a", "one"};
//        AttributeItem b {"b", "two"};
//        AttributeItem c {"c", "three"};
//        AttributeItem d {"d", "four"};

//        vec << a << b << c << d;
//        hash.insert(&a.key, a.value);
//        hash.insert(&b.key, b.value);
//        hash.insert(&c.key, c.value);
//        hash.insert(&d.key, d.value);
////        hash.insert(&a.key, &a.value);
////        hash.insert(&b.key, &b.value);
////        hash.insert(&c.key, &c.value);
////        hash.insert(&d.key, &d.value);

//        qDebug() << "hash:";
//        for (QVariant &val : hash) {
//            qDebug() << val.value<QString>();
//        }
//        qDebug() << "\nvector:";
//        for (auto &val : vec) {
//            qDebug() << val.value.value<QString>();
//        }
//        qDebug() << "\nothers:";
//        qDebug() << hash.find(&b.key).value().get().value<QString>();
////        qDebug() << hash[&b.key].get().value<QString>();
////        qDebug() << hash[&d.key]->value<QString>();
////        qDebug() << hash[&b.key]->value<QString>();
//        qt_noop();
//    }

    /* std::optional() with std::reference_wrapper() */
//    {
//        struct ToModify
//        {
//            QString name;
//        };

//        ToModify a {"first"};
//        ToModify b {"second"};
//        ToModify c {"third"};

//        QHash<QString, std::optional<std::reference_wrapper<ToModify>>> hash
//        {
//            {"one", a},
//            {"two", b},
//            {"three", c},
//        };

//        ToModify &value = *hash.find("two").value();
//        value.name = "xx";
//        qDebug() << b.name;
//        qDebug() << hash.find("two").value()->get().name;
//        qt_noop();
//    }

    /* Model::push() - lazy load */
    {
        qInfo() << "\n\nModel::push() - lazy load\n---";

        auto torrent = Torrent::find(2);

        // eager load
//        auto files = torrent->getRelation<TorrentPreviewableFile>("torrentFiles");
//        auto *file = files.first();
//        auto *fileProperty = file->getRelation<TorrentPreviewableFileProperty, One>("fileProperty");

        // lazy load
        auto files = torrent->getRelationValue<TorrentPreviewableFile>("torrentFiles");
        auto *file = files.first();
        auto *fileProperty =
                file->getRelationValue<TorrentPreviewableFileProperty, One>(
                    "fileProperty");

        auto torrentName      = torrent->getAttribute("name").value<QString>();
        auto filepath         = file->getAttribute("filepath").value<QString>();
        auto filePropertyName = fileProperty->getAttribute("name").value<QString>();
        qDebug() << torrentName;
        qDebug() << filepath;
        qDebug() << filePropertyName;

        torrent->setAttribute("name", QVariant(torrentName + " x"));
        file->setAttribute("filepath", QVariant(filepath + ".xy"));
        fileProperty->setAttribute("name", QVariant(filePropertyName + " x"));

        const auto debugAttributes = [&torrent]
        {
            auto files = torrent->getRelation<TorrentPreviewableFile>("torrentFiles");
            auto file = files.first();
            auto *fileProperty =
                    file->getRelation<TorrentPreviewableFileProperty, One>(
                        "fileProperty");
            qDebug() << torrent->getAttribute("name").value<QString>();
            qDebug() << file->getAttribute("filepath").value<QString>();
            qDebug() << fileProperty->getAttribute("name").value<QString>();
        };

        debugAttributes();
        torrent->push();

        // Set it back, so I don't have to manually
        torrent->setAttribute("name", torrentName);
        file->setAttribute("filepath", filepath);
        fileProperty->setAttribute("name", filePropertyName);

        debugAttributes();
        torrent->push();

        qt_noop();
    }

    /* eager/lazy load - getRelation() and getRelationValue() */
    {
        qInfo() << "\n\neager/lazy load - getRelation() and getRelationValue()\n---";
        auto torrent = Torrent::find(2);

        // eager load
//        {
//            qDebug() << "\neager load:";
//            auto files = torrent->getRelation<TorrentPreviewableFile>("torrentFiles");

//            for (auto *file : files) {
//                qDebug().nospace().noquote()
//                        << "\nFile : "
//                        << file->getAttribute("filepath").value<QString>()
//                        << "(" << file->getAttribute("id").value<quint64>() << ")";

//                auto fileProperty = file->getRelation<TorrentPreviewableFileProperty, One>("fileProperty");

//                qDebug().nospace().noquote()
//                        << fileProperty->getAttribute("name").value<QString>()
//                        << "(" << fileProperty->getAttribute("id").value<quint64>() << ")";
//                qt_noop();
//            }
//        }

        // lazy load
        {
            qDebug() << "\nlazy load, !!comment out 'with' relation to prevent eager load";
            qDebug() << "\n↓↓↓ lazy load works, if an Executed query is logged under this comment:";
            auto files = torrent->getRelationValue<TorrentPreviewableFile>("torrentFiles");

            for (auto *file : files) {
                qDebug().nospace().noquote()
                        << "\nFile : "
                        << file->getAttribute("filepath").value<QString>()
                        << "(" << file->getAttribute("id").value<quint64>() << ")";

                auto fileProperty = file->getRelationValue<TorrentPreviewableFileProperty, One>("fileProperty");

                if (fileProperty)
                    qDebug().nospace().noquote()
                            << fileProperty->getAttribute("name").value<QString>()
                            << "(" << fileProperty->getAttribute("id").value<quint64>()
                            << ")";
                qt_noop();
            }
        }

        qt_noop();
    }

    /* Model::with() - One - hasOne() */
    {
        qInfo() << "\n\nModel::with() - One - hasOne()\n---";
        auto torrents = Torrent::with("torrentPeer")->orderBy("id").get();
        auto *peer = torrents[1].getRelation<TorrentPeer, One>("torrentPeer");
        qDebug() << peer->getAttribute("id");
        qt_noop();
    }

    /* Model::with() - One - belongsTo() */
    {
        qInfo() << "\n\nModel::with() - One - belongsTo()\n---";
        auto files = TorrentPreviewableFile::with("torrent")->orderBy("id").get();
        auto *torrent = files[1].getRelation<Torrent, One>("torrent");
        qDebug() << torrent->getAttribute("id");
        qt_noop();
    }

    /* Model::with() - Many - hasMany() */
    {
        qInfo() << "\n\nModel::with() - Many - hasMany()\n---";
        auto torrents = Torrent::with("torrentFiles")->orderBy("id").get();
        auto files = torrents[1].getRelation<TorrentPreviewableFile>("torrentFiles");
        for (const auto &file : files)
            qDebug() << file->getAttribute("filepath");
        qt_noop();
    }

    /* Model::withOnly() */
    {
        qInfo() << "\n\nModel::withOnly()\n---";
        auto t = TorrentEager::withOnly("torrentPeer")->find(1);
        auto *p = t->getRelation<TorrentPeerEager_NoRelations, One>("torrentPeer");
        qDebug() << p->getAttribute("id");
        qt_noop();
    }

    /* Timestamps - update */
    {
        qInfo() << "\n\nTimestamps - update\n---";
        auto torrent = Torrent::find(2);

        qDebug() << "progress before :" << torrent->getAttribute("progress");
        torrent->setAttribute("progress",
                              torrent->getAttribute("progress").value<uint>() + 1);

        const auto &updatedAt = torrent->getUpdatedAtColumn();
        qDebug() << updatedAt << "before :" << torrent->getAttribute(updatedAt).toDateTime();

        torrent->save();
        qDebug() << "progress after :" << torrent->getAttribute("progress");
        qDebug() << updatedAt << "after :" << torrent->getAttribute(updatedAt).toDateTime();
        qt_noop();
    }

    /* Timestamps - update, select w/o updated_at column */
    {
        qInfo() << "\n\nTimestamps - update, select w/o updated_at column\n---";
        auto torrent = Torrent::whereEq("id", 2)->first({"id", "name", "progress"});

        qDebug() << "progress before :" << torrent->getAttribute("progress");
        torrent->setAttribute("progress",
                              torrent->getAttribute("progress").value<uint>() + 1);

        const auto &updatedAt = torrent->getUpdatedAtColumn();
        qDebug() << updatedAt << "before :" << torrent->getAttribute(updatedAt).toDateTime();

        torrent->save();
        qDebug() << "progress after :" << torrent->getAttribute("progress");
        qDebug() << updatedAt << "after :" << torrent->getAttribute(updatedAt).toDateTime();
        qt_noop();
    }

    /* Timestamps - update, u_timestamps = false */
    {
        qInfo() << "\n\nTimestamps - update, u_timestamps = false\n---";
        auto fileProperty = TorrentPreviewableFileProperty::find(2);

        qDebug() << "size before :" << fileProperty->getAttribute("size");
        fileProperty->setAttribute("size",
                                   fileProperty->getAttribute("size").value<uint>() + 1);

        fileProperty->save();
        qDebug() << "size after :" << fileProperty->getAttribute("size");
        qt_noop();
    }

    /* Touching Parent Timestamps */
    {
        qInfo() << "\n\nTouching Parent Timestamps\n---";
        auto filePropertyProperty = FilePropertyProperty::find(4);

        qDebug() << "value before :" << filePropertyProperty->getAttribute("value");
        filePropertyProperty->setAttribute("value",
                                           filePropertyProperty->getAttribute("value")
                                           .value<uint>() + 1);

        const auto &updatedAtFilePropertyProperty = filePropertyProperty->getUpdatedAtColumn();
        qDebug() << "FilePropertyProperty" << updatedAtFilePropertyProperty << "before :"
                 << filePropertyProperty->getAttribute(updatedAtFilePropertyProperty).toDateTime();

        auto fileProperty = TorrentPreviewableFileProperty::find(3);
        const auto &updatedAtFileProperty = fileProperty->getUpdatedAtColumn();
        qDebug() << "TorrentPreviewableFileProperty" << updatedAtFileProperty << "before :"
                 << fileProperty->getAttribute(updatedAtFileProperty).toDateTime();

        auto torrentFile = TorrentPreviewableFile::find(4);
        const auto &updatedAtTorrentFile = torrentFile->getUpdatedAtColumn();
        qDebug() << "TorrentPreviewableFile" << updatedAtTorrentFile << "before :"
                 << torrentFile->getAttribute(updatedAtTorrentFile).toDateTime();

        auto torrent = Torrent::find(3);
        const auto &updatedAtTorrent = torrent->getUpdatedAtColumn();
        qDebug() << "Torrent" << updatedAtTorrent << "before :"
                 << torrent->getAttribute(updatedAtTorrent).toDateTime();

        filePropertyProperty->save();

        qDebug() << "value after :" << filePropertyProperty->getAttribute("value");

        // Fetch fresh model
        auto filePropertyProperty1 = FilePropertyProperty::find(4);
        qDebug() << "FilePropertyProperty" << updatedAtFilePropertyProperty << "after :"
                 << filePropertyProperty1->getAttribute(updatedAtFilePropertyProperty).toDateTime();

        fileProperty = TorrentPreviewableFileProperty::find(3);
        qDebug() << "TorrentPreviewableFileProperty" << updatedAtFileProperty << "after :"
                 << fileProperty->getAttribute(updatedAtFileProperty).toDateTime();

        torrentFile = TorrentPreviewableFile::find(4);
        qDebug() << "TorrentPreviewableFile" << updatedAtTorrentFile << "after :"
                 << torrentFile->getAttribute(updatedAtTorrentFile).toDateTime();

        torrent = Torrent::find(3);
        qDebug() << "Torrent" << updatedAtTorrent << "after :"
                 << torrent->getAttribute(updatedAtTorrent).toDateTime();

        qt_noop();
    }

    /* Timestamps - insert */
    {
        qInfo() << "\n\nTimestamps - insert\n---";
        const auto torrentId = 1;
        TorrentPreviewableFile torrentFile;
        torrentFile.setAttribute("torrent_id", torrentId);
        torrentFile.setAttribute("file_index", 1);
        torrentFile.setAttribute("filepath", "test1_file2.mkv");
        torrentFile.setAttribute("size", 1000);
        torrentFile.setAttribute("progress", 50);

        [[maybe_unused]]
        auto result = torrentFile.save();
        const auto &createdAt = torrentFile.getCreatedAtColumn();
        qDebug() << createdAt << "after :" << torrentFile.getAttribute(createdAt).toDateTime();
        const auto &updatedAt = torrentFile.getUpdatedAtColumn();
        qDebug() << updatedAt << "after :" << torrentFile.getAttribute(updatedAt).toDateTime();
        qt_noop();

        const auto removeResult = torrentFile.remove();
        qDebug() << "called remove():" << removeResult;

        qDebug() << "TorrentPreviewableFile" << updatedAt << "after remove:"
                 << torrentFile.getAttribute(updatedAt).toDateTime();

        auto torrent = Torrent::find(torrentId);
        const auto &torrentUpdatedAt = torrent->getUpdatedAtColumn();
        qDebug() << "Torrent" << torrentUpdatedAt << "after remove:"
                 << torrent->getAttribute(torrentUpdatedAt).toDateTime();

        qt_noop();
    }

    /* TinyBuilder::increment()/decrement() */
    {
        qInfo() << "\n\nTinyBuilder::increment()/decrement()\n---";

        auto torrent = Torrent::find(1);

        const auto &updatedAt = torrent->getUpdatedAtColumn();
        qDebug() << "size before:" << torrent->getAttribute("size").value<uint>();
        qDebug() << updatedAt << "before:"
                 << torrent->getAttribute(updatedAt).toDateTime();
        qDebug() << "progress before:"
                 << (*torrent)["progress"]->value<uint>();

        Torrent::whereEq("id", 1)->increment("size", 2, {{"progress", 111}});

        torrent = Torrent::find(1);
        qDebug() << "size after increment:" << torrent->getAttribute("size").value<uint>();
        qDebug() << updatedAt << "after increment:"
                 << torrent->getAttribute(updatedAt).toDateTime();
        qDebug() << "progress after increment:"
                 << (*torrent)["progress"]->value<uint>();

        Torrent::whereEq("id", 1)->decrement("size", 2, {{"progress", 100}});

        torrent = Torrent::find(1);
        qDebug() << "size after decrement:" << torrent->getAttribute("size").value<uint>();
        qDebug() << updatedAt << "after decrement:"
                 << torrent->getAttribute(updatedAt).toDateTime();
        qDebug() << "progress after decrement:"
                 << (*torrent)["progress"]->value<uint>();
        qt_noop();
    }

    /* Model::create() */
    {
        qInfo() << "\n\nModel::create()\n---";

        auto torrent = Torrent::create({
            {"name",     "test100"},
            {"size",     100},
            {"progress", 333},
            {"added_on", QDateTime::currentDateTime()},
            {"hash",     "1009e3af2768cdf52ec84c1f320333f68401dc6e"},
        });

        torrent.setAttribute("name", "test100 create");
        torrent.save();

        torrent.remove();

        qt_noop();
    }

    /* TinyBuilder::update() */
    {
        qInfo() << "\n\nTinyBuilder::update()\n---";

        Torrent::whereEq("id", 3)
                ->update({{"progress", 333}});

        qDebug() << "progress after:"
                 << (*Torrent::find(3))["progress"].value<uint>();
        qt_noop();
    }

    /* Model::update() */
    {
        qInfo() << "\n\nModel::update()\n---";

        Torrent::find(3)
                ->update({{"progress", 300}});

        qDebug() << "progress after:"
                 << Torrent::find(3)->getAttribute("progress").value<uint>();
        qt_noop();
    }

    /* Model::isClean()/isDirty() */
    {
        qInfo() << "\n\nModel::isClean()/isDirty()\n---";

        auto torrent = Torrent::find(3);

        Q_ASSERT(torrent->isClean());
        Q_ASSERT(!torrent->isDirty());
        Q_ASSERT(torrent->isClean("name"));
        Q_ASSERT(!torrent->isDirty("name"));

        torrent->setAttribute("name", "test3 dirty");

        Q_ASSERT(!torrent->isClean());
        Q_ASSERT(torrent->isDirty());
        Q_ASSERT(!torrent->isClean("name"));
        Q_ASSERT(torrent->isDirty("name"));
        Q_ASSERT(torrent->isClean("size"));
        Q_ASSERT(!torrent->isDirty("size"));

        torrent->save();

        Q_ASSERT(torrent->isClean());
        Q_ASSERT(!torrent->isDirty());
        Q_ASSERT(torrent->isClean("name"));
        Q_ASSERT(!torrent->isDirty("name"));
        Q_ASSERT(torrent->isClean("size"));
        Q_ASSERT(!torrent->isDirty("size"));

        torrent->setAttribute("name", "test3");
        torrent->save();

        qt_noop();
    }

    /* Model::wasChanged() */
    {
        qInfo() << "\n\nModel::wasChanged()\n---";

        auto torrent = Torrent::find(3);

        Q_ASSERT(!torrent->wasChanged());
        Q_ASSERT(!torrent->wasChanged("name"));

        torrent->setAttribute("name", "test3 changed");

        Q_ASSERT(!torrent->wasChanged());
        Q_ASSERT(!torrent->wasChanged("name"));

        torrent->save();

        Q_ASSERT(torrent->wasChanged());
        Q_ASSERT(torrent->wasChanged("name"));
        Q_ASSERT(!torrent->wasChanged("size"));

        torrent->setAttribute("name", "test3");
        torrent->save();

        qt_noop();
    }

    /* Model::fresh() */
    {
        qInfo() << "\n\nModel::fresh()\n---";

        Torrent t;
        Q_ASSERT(t.fresh() == std::nullopt);

        auto torrent = Torrent::find(3);
        auto fresh = torrent->fresh();
        Q_ASSERT(&*torrent != &*fresh);

        qt_noop();
    }

    /* Model::refresh() - only attributes check */
    {
        qInfo() << "\n\nModel::refresh() - only attributes check\n---";

        Torrent t;
        Q_ASSERT(&t.refresh() == &t);

        auto torrent = Torrent::find(3);
        auto original = torrent->getAttribute("name").value<QString>();

        torrent->setAttribute("name", "test3 refresh");
        auto &refreshed = torrent->refresh();
        Q_ASSERT(&*torrent == &refreshed);
        Q_ASSERT(torrent->getAttribute("name") == QVariant(original));

        qt_noop();
    }

    /* Model::load() */
    {
        qInfo() << "\n\nModel::load()\n---";

        auto torrent = Torrent::find(2);

        Q_ASSERT(torrent->getRelations().empty());

        torrent->load({{"torrentFiles"}, {"torrentPeer"}});

        const auto &relations = torrent->getRelations();
        Q_ASSERT(relations.size() == 2);
        Q_ASSERT(relations.contains("torrentFiles"));
        Q_ASSERT(relations.contains("torrentPeer"));

        qt_noop();
    }

    /* Model::refresh() - only relations check */
    {
        qInfo() << "\n\nModel::refresh() - only relations check\n---";

        Torrent t;
        Q_ASSERT(&t.refresh() == &t);

        auto torrent = Torrent::find(3);

        auto &relations = torrent->getRelations();
        Q_ASSERT(relations.empty());

        // Validate original attribute values in relations
        auto filesOriginal =
                torrent->getRelationValue<TorrentPreviewableFile>("torrentFiles");
        auto filepathOriginal =
                filesOriginal.first()->getAttribute("filepath");
        auto peerOriginal =
                torrent->getRelationValue<TorrentPeer, One>("torrentPeer");
        auto seedsOriginal =
                peerOriginal->getAttribute("seeds");
        Q_ASSERT(relations.size() == 2);
        Q_ASSERT(filepathOriginal == QVariant("test3_file1.mkv"));
        Q_ASSERT(seedsOriginal == QVariant(3));

        // Change attributes in relations
        filesOriginal.first()->setAttribute("filepath", "test3_file1-refresh.mkv");
        peerOriginal->setAttribute("seeds", 33);

        // Validate changed attributes in relations
        auto filepathOriginalChanged =
                torrent->getRelationValue<TorrentPreviewableFile>("torrentFiles")
                .first()->getAttribute("filepath");
        auto seedsOriginalChanged =
                torrent->getRelationValue<TorrentPeer, One>("torrentPeer")
                ->getAttribute("seeds");
        Q_ASSERT(filepathOriginalChanged == QVariant("test3_file1-refresh.mkv"));
        Q_ASSERT(seedsOriginalChanged == QVariant(33));

        uintptr_t relationFilesKeyOriginal =
                reinterpret_cast<uintptr_t>(&relations.find("torrentFiles")->first);
        uintptr_t relationFilesValueOriginal =
                reinterpret_cast<uintptr_t>(&relations.find("torrentFiles")->second);

        torrent->refresh();

        Q_ASSERT(relations.size() == 2);
        /* Values in the QHash container can't be the same, because they were
           moved from the Model copy in the Model::load() method. */
        Q_ASSERT(relationFilesKeyOriginal
                 == reinterpret_cast<uintptr_t>(&relations.find("torrentFiles")->first));
        Q_ASSERT(relationFilesValueOriginal
                 == reinterpret_cast<uintptr_t>(&relations.find("torrentFiles")->second));

        // Validate refreshed attributes in relations
        auto filesRefreshed =
                torrent->getRelationValue<TorrentPreviewableFile>("torrentFiles");
        auto filepathRefreshed = filesRefreshed.first()->getAttribute("filepath");
        auto peerRefreshed =
                torrent->getRelationValue<TorrentPeer, One>("torrentPeer");
        auto seedsRefreshed = peerRefreshed->getAttribute("seeds");
        Q_ASSERT(filepathOriginal == filepathRefreshed);
        Q_ASSERT(seedsOriginal == seedsRefreshed);

        qt_noop();
    }

    /* Model::is()/isNot() */
    {
        qInfo() << "\n\nModel::is()/isNot()\n---";

        auto torrent31 = Torrent::find(3);
        auto torrent32 = Torrent::find(3);
        auto torrent4 = Torrent::find(4);

        Q_ASSERT(torrent31->is(torrent32));
        Q_ASSERT(torrent31->isNot(torrent4));

        qt_noop();
    }

    /* Many-to-Many Relationship */
    {
        qInfo() << "\n\nMany-to-Many Relationship\n---";

        auto x = Torrent::find(2);
        auto tags = x->getRelationValue<Tag>("tags");
        [[maybe_unused]]
        auto *pivot1 = tags.first()->getRelation<Tagged, One>("tagged");
        [[maybe_unused]]
        auto *pivot2 = tags.at(1)->getRelation<Tagged, One>("tagged");
        [[maybe_unused]]
        auto *pivot3 = tags.at(2)->getRelation<Tagged, One>("tagged");
//        [[maybe_unused]]
//        auto *pivot1 = tags.first()->getRelation<Pivot, One>("tagged");
//        [[maybe_unused]]
//        auto *pivot2 = tags.at(1)->getRelation<Pivot, One>("tagged");
//        [[maybe_unused]]
//        auto *pivot3 = tags.at(2)->getRelation<Pivot, One>("tagged");
        qDebug() << "Tagged Active :"
                 << pivot3->getAttribute("active").value<bool>();
        qDebug() << "Tagged created_at :"
                 << pivot3->getAttribute("created_at").toDateTime();

        auto *tagProperty = tags.first()->getRelation<TagProperty, One>("tagProperty");
        qDebug() << "TagProperty:";
        qDebug() << "id :" << tagProperty->getAttribute("id").value<quint64>() << ";"
                 << "color :" << tagProperty->getAttribute("color").value<QString>() << ";"
                 << "position :" << tagProperty->getAttribute("position").value<uint>();

        qt_noop();
    }

    /* Many-to-Many Relationship - Inverse side and refresh test */
    {
        qInfo() << "\n\nMany-to-Many Relationship - Inverse side and refresh test\n---";

        qDebug() << "\nRefresh has to exclude pivot relations, to avoid exception.\n";

        auto tag = Tag::find(2);
        auto torrents = tag->getRelationValue<Torrent>("torrents");
//        [[maybe_unused]]
//        auto *pivot21 = torrents.first()->getRelation<Tagged, One>("pivot");
//        [[maybe_unused]]
//        auto *pivot22 = torrents.at(1)->getRelation<Tagged, One>("pivot");
        [[maybe_unused]]
        auto *pivot21 = torrents.first()->getRelation<Pivot, One>("pivot");
        [[maybe_unused]]
        auto *pivot22 = torrents.at(1)->getRelation<Pivot, One>("pivot");
        qDebug() << "Pivot Active :"
                 << pivot21->getAttribute("active").value<bool>();
        qDebug() << "Pivot created_at :"
                 << pivot21->getAttribute("created_at").toDateTime();

        auto *torrent1 = torrents.first();
        torrent1->setAttribute("name", "xyz");

        torrent1->refresh();
    }

    /* BelongsTo::associate */
    {
        qInfo() << "\n\nBelongsTo::associate\n---";

        TorrentPreviewableFile file {
            {"file_index", 3},
            {"filepath", "test5_file4.mkv"},
            {"size", 3255},
            {"progress", 115},
            {"note", "associate"},
        };

        auto torrent = Torrent::find(5);

        auto &fileRef = file.torrent()->associate(*torrent);
        fileRef = file.torrent()->disassociate();
        fileRef = file.torrent()->associate(*torrent);

//        fileRef.save();
        file.save();

        [[maybe_unused]]
        auto verifyTorrent5 = file.getRelation<Torrent, One>("torrent");

        /* Have to unset current relationship, this is clearly visible in the Eqloquent's
           associate implementation. */
        fileRef = file.torrent()->associate(2);

//        fileRef.save();
        file.save();

        [[maybe_unused]]
        auto verifyTorrent4 = file.getRelation<Torrent, One>("torrent");

        // Restore db
        fileRef.remove();

        qt_noop();
    }

    /* BelongsTo::associate with an ID */
    {
        qInfo() << "\n\nBelongsTo::associate with an ID\n---";

        TorrentPreviewableFile file {
            {"file_index", 3},
            {"filepath", "test5_file4.mkv"},
            {"size", 3255},
            {"progress", 115},
            {"note", "associate"},
        };

        auto &fileRef = file.torrent()->associate(5);

//        fileRef.save();
        file.save();

        [[maybe_unused]]
        auto verifyTorrent = file.getRelation<Torrent, One>("torrent");

        // Restore db
        fileRef.remove();

        qt_noop();
    }

    /* BelongsToMany::attach with Custom pivot */
    {
        qInfo() << "\n\nBelongsToMany::attach with Custom pivot\n---";

        Tag tag100({{"name", "tag100"}});
        tag100.save();
        Tag tag101({{"name", "tag101"}});
        tag101.save();

        auto torrent5 = Torrent::find(5);

        torrent5->tags()->attach({tag100["id"], tag101["id"]},
                                 {{"active", false}});

        tag100.remove();
        tag101.remove();

        qt_noop();
    }

    /* BelongsToMany::attach with Pivot */
    {
        qInfo() << "\n\nBelongsToMany::attach with Pivot\n---";

        Torrent torrent100 {
            {"name", "test100"}, {"size", 100}, {"progress", 555},
            {"hash", "xyzhash100"}, {"note", "attach with pivot"},
        };
        torrent100.save();
        Torrent torrent101 {
            {"name", "test101"}, {"size", 101}, {"progress", 556},
            {"hash", "xyzhash101"}, {"note", "attach with pivot"},
        };
        torrent101.save();

        auto tag4 = Tag::find(4);

        tag4->torrents()->attach({torrent100["id"], torrent101["id"]},
                                 {{"active", false}});

        torrent100.remove();
        torrent101.remove();

        qt_noop();
    }

    /* BelongsToMany::attach with Custom pivot and IDs with Attributes overload */
    {
        qInfo() << "\n\nBelongsToMany::attach with Custom pivot and "
                    "IDs with Attributes overload\n---";

        Tag tag100({{"name", "tag100"}});
        tag100.save();
        Tag tag101({{"name", "tag101"}});
        tag101.save();

        auto torrent5 = Torrent::find(5);

        torrent5->tags()->attach({
            {tag100["id"]->value<quint64>(), {{"active", false}}},
            {tag101["id"]->value<quint64>(), {{"active", true}}}
        });

        tag100.remove();
        tag101.remove();

        qt_noop();
    }

    /* BelongsToMany::attach with Pivot and IDs with Attributes overload */
    {
        qInfo() << "\n\nBelongsToMany::attach with Pivot and "
                    "IDs with Attributes overload\n---";

        Torrent torrent100 {
            {"name", "test100"}, {"size", 100}, {"progress", 555},
            {"hash", "xyzhash100"}, {"note", "attach with pivot"},
        };
        torrent100.save();
        Torrent torrent101 {
            {"name", "test101"}, {"size", 101}, {"progress", 556},
            {"hash", "xyzhash101"}, {"note", "attach with pivot"},
        };
        torrent101.save();

        auto tag4 = Tag::find(4);

        tag4->torrents()->attach({
            {torrent100["id"]->value<quint64>(), {{"active", false}}},
            {torrent101["id"]->value<quint64>(), {{"active", true}}},
        });

        torrent100.remove();
        torrent101.remove();

        qt_noop();
    }

    /* BelongsToMany::detach with Custom pivot */
    {
        qInfo() << "\n\nBelongsToMany::detach with Custom pivot\n---";

        Tag tag100({{"name", "tag100"}});
        tag100.save();
        Tag tag101({{"name", "tag101"}});
        tag101.save();

        auto torrent5 = Torrent::find(5);

        torrent5->tags()->attach({tag100, tag101},
                                 {{"active", false}});

        auto affected = torrent5->tags()->detach({tag100, tag101});
        qDebug() << "Detached affected :" << affected;

        tag100.remove();
        tag101.remove();

        qt_noop();
    }

    /* BelongsToMany::detach with Pivot */
    {
        qInfo() << "\n\nBelongsToMany::detach with Pivot\n---";

        Torrent torrent100 {
            {"name", "test100"}, {"size", 100}, {"progress", 555},
            {"hash", "xyzhash100"}, {"note", "detach with pivot"},
        };
        torrent100.save();
        Torrent torrent101 {
            {"name", "test101"}, {"size", 101}, {"progress", 556},
            {"hash", "xyzhash101"}, {"note", "detach with pivot"},
        };
        torrent101.save();

        auto tag4 = Tag::find(4);

        tag4->torrents()->attach({torrent100, torrent101},
                                 {{"active", true}});

        auto affected = tag4->torrents()->detach({torrent100, torrent101});
        qDebug() << "Detach affected :" << affected;

        torrent100.remove();
        torrent101.remove();

        qt_noop();
    }

    /* BelongsToMany::sync with Custom pivot */
    {
        qInfo() << "\n\nBelongsToMany::sync with Custom pivot\n---";

        Tag tag100({{"name", "tag100"}});
        tag100.save();
        Tag tag101({{"name", "tag101"}});
        tag101.save();
        Tag tag102({{"name", "tag102"}});
        tag102.save();
        Tag tag103({{"name", "tag103"}});
        tag103.save();

        auto torrent5 = Torrent::find(5);

        torrent5->tags()->attach({{tag101}, {tag102}});

        auto changed = torrent5->tags()->sync(
                           {{tag100["id"]->value<quint64>(), {{"active", true}}},
                            {tag101["id"]->value<quint64>(), {{"active", false}}},
                            {tag103["id"]->value<quint64>(), {{"active", true}}}});

        tag100.remove();
        tag101.remove();
        tag102.remove();
        tag103.remove();

        qt_noop();
    }

    /* BelongsToMany::sync with Pivot */
    {
        qInfo() << "\n\nBelongsToMany::sync with Pivot\n---";

        Torrent torrent100 {
            {"name", "test100"}, {"size", 100}, {"progress", 555},
            {"hash", "xyzhash100"}, {"note", "sync with pivot"},
        };
        torrent100.save();
        Torrent torrent101 {
            {"name", "test101"}, {"size", 101}, {"progress", 556},
            {"hash", "xyzhash101"}, {"note", "sync with pivot"},
        };
        torrent101.save();
        Torrent torrent102 {
            {"name", "test102"}, {"size", 102}, {"progress", 557},
            {"hash", "xyzhash102"}, {"note", "sync with pivot"},
        };
        torrent102.save();
        Torrent torrent103 {
            {"name", "test103"}, {"size", 103}, {"progress", 558},
            {"hash", "xyzhash103"}, {"note", "sync with pivot"},
        };
        torrent103.save();

        auto tag5 = Tag::find(5);

        tag5->torrents()->attach({torrent101, torrent102},
                                 {{"active", true}});

        auto changed = tag5->torrents()->sync(
                           {{torrent100["id"]->value<quint64>(), {{"active", true}}},
                            {torrent101["id"]->value<quint64>(), {{"active", false}}},
                            {torrent103["id"]->value<quint64>(), {{"active", true}}}});

        torrent100.remove();
        torrent101.remove();
        torrent102.remove();
        torrent103.remove();

        qt_noop();
    }

    /* Many-to-Many Relationship - with Pivot and with pivot attribute */
    {
        qInfo() << "\n\nMany-to-Many Relationship - with Pivot and "
                   "with pivot attribute ; also test u_timestamps\n---";

        auto roles = Role::with("users")->get();

        for (auto &role : roles)
            for (auto *user : role.getRelation<User>("users")) {
                qDebug() << user->getRelation<Pivot, Orm::One>("pivot")
                                ->getAttribute("active");

                // Should not use timestamps
                auto useTimestamps = user->getRelation<Pivot, Orm::One>("pivot")
                        ->usesTimestamps();
                qDebug() << "Timestamps :"  << useTimestamps;
                Q_ASSERT(!useTimestamps);
            }

        qt_noop();
    }

    /* Many-to-Many Relationship - with Custom pivot type, with pivot attribute and
       with custom relation name */
    {
        qInfo() << "\n\nMany-to-Many Relationship - with Custom pivot type, "
                   "with pivot attribute and with custom relation name ; also test "
                   "u_timestamps\n---";

        auto users = User::with("roles")->get();

        for (auto &user : users)
            for (auto *role : user.getRelation<Role>("roles")) {
                qDebug() << role->getRelation<RoleUser, Orm::One>("subscription")
                                ->getAttribute("active");

                // Should not use timestamps
                auto useTimestamps = role->getRelation<RoleUser, Orm::One>("subscription")
                        ->usesTimestamps();
                qDebug() << "Timestamps :"  << useTimestamps;
                Q_ASSERT(!useTimestamps);
            }

        qt_noop();
    }

    /* Many-to-Many Relationship - Custom pivot should use timestamps */
    {
        qInfo() << "\n\nMany-to-Many Relationship - Custom pivot should use "
                   "timestamps\n---";

        auto torrents = Torrent::with("tags")->get();

        for (auto &torrent : torrents)
            for (auto *tag : torrent.getRelation<Tag>("tags")) {
                qDebug() << tag->getRelation<Tagged, Orm::One>("tagged")
                        ->getAttribute("active");

                auto useTimestamps = tag->getRelation<Tagged, Orm::One>("tagged")
                        ->usesTimestamps();
                qDebug() << "Timestamps :"  << useTimestamps;
                Q_ASSERT(useTimestamps);
            }

        qt_noop();
    }

    /* Test with() overloads */
    {
        qInfo() << "\n\nTest with() overloads\n---";

        auto t1 = Torrent::query()->with("torrentPeer").find(2);
        auto t2 = Torrent::query()->with({"torrentPeer", "torrentFiles"}).find(2);
        auto t3 = Torrent::query()->with({{"torrentPeer"}, {"torrentFiles"}}).find(2);

        QVector<QString> w {"torrentPeer", "torrentFiles"};
        auto t4 = Torrent::query()->with(w).find(2);

        auto t5 = TorrentPeer().torrent()->with("torrentPeer").find(2);
        auto t6 = TorrentPeer().torrent()
                  ->with({"torrentPeer", "torrentFiles"}).find(2);
        auto t7 = TorrentPeer().torrent()
                  ->with({{"torrentPeer"}, {"torrentFiles"}}).find(2);

        auto t8 = TorrentPeer().torrent()->with(w).find(2);

        auto t9 = Torrent::query()->with({{"torrentPeer", [](auto &q)
                                          {
                                              q.oldest();
                                          }}}).find(2);
        auto t10 = TorrentPeer().torrent()->with({{"torrentPeer", [](auto &q)
                                                   {
                                                       q.oldest();
                                                   }}}).find(2);
        qt_noop();
    }

    /* Test findOrXx/firstOrXx/updateOrCreate on HasOneOrMany relations */
    {
        qInfo() << "\n\nTest findOrXx/firstOrXx/updateOrCreate on HasOneOrMany "
                   "relations\n---";

        auto t1 = Torrent::find(1)->torrentFiles()->findOrNew(1);
        Q_ASSERT(t1.exists);

        auto t2 = Torrent::find(1)->torrentFiles()->firstOrNew(
                      {{"filepath", "test1_file1.mkv"}},
                      {{"size", 1026},
                       {"progress", 123},
                       {"file_index", 2}});
        Q_ASSERT(t2.exists);

        auto t3 = Torrent::find(1)->torrentFiles()->firstOrNew(
                      {{"filepath", "test1_fileXX.mkv"}},
                      {{"size", 1026},
                       {"progress", 123},
                       {"file_index", 2}});
        Q_ASSERT(!t3.exists);
        Q_ASSERT(t3.getAttributesHash().size() == 5);

        auto t4 = Torrent::find(1)->torrentFiles()->firstOrCreate(
                      {{"filepath", "test1_file1.mkv"}},
                      {{"size", 1026},
                       {"progress", 123},
                       {"file_index", 2}});
        Q_ASSERT(t4.exists);
        Q_ASSERT(t4["id"]->value<quint64>() == 1);

        auto t5 = Torrent::find(1)->torrentFiles()->firstOrCreate(
                      {{"filepath", "test1_fileXX.mkv"}},
                      {{"size", 1026},
                       {"progress", 123},
                       {"file_index", 2}});
        Q_ASSERT(t5.exists);
        Q_ASSERT(t5["id"]->value<quint64>() > 9);

        auto t6 = Torrent::find(1)->torrentFiles()->updateOrCreate(
                      {{"filepath", "test1_file1.mkv"}},
                      {{"size", 1025}});
        Q_ASSERT(t6.exists);
        Q_ASSERT(t6["id"]->value<quint64>() == 1);

        auto t7 = Torrent::find(1)->torrentFiles()->updateOrCreate(
                      {{"filepath", "test1_fileYY.mkv"},
                       {"size", 1026},
                       {"progress", 123},
                       {"file_index", 3}});
        Q_ASSERT(t7.exists);
        Q_ASSERT(t7["id"]->value<quint64>() > 9);

        // Restore
        auto removed = TorrentPreviewableFile::destroy({t5["id"], t7["id"]});
        Q_ASSERT(removed == 2);

        Torrent::find(1)->torrentFiles()->whereEq("id", 1).update({{"size", 1024}});

        qt_noop();
    }

    /* Test find/findMany/findOrXX on BelongsToMany relation */
    {
        qInfo() << "\n\nTest find/findMany/findOrXX on BelongsToMany relation\n---";

        auto t1 = Torrent::find(2)->tags()->find(2);
        Q_ASSERT(t1->exists);

        auto t2 = Torrent::find(2)->tags()->findMany({2, 3});
        Q_ASSERT(t2.size() == 2);

        auto t3 = Torrent::find(2)->tags()->findOrNew(2);
        Q_ASSERT(t3.exists);

        auto t4 = Torrent::find(2)->tags()->findOrNew(20);
        Q_ASSERT(!t4.exists);
        Q_ASSERT(t4.getAttributesHash().empty());

        auto t5 = Torrent::find(2)->tags()->findOrFail(2);
        Q_ASSERT(t5.exists);

        TINY_VERIFY_EXCEPTION_THROWN(
                    Torrent::find(2)->tags()->findOrFail(20),
                    ModelNotFoundError);

        qt_noop();
    }

    /* Test first/firstOrXx/findOrXX/firstWhere/updateOrCreate on BelongsToMany
       relation */
    {
        qInfo() << "\n\nTest first/firstOrXx/findOrXX/firstWhere/updateOrCreate "
                   "on BelongsToMany relation\n---";

        auto t1 = Torrent::find(2)->tags()->first();
        Q_ASSERT(t1->exists);

        auto t2 = Torrent::find(2)->tags()->firstOrNew(
                      {{"name", "tag4"}},
                      {{"note", "firstOrNew"}});
        Q_ASSERT(t2.exists);

        auto t3 = Torrent::find(2)->tags()->firstOrNew(
                      {{"name", "tag20"}},
                      {{"note", "firstOrNew"}});
        Q_ASSERT(!t3.exists);
        Q_ASSERT(t3.getAttributesHash().size() == 2);

        auto t4 = Torrent::find(2)->tags()->firstOrNew(
                      {{"id", 100}, {"name", "tag20"}},
                      {{"note", "firstOrNew"}});
        Q_ASSERT(!t4.exists);
        Q_ASSERT(t4.getAttributesHash().size() == 2);

        auto t5 = Torrent::find(2)->tags()->firstOrCreate(
                      {{"name", "tag4"}},
                      {{"note", "firstOrCreate"}},
                      {{"active", false}});
        Q_ASSERT(t5.exists);

        auto t6 = Torrent::find(2)->tags()->firstOrCreate(
                      {{"name", "tag20"}},
                      {{"note", "firstOrCreate"}},
                      {{"active", false}});
        Q_ASSERT(t6.exists);
        Q_ASSERT(t6["id"]->value<quint64>() > 5);

        auto t7 = Torrent::find(2)->tags()->firstOrCreate(
                      {{"id", 100}, {"name", "tag21"}},
                      {{"note", "firstOrCreate"}},
                      {{"active", false}});
        Q_ASSERT(t7.exists);
        Q_ASSERT(t7["id"]->value<quint64>() > 5);
        Q_ASSERT(t7["id"]->value<quint64>() != 100);

        auto t8 = Torrent::find(2)->tags()->firstWhereEq("name", "tag4");
        Q_ASSERT(t8->exists);

        auto t9 = Torrent::find(2)->tags()->whereEq("name", "tag4").firstOrFail();
        Q_ASSERT(t9.exists);

        TINY_VERIFY_EXCEPTION_THROWN(
                    Torrent::find(4)->tags()->whereEq("name", "tag30").firstOrFail(),
                    ModelNotFoundError);

        auto t10 = Torrent::find(2)->tags()->updateOrCreate(
                       {{"name", "tag4"}},
                       {{"name", "tag4"}, {"note", "updateOrCreate"}},
                       {{"active", false}});
        Q_ASSERT(t10.exists);
        Q_ASSERT(t10["id"]->value<quint64>() == 4);

        auto t11 = Torrent::find(2)->tags()->updateOrCreate(
                       {{"name", "tag40"}},
                       {{"name", "tag40"}, {"note", "updateOrCreate"}},
                       {{"active", false}});
        Q_ASSERT(t11.exists);
        Q_ASSERT(t11["id"]->value<quint64>() > 5);

        // Restore
        auto removed = Tag::destroy({t6["id"], t7["id"], t11["id"]});
        Q_ASSERT(removed == 3);

        Torrent::find(2)->tags()->updateOrCreate(
                    {{"name", "tag4"}},
                    {{"name", "tag4"},
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
                     {"note", QVariant(QMetaType::QString)}},
#else
                     {"note", QVariant(QVariant::String)}},
#endif
                    {{"active", true}});

        qt_noop();
    }

    logQueryCounters(__FUNCTION__, timer.elapsed());
}

void TestOrm::testQueryBuilder()
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
////                         {"created_at", QDateTime().currentDateTime().toString(Qt::ISODate)}});
//        qDebug() << "TENTH";
//        qDebug() << "last id :" << id;

//        qt_noop();
//    }

    // Insert with boolean value, Qt converts it to int
//    {
//        auto q = DB::table("torrents")->insert(
//                     {{"name", "bool test"}, {"size", 2048}, {"progress", 300},
////                      {"added_on", QDateTime().currentDateTime().toString(Qt::ISODate)},
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
//        {{"torrent_id", id_i}, {"file_index", 0}, {"filepath", "abc.mkv"}, {"size", 2048},
//            {"progress", 10}},
//        {{"torrent_id", id_i}, {"file_index", 1}, {"filepath", "xyz.mkv"}, {"size", 1024},
//            {"progress", 15}}});
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

            if (DB::getDefaultConnection() == "mysql") {
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
//            qInfo() << "\n\nQueryBuilder::remove() [sqlite] - with join\n---";

//            auto [affected, _] = DB::table("torrents", "", "sqlite")
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
//            .update({{"name", QVariant::fromValue(Expression("first"))}, {"progress", 350}});
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

        auto query = DB::table("torrents")->joinSub([](auto &query)
        {
            query.from("torrent_previewable_files")
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

    logQueryCounters(__FUNCTION__, timer.elapsed());
}

void TestOrm::testQueryBuilderDbSpecific()
{
    QElapsedTimer timer;
    timer.start();

    resetAllQueryLogCounters();

    qInfo().nospace()
            << "\n\n===================================="
            << "\n  QueryBuilder - database specific  "
            << "\n====================================";

    /* QueryBuilder::fromSub() - cross database query */
    {
        qInfo() << "\n\nQueryBuilder::fromSub() - cross database query\n---";

        auto query1 = DB::query("mysql_laravel8")->from("users")
                     .select({"id", "name"});

        auto query = DB::query("mysql")
                     ->fromSub(query1, "u")
                     .where("id", "<", 3)
                     .get();

        while (query.next())
            qDebug() << "id :" << query.value("id").value<quint64>() << ";"
                     << "name :" << query.value("name").value<QString>();

        qt_noop();
    }

    logQueryCounters(__FUNCTION__, timer.elapsed());
}

void TestOrm::ctorAggregate()
{
//    {
//        AggTest a {.name = "xx", .value = 1};
//        AggTest a;
//        a.name = "xx";
//        a.value = 1;
//        OtherTest ot {.name = "zz", .value = 10};
//        OtherTest ot;
//        ot.name = "zz";
//        ot.value = 10;
//        OtherTest o = a.toOther();

//        a.name = std::move(ot.name);
//        ot = std::move(a);

//        OtherTest o = a;

//        std::vector<AggTest> va {{"x", 1}, {"y", 2}, {"z", 3}};
//        std::vector<OtherTest> vo;

//        //        std::vector<OtherTest> vo {{"a", 10}, {"b", 20}, {"c", 30}};
//        //        std::copy(va.begin(), va.end(), std::back_inserter(vo));
//        std::move(va.begin(), va.end(), std::back_inserter(vo));
//        ranges::move(std::move(va), ranges::back_inserter(vo));
//        qt_noop();
    //    }
}

void TestOrm::jsonConfig()
{
//    {
//        auto j = R"(
//{
//    "default": "mysql",

//    "connections": {

//        "sqlite": {
//            "driver": "sqlite",
//            "url": "",
//            "database": "database.sqlite",
//            "prefix": "",
//            "foreign_key_constraints": true
//        },
//        "mysql": {
//            "driver": "mysql",
//            "url": "xx",
//            "database": "database.mysql",
//            "prefix": ""
//        }
//    }
//}
//)"_json;

//        std::string s = j.dump();

//        std::cout << j.dump(4) << std::endl;
//    }

    {
        auto s = R"(
{
    "default": "mysql",

    "connections": {

        "sqlite": {
            "driver": "sqlite",
            "url": "",
            "database": "database.sqlite",
            "prefix": "",
            "foreign_key_constraints": true
        },
        "mysql": {
            "driver": "mysql",
            "url": "xx",
            "database": "database.mysql",
            "prefix": ""
        }
    }
}
)";

        auto j = json::parse(s);
//        std::string s = j.dump();

//        std::cout << j.dump(4) << std::endl;

//        std::cout << j["default"].get<std::string>();
//        std::cout << j["connections"]["mysql"]["database"].get<std::string>();

        std::cout << j["connections"]["mysql"]["database"];
        std::cout << j["/connections/mysql/database"_json_pointer];

        auto x = j["connections"]["mysql"]["database"];
        std::cout << j["/connections/mysql/database"_json_pointer];

//        qDebug() << QString::fromStdString(j["default"].get<std::string>());

//        for (auto &[key, value] : j.items()) {
//            std::cout << key << " : " << value << "\n";
//        }

        qt_noop();
    }

    // read a JSON file
//    std::ifstream i("config_test.json");
//    json j;
//    i >> j;

    // write prettified JSON to another file
//    std::ofstream o("pretty.json");
//    o << std::setw(4) << j << std::endl;

    qt_noop();
}

void TestOrm::standardPaths()
{
    // Exclude from prod. builds for better performance measurement accuracy
    if (!QLibraryInfo::isDebugBuild())
        return;

    qDebug() << QStandardPaths::displayName(QStandardPaths::AppDataLocation);
    qDebug() << QStandardPaths::displayName(QStandardPaths::AppLocalDataLocation);
    qDebug() << QStandardPaths::displayName(QStandardPaths::ApplicationsLocation);

    qDebug() << QStandardPaths::findExecutable("php");

    qDebug() << "\nAppDataLocation:";
    qDebug() << QStandardPaths::standardLocations(QStandardPaths::AppDataLocation);
    qDebug() << QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    qDebug() << "\nAppLocalDataLocation:";
    qDebug() << QStandardPaths::standardLocations(QStandardPaths::AppLocalDataLocation);
    qDebug() << QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);

    qDebug() << "\nAppConfigLocation:";
    qDebug() << QStandardPaths::standardLocations(QStandardPaths::AppConfigLocation);
    qDebug() << QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);

    qDebug() << "\nGenericConfigLocation:";
    qDebug() << QStandardPaths::standardLocations(QStandardPaths::GenericConfigLocation);
    qDebug() << QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation);

    qDebug() << "\nGenericDataLocation:";
    qDebug() << QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation);
    qDebug() << QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);

    qDebug() << "\nGenericCacheLocation:";
    qDebug() << QStandardPaths::standardLocations(QStandardPaths::GenericCacheLocation);
    qDebug() << QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation);

    qDebug() << "\nCacheLocation:";
    qDebug() << QStandardPaths::standardLocations(QStandardPaths::CacheLocation);
    qDebug() << QStandardPaths::writableLocation(QStandardPaths::CacheLocation);

    qDebug() << "\nTempLocation:";
    qDebug() << QStandardPaths::standardLocations(QStandardPaths::TempLocation);
    qDebug() << QStandardPaths::writableLocation(QStandardPaths::TempLocation);

    qt_noop();
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
        m_appFunctionsElapsed += *functionElapsed;

    // Total counters for the summary
    int summaryElapsed = -1;
    StatementsCounter summaryStatementsCounter;
    bool summaryRecordsHaveBeenModified = false;

    /* If any connection count statements, then counters will be -1, set them
       to the zero values only if some connection count statements, the same is true
       for queries execution time counter. 🧹 */
    if (DB::anyCountingStatements())
        summaryStatementsCounter = {0, 0, 0};
    if (DB::anyCountingElapsed())
        summaryElapsed = 0;

    // Log all connections
    for (const auto &connectionName : CONNECTIONS_TO_COUNT) {
        auto &connection = DB::connection(connectionName);

        // Queries execution time
        const auto elapsed = connection.takeElapsedCounter();
        // Don't count if counting is not enabled
        if (connection.countingElapsed()) {
            summaryElapsed += elapsed;
            m_appElapsed += elapsed;
        }

        // Executed statements counter
        const auto statementsCounter = connection.takeStatementsCounter();
        // Count only when the counting is enabled
        if (connection.countingStatements()) {
            summaryStatementsCounter.normal        += statementsCounter.normal;
            summaryStatementsCounter.affecting     += statementsCounter.affecting;
            summaryStatementsCounter.transactional += statementsCounter.transactional;

            m_appStatementsCounter.normal        += statementsCounter.normal;
            m_appStatementsCounter.affecting     += statementsCounter.affecting;
            m_appStatementsCounter.transactional += statementsCounter.transactional;
        }

        // Whether recods have been modified
        const auto recordsHaveBeenModified = connection.getRecordsHaveBeenModified();
        summaryRecordsHaveBeenModified |= recordsHaveBeenModified;
        m_appRecordsHaveBeenModified |= recordsHaveBeenModified;

        // Log connection statistics
        logQueryCountersBlock(
                    QStringLiteral("Connection name - '%1'").arg(connectionName),
                    elapsed, statementsCounter, recordsHaveBeenModified);
    }

    // Summary
    logQueryCountersBlock(QStringLiteral("Summary"),
                          summaryElapsed, summaryStatementsCounter,
                          summaryRecordsHaveBeenModified);

    qInfo().noquote() << line;
}

void TestOrm::logQueryCountersBlock(
            const QString &title, const qint64 elapsed,
            const StatementsCounter statementsCounter,
            const bool recordsHaveBeenModified) const
{
    // Header
    if (title.contains("Application")) {
        qInfo() << "\n-----------------------";
        qInfo().noquote().nospace() << "  " << title;
        qInfo() << "-----------------------";
    } else {
        qInfo() << "";
        qInfo().noquote() << title;
        qInfo() << "---";
    }

#ifdef _MSC_VER
    if (title.contains("Application"))
        qInfo() << "⚙ _MSC_VER                 :" << _MSC_VER;
#endif

    if (title.contains("Application")) {
        qInfo().nospace() << "  Qt version               : "
                          << QT_VERSION_STR;
        qInfo().nospace() << "  Build type               : "
                          << (QLibraryInfo::isDebugBuild() ? "Debug" : "Release")
                          << "\n";

        // All Functions execution time
        qInfo().nospace() << "⚡ Functions execution time : "
                          << m_appFunctionsElapsed << "ms\n";
    }

    // Counters on connections
    if (title.contains("Summary"))
        qInfo().nospace() << "∑ " << "Counted connections    "
                          << (title.contains("Application") ? "  " : "")
                          << ": "
                          << CONNECTIONS_TO_COUNT.size();

    // Queries execution time
    qInfo().nospace() << "⚡ Queries execution time "
                      << (title.contains("Application") ? "  " : "")
                      << ": "
                      << elapsed << (elapsed > -1 ? "ms" : "");

    // Whether records have been modified on the current connection
    qInfo().nospace() << "✎ Records was modified   "
                      << (title.contains("Application") ? "  " : "")
                      << ": "
                      << (recordsHaveBeenModified ? "yes" : "no");

    // Count total executed queries
    const auto &[normal, affecting, transactional] = statementsCounter;
    int total = normal == -1 && affecting == -1 && transactional == -1 ? -1 : 0;
    if (normal != -1)
        total += normal;
    if (affecting != -1)
        total += affecting;
    if (transactional != -1)
        total += transactional;

    qInfo() << "⚖ Statements counters";
    qInfo() << "  Normal        :" << normal;
    qInfo() << "  Affecting     :" << affecting;
    qInfo() << "  Transaction   :" << transactional;
    qInfo() << "  Total         :" << total;
    qInfo() << "---";
}

QString TestOrm::getCheckDatabaseExistsFile()
{
    auto path = qEnvironmentVariable("DB_SQLITE_DATABASE", "");

    if (path.isEmpty())
        throw std::invalid_argument(
                "Undefined environment variable 'DB_SQLITE_DATABASE'.");

    path.truncate(QDir::fromNativeSeparators(path).lastIndexOf(QChar('/')));

    return path + "/q_tinyorm_test-check_exists.sqlite3";
}

void TestOrm::resetAllQueryLogCounters() const
{
    DB::resetElapsedCounters(CONNECTIONS_TO_COUNT);
    DB::resetStatementCounters(CONNECTIONS_TO_COUNT);

    for (const auto &connection : CONNECTIONS_TO_COUNT)
        DB::forgetRecordModificationState(connection);
}
