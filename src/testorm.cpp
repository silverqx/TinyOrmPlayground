#include "testorm.hpp"

#include <QDebug>
#include <QStandardPaths>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <optional>
//#include <format>

#include <nlohmann/json.hpp>
#include <range/v3/all.hpp>

#include <orm/ormtypes.hpp>
#include "models/setting.hpp"
#include "models/torrent.hpp"
#include "models/torrentpeer.hpp"
#include "models/torrentpreviewablefile.hpp"

using namespace ranges;

using json = nlohmann::json;

TestOrm::TestOrm()
//    : m_dm(Orm::DatabaseManager::create({
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
//    }))
    : m_dm(Orm::DatabaseManager::create({
        {"tinyorm_default", {
             {"driver",    "QMYSQL"},
             {"host",      qEnvironmentVariable("DB_HOST", "127.0.0.1")},
             {"port",      qEnvironmentVariable("DB_PORT", "3306")},
             {"database",  qEnvironmentVariable("DB_DATABASE", "")},
             {"username",  qEnvironmentVariable("DB_USERNAME", "")},
             {"password",  qEnvironmentVariable("DB_PASSWORD", "")},
             {"charset",   qEnvironmentVariable("DB_CHARSET", "utf8mb4")},
             {"collation", qEnvironmentVariable("DB_COLLATION", "utf8mb4_0900_ai_ci")},
             {"prefix",    ""},
             {"strict",    true},
             {"options",   QVariantHash()},
        }},
        {"crystal", {
             {"driver",    "QMYSQL"},
             {"host",      qEnvironmentVariable("DB_HOST", "127.0.0.1")},
             {"port",      qEnvironmentVariable("DB_PORT", "3306")},
             {"database",  qEnvironmentVariable("DB_DATABASE", "")},
             {"username",  qEnvironmentVariable("DB_USERNAME", "")},
             {"password",  qEnvironmentVariable("DB_PASSWORD", "")},
             {"charset",   qEnvironmentVariable("DB_CHARSET", "utf8mb4")},
             {"collation", qEnvironmentVariable("DB_COLLATION", "utf8mb4_0900_ai_ci")},
             {"prefix",    ""},
             {"strict",    true},
             {"options",   QVariantHash()},
        }},
    }, "tinyorm_default"))
{}

void TestOrm::run()
{
//    ctorAggregate();
//    anotherTests();
    testTinyOrm();
    testQueryBuilder();
//    jsonConfig();
//    standardPaths();
}

void TestOrm::anotherTests()
{
    printf("Function name: %s\n", __FUNCTION__);
    printf("Decorated function name: %s\n", __FUNCDNAME__);
    printf("Function signature: %s\n", __FUNCSIG__);
    qt_noop();

    // formatting
    // ---
//    int num = 100;
//    std::string s = "hello";

//    printf("before: %i\n", num);
////    printf("before: %f\n", num);
//    printf("before: %s\n", s.c_str());

//    std::string message = std::format("The answer is {}.", 42);

//    qt_noop();
}

void TestOrm::testTinyOrm()
{
//    {
//        TorrentPreviewableFile a;
//        auto files = a.query()->where("torrent_id", "=", 2).get();
//////        auto files = a.query()->where("torrent_id", "=", 261).get();
//////        auto torrent1 = files.first().getRelation<Torrent, QVector>("torrent");
//        auto torrent1 = files.first().getRelation<Torrent, Orm::One>("torrent");
////        qDebug() << torrent1->getAttribute("name");
////        auto peer1 = torrent1->getRelation<TorrentPeer, Orm::One>("torrentPeer");
//////        auto torrent2 = files.first().getRelationValue<Torrent>("torrent");
////        auto torrent3 = files.first().getRelationValue<Torrent, Orm::One>("torrent");
////        auto torrent3 = a.getRelationValue<Torrent, Orm::One>("torrent");
//////        qDebug() << torrent2;
//////        qDebug() << torrent3.has_value();
//        qt_noop();
//    }

    /* Basic get all */
    {
        Torrent torrent;
        auto torrents = torrent.query()->get();

        for (auto &t : torrents)
            qDebug() << "id :" << t.getAttribute("id").toULongLong() << ";"
                     << "name :" << t.getAttribute("name").toString();

        qt_noop();
    }
    {
        for (auto &t : Torrent::all())
            qDebug() << "id :" << t.getAttribute("id").toULongLong() << ";"
                     << "name :" << t.getAttribute("name").toString();

        qt_noop();
    }

    /* Model::where() */
    {
        for (auto &t : Torrent::where("id", ">", 3)->get())
            qDebug() << "id :" << t.getAttribute("id").toULongLong() << ";"
                     << "name :" << t.getAttribute("name").toString();

        qt_noop();
    }

    /* Model::whereEq() */
    {
        auto t = Torrent::whereEq("id", 4)->get().first();
        qDebug() << "id :" << t["id"].toULongLong() << ";"
                 << "name :" << t["name"].toString();
//        qDebug() << "id :" << t.getAttribute("id").toULongLong() << ";"
//                 << "name :" << t.getAttribute("name").toString();

        qt_noop();
    }

    /* Model::where() vector */
    {
        for (auto &t : Torrent::where({{"id", 4}})->get())
            qDebug() << "id :" << t.getAttribute("id").toULongLong() << ";"
                     << "name :" << t.getAttribute("name").toString();

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

//    m_dm.query()->from("torrent_previewable_files")
//            .insert(values);
//    qt_noop();

//    Torrent b;
//    auto torrents = b.query()->where("id", "=", 2).get();
//    auto peer = torrents.first().getRelation<TorrentPeer, Orm::One>("torrentPeer");
////////    auto peer = torrents.first().getRelationValue<TorrentPeer, Orm::One>("torrentPeer");
////////    auto peer = b.getRelationValue<TorrentPeer, Orm::One>("torrentPeer");
//////    qDebug() << "peers :" << !!peer;
////    auto files = torrents.first().getRelation<TorrentPreviewableFile>("torrentFiles");
////////    auto files = b.getRelationValue<TorrentPreviewableFile>("torrentFiles");
//    qt_noop();

    /* Model::remove() */
//    {
//        TorrentPreviewableFile a;
//        auto files = a.query()->where("id", "=", 1005).get();
//        auto torrent1 = files.first().getRelation<Torrent, Orm::One>("torrent");
//        auto peer1 = torrent1->getRelation<TorrentPeer, Orm::One>("torrentPeer");
//        auto file1 = files.first();
//        auto res = file1.remove();
//        qt_noop();
//    }

    /* Model::destroy() */
//    {
//        auto count = TorrentPreviewableFile::destroy({1004, 1003});
//        qDebug() << count;
//        qt_noop();
//    }

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
//        auto torrentFile = TorrentPreviewableFile().query()->where("id", "=", 1012).first();

//        torrentFile->setAttribute("file_indexx", 11);

//        auto result = torrentFile->save();
//        qDebug() << "save :" << result;
//        qt_noop();
//    }

    /* Model::find(id) */
    {
        const auto id = 3;
        auto torrentFile = TorrentPreviewableFile::find(id);

        qDebug() << "torrentFile id :" << id;
        qDebug() << torrentFile->getAttribute("filepath").toString();

        qt_noop();
    }

    /* Model::findWhere(id) */
    {
        auto torrentFile = TorrentPreviewableFile::whereEq("id", 3)->first();
        auto torrentFile1 = TorrentPreviewableFile::firstWhere("id", "=", 4);
        auto torrentFile2 = TorrentPreviewableFile::firstWhereEq("id", 5);

        qt_noop();
    }

    /* Model::firstOrNew() */
    {
        auto torrent = Torrent::firstOrNew(
                           {{"id", 10}},

                           {{"name", "test10"},
                            {"size", 20},
                            {"progress", 800},
                            {"id", 15}
                           });

        qt_noop();
    }

    /* Model::firstOrCreate() */
//    {
//        auto torrent = Torrent::firstOrCreate(
//                           {{"name", "test10"}},

//                           {{"size", 20},
//                            {"progress", 800},
//                            {"hash", "0679e3af2768cdf52ec84c1f320333f68401dc6e"},
////                            {"id", 15},
//                           });

//        if (!torrent.exists)
//            // TODO next throw exception really needed here silverqx
//            qDebug() << "Torrent was not created successfuly, name :"
//                     << torrent.getAttribute("name").toString();

//        qt_noop();
//    }

    /* hash which manages insertion order */
//    {
//        QHash<QString *, std::reference_wrapper<QVariant>> hash;
//        QVector<Orm::AttributeItem> vec;

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
//            qDebug() << val.toString();
//        }
//        qDebug() << "\nvector:";
//        for (auto &val : vec) {
//            qDebug() << val.value.toString();
//        }
//        qDebug() << "\nothers:";
//        qDebug() << hash.find(&b.key).value().get().toString();
////        qDebug() << hash[&b.key].get().toString();
////        qDebug() << hash[&d.key]->toString();
////        qDebug() << hash[&b.key]->toString();
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

    /* Model::push() */
//    {
//        Torrent t;
//        auto torrent = t.find(2);
//        // eager load
//        auto files = torrent->getRelation<TorrentPreviewableFile>("torrentFiles");
//        auto file = files.first();
//        auto *fileProperty = file->getRelation<TorrentPreviewableFileProperty, Orm::One>("fileProperty");
//        // lazy load
////        auto files = torrent->getRelationValue<TorrentPreviewableFile>("torrentFiles");
////        auto *file = files.first();
////        auto *fileProperty = file->getRelationValue<TorrentPreviewableFileProperty, Orm::One>("fileProperty");

//        auto torrentName      = torrent->getAttribute("name").toString();
//        auto filepath         = file->getAttribute("filepath").toString();
//        auto filePropertyName = fileProperty->getAttribute("name").toString();
//        qDebug() << torrentName;
//        qDebug() << filepath;
//        qDebug() << filePropertyName;

//        torrent->setAttribute("name", QVariant(torrentName + " x"));
//        file->setAttribute("filepath", QVariant(filepath + ".xy"));
//        fileProperty->setAttribute("name", QVariant(filePropertyName + " x"));

//        const auto debugAttributes = [&torrent]
//        {
//            auto files = torrent->getRelation<TorrentPreviewableFile>("torrentFiles");
//            auto file = files.first();
//            auto *fileProperty = file->getRelation<TorrentPreviewableFileProperty, Orm::One>("fileProperty");
//            qDebug() << torrent->getAttribute("name").toString();
//            qDebug() << file->getAttribute("filepath").toString();
//            qDebug() << fileProperty->getAttribute("name").toString();
//        };

//        debugAttributes();
//        torrent->push();

//        // Set it back, so I don't have to manually
//        torrent->setAttribute("name", torrentName);
//        file->setAttribute("filepath", filepath);
//        fileProperty->setAttribute("name", filePropertyName);

//        debugAttributes();
//        torrent->push();

//        qt_noop();
//    }

    /* eager/lazy load - getRelation(), getRelationValue() and with() */
    {
        auto torrent = Torrent::find(2);

        // eager load
//        {
//            qDebug() << "\neager load:";
//            auto files = torrent->getRelation<TorrentPreviewableFile>("torrentFiles");

//            for (auto &file : files) {
//                qDebug().nospace().noquote()
//                        << "\nFile : "
//                        << file->getAttribute("filepath").toString()
//                        << "(" << file->getAttribute("id").toString() << ")";

//                auto fileProperty = file->getRelation<TorrentPreviewableFileProperty, Orm::One>("fileProperty");

//                qDebug().nospace().noquote()
//                        << fileProperty->getAttribute("name").toString()
//                        << "(" << fileProperty->getAttribute("id").toString() << ")";
//                qt_noop();
//            }
//        }

        // lazy load
        {
            qDebug() << "\nlazy load, !!comment out 'with' relation to prevent eager load";
            qDebug() << "\n↓↓↓ lazy load works, if an Executed query is logged under this comment:";
            auto files = torrent->getRelationValue<TorrentPreviewableFile>("torrentFiles");

            for (auto &file : files) {
                qDebug().nospace().noquote()
                        << "\nFile : "
                        << file->getAttribute("filepath").toString()
                        << "(" << file->getAttribute("id").toString() << ")";

                auto fileProperty = file->getRelationValue<TorrentPreviewableFileProperty, Orm::One>("fileProperty");

                qDebug().nospace().noquote()
                        << fileProperty->getAttribute("name").toString()
                        << "(" << fileProperty->getAttribute("id").toString() << ")";
                qt_noop();
            }
        }

        qt_noop();
    }

    /* Model::with() - One */
    {
        auto torrents = Torrent().with("torrentPeer")->get();
        auto peer = torrents[1].getRelation<TorrentPeer, Orm::One>("torrentPeer");
        qDebug() << peer->getAttribute("id");
        qt_noop();
    }
    {
        auto files = TorrentPreviewableFile().with("torrent")->get();
        auto torrent = files[1].getRelation<Torrent, Orm::One>("torrent");
        qDebug() << torrent->getAttribute("id");
        qt_noop();
    }

    /* Model::with() - Many */
    {
        auto torrents = Torrent().with("torrentFiles")->get();
        auto files = torrents[1].getRelation<TorrentPreviewableFile>("torrentFiles");
        for (const auto &file : files)
            qDebug() << file->getAttribute("filepath");
        qt_noop();
    }

    qt_noop();
}

//OtherTest::OtherTest(AggTest &a)
//{
//    name = a.name;
//    value = a.value;
//}

void TestOrm::testQueryBuilder()
{
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
//        auto [ok, query] = m_dm.query()->from("torrents")
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
//            qDebug() << "id :" << query.value("id") << "; name :" << query.value("name");
//        }
//        qt_noop();
//    }

//    auto [ok, b] = m_dm.query()->from("torrents").get({"id, name"});
//    qDebug() << "SECOND :" << b.executedQuery();
//    while (b.next()) {
//        qDebug() << "id :" << b.value("id") << "; name :" << b.value("name");
//    }
//    qt_noop();

    /* WHERE */
    auto [ok, c] = m_dm.query()->from("torrents")
             .where("name", "=", "test2", "and")
             .get({"id", "name"});
    qDebug() << "THIRD :" << c.executedQuery();
    while (c.next()) {
        qDebug() << "id :" << c.value("id") << "; name :" << c.value("name");
    }
    qt_noop();

    /* WHERE - like */
//    {
//        auto [ok, c] = m_dm.query()->from("torrents")
//                .where("name", "like", "%no peers", "and")
//                .get({"id", "name"});
//        qDebug() << "THIRD :" << c.executedQuery();
//        while (c.next()) {
//            qDebug() << "id :" << c.value("id") << "; name :" << c.value("name");
//        }
//        qt_noop();
//    }

    /* also nested where */
//    auto [ok, d] = m_dm.query()->from("torrents")
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
//    auto [ok_c1, c1] = m_dm.query()->from("torrents")
//             .where("name", "=", "Internat - S1", "and")
//             .get({"id", "name"});
//    qDebug() << "whereIn :" << c1.executedQuery();
//    while (c1.next()) {
//        qDebug() << "id :" << c1.value("id") << "; name :" << c1.value("name");
//    }
//    qt_noop();

    /* where() - an array of basic where clauses */
//    {
//        auto [ok, query] = m_dm.query()->from("torrents")
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
//        auto [ok, query] = m_dm.query()->from("torrents")
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

    /* JOINs */
//    auto e = m_dm.query()->from("torrents")
//             .rightJoin("torrent_previewable_files", "torrents.id", "=", "torrent_id")
//             .where("torrents.id", "=", 256)
//             .get({"torrents.id", "name", "file_index", "filepath"});
//    qDebug() << "FIFTH :" << e.executedQuery();
//    while (e.next()) {
//        qDebug() << "id :" << e.value("id") << "; name :" << e.value("name")
//                 << "file_index :" << e.value("file_index")
//                 << "filepath :" << e.value("filepath");
//    }

//    auto e = m_dm.query()->from("torrents")
//             .join("torrent_previewable_files", [](auto &join)
//    {
//        join.on("torrents.id", "=", "torrent_id")
//                .where("torrents.progress", "<", 20);
//    })
////             .where("torrents.id", "=", 256)
//             .get({"torrents.id", "name", "file_index", "filepath"});
//    qDebug() << "SIXTH :" << e.executedQuery();
//    while (e.next()) {
//        qDebug() << "id :" << e.value("id") << "; name :" << e.value("name")
//                 << "file_index :" << e.value("file_index")
//                 << "filepath :" << e.value("filepath");
//    }
//    qt_noop();

    /* first and find */
//    auto f = m_dm.query()->from("torrents")
//            .where("torrents.id", "=", 256)
////            .first({"id", "name"});
//            .value("name");
////    qDebug() << "SEVENTH :" << f.executedQuery();
////    qDebug() << "id :" << f.value("id") << "; name :" << f.value("name");
//    qDebug() << "name:" << f;
//    qt_noop();

//    auto [ok, g] = m_dm.query()->from("torrents")
//            .find(256, {"id", "name"});
//    qDebug() << "EIGTH :" << g.executedQuery();
//    qDebug() << "id :" << g.value("id") << "; name :" << g.value("name");
//    qt_noop();

    /* GROUP BY and HAVING */
//    auto h = m_dm.query()->from("torrents")
//             .groupBy({"status"})
////             .having("status", ">", 10)
////             .having("status", ">", "Paused")
//             .having("status", ">", static_cast<int>(TorrentStatus::Paused))
//             .get({"id", "name", "status"});
//    qDebug() << "NINETH :" << h.executedQuery();
//    while (h.next()) {
//        qDebug() << "id :" << h.value("id") << "; name :" << h.value("name")
//                 << "status :" << h.value("status");
//    }

    /* INSERTs */
//    auto id_i = m_dm.query()->from("torrents").insertGetId(
//                    {{"name", "first"}, {"progress", 300}, {"eta", 8000000}, {"size", 2048},
//                     {"seeds", 0}, {"total_seeds", 0}, {"leechers", 0}, {"total_leechers", 0},
//                     {"remaining", 1024},
//                     {"added_on", QDateTime().currentDateTime().toString(Qt::ISODate)},
//                     {"hash", "xxxx61defa3daecacfde5bde0214c4a439351d4d"},
//                     {"status", static_cast<int>(TorrentStatus::Stalled)},
//                     {"savepath", "D:/downloads/uTorrent/downloads/_dev1/_videos"}});
//    qDebug() << "TENTH";
//    qDebug() << "last id :" << id_i;
//    qt_noop();

    // Insert with boolean value, Qt converts it to int
//    {
//        auto [ok, q] = m_dm.query()->from("torrents").insert(
//                        {{"name", "bool test"}, {"size", 2048}, {"progress", 300},
////                         {"added_on", QDateTime().currentDateTime().toString(Qt::ISODate)},
//                         {"hash", "xxxx61defa3daecacfde5bde0214c4a439351d4d"},
//                         {"bool", false}});
//        qt_noop();
//    }

    // Empty attributes
//    {
//        auto id = m_dm.query()->from("settings").insertGetId({});
//        qDebug() << "Insert - empty attributes";
//        qDebug() << "last id :" << id;
//        qt_noop();
//    }

//    const auto id_i = 278;
//    auto [ok_j, j] = m_dm.query()->from("torrent_previewable_files").insert({
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

//    auto [ok_k, k] = m_dm.query()->from("torrent_previewable_files").insert({
//        {"torrent_id", id_i}, {"file_index", 2}, {"filepath", "qrs.mkv"}, {"size", 3074},
//        {"progress", 20}});
//    qDebug() << "TWELVE :" << k->executedQuery();
//    if (ok_k) {
//        qDebug() << "last id :" << k->lastInsertId()
//                 << "; affected rows :" << k->numRowsAffected();
//    }
//    qt_noop();

//    const auto id_l = 1;
//    auto [ok_l, l] = m_dm.query()->from("torrent_previewable_files").insertOrIgnore({
//        {{"torrent_id", id_l}, {"file_index", 2}, {"filepath", "qrs.mkv"}, {"size", 3074},
//            {"progress", 20}},
//        {{"torrent_id", id_l}, {"file_index", 3}, {"filepath", "ghi.mkv"}, {"size", 3074},
//            {"progress", 20}},
//        {{"torrent_id", id_l}, {"file_index", 4}, {"filepath", "def.mkv"}, {"size", 3074},
//            {"progress", 20}},
//    });
//    qDebug() << "TWELVE :" << l->executedQuery();
//    if (ok_l) {
//        qDebug() << "last id :" << l->lastInsertId() // undefined behavior
//                 << "; affected rows :" << l->numRowsAffected();
//    }
//    qt_noop();

    /* UPDATEs */
//    auto [affected_m, m] = m_dm.query()->from("torrents")
//            .where("id", "=", 277)
//            .update({{"name", "first1"}, {"progress", 350}});
//    qDebug() << "THIRTEEN :" << m.executedQuery();
//    qDebug() << "affected rows :" << affected_m;
//    qt_noop();

//    auto [affected_n, n] = m_dm.query()->from("torrents")
//            .join("torrent_previewable_files", "torrents.id", "=",
//                  "torrent_previewable_files.torrent_id")
//            .where("torrents.id", "=", 277)
//            .where("torrent_previewable_files.id", "=", 100)
//            .update({{"name", "first4"}, {"torrents.progress", 354},
//                    {"torrent_previewable_files.progress", 15}});
//    qDebug() << "FOURTEEN :" << n.executedQuery();
//    qDebug() << "affected rows :" << affected_n;
//    qt_noop();

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

//    auto [affected_o, o] = m_dm.query()->from("torrents")
//            .where("id", "=", 277)
//            .update({{"name", QVariant::fromValue(Expression("first"))}, {"progress", 350}});
////            .update({{"name", x}, {"progress", 350}});
//    qDebug() << "FIFTEEN :" << o.executedQuery();
//    qDebug() << "affected rows :" << affected_o;
//    qt_noop();

    /* INCREMENT / DECREMENT */
//    auto [affected_p, p] = m_dm.query()->from("torrents")
//            .whereEq("id", 277)
////            .increment("progress", 1);
//            .decrement("progress", 1, {
//                {"seeds", 6}
//            });
//    qDebug() << "SIXTEEN :" << p.executedQuery();
//    qDebug() << "affected rows :" << affected_p;
//    qt_noop();

    /* DELETEs */
//    auto [affected_q, q] = m_dm.query()->from("torrent_previewable_files")
//            .whereEq("id", 107)
//            .remove();
//    qDebug() << "SEVENTEEN :" << q.executedQuery();
//    qDebug() << "affected rows :" << affected_q;
//    qt_noop();

//    auto [affected_r, r] = m_dm.query()->from("torrents")
//            .join("torrent_previewable_files", "torrents.id", "=",
//                  "torrent_previewable_files.torrent_id")
//            .whereEq("torrents.id", 7)
//            .whereEq("torrent_previewable_files.id", 1006)
//            .remove();
//    qDebug() << "EIGHTEEN :" << r.executedQuery();
//    qDebug() << "affected rows :" << affected_r;
//    qt_noop();

    /* TRUNCATE */
//    auto [ok_s, s] = m_dm.query()->from("xxx")
//            .truncate();
//    qDebug() << "NINETEEN :" << s.executedQuery();
//    ok_s ? qDebug() << "truncate was successful"
//            : qDebug() << "truncate was unsuccessful";
    //    qt_noop();
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

//    // write prettified JSON to another file
//    std::ofstream o("pretty.json");
//    o << std::setw(4) << j << std::endl;

    qt_noop();
}

void TestOrm::standardPaths()
{
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
