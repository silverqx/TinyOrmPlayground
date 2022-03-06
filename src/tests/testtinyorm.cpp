#include "tests/testtinyorm.hpp"

#include <QDebug>
#include <QElapsedTimer>

#include <orm/db.hpp>
#include <orm/query/joinclause.hpp>

#include "models/torrent.hpp"
#include "models/torrenteager.hpp"

#include "macros.hpp"

// clazy:excludeall=unused-non-trivial-variable

using Models::FilePropertyProperty;
using Models::Role;
using Models::RoleUser;
using Models::Tag;
using Models::TagProperty;
using Models::Tagged;
using Models::Torrent;
using Models::TorrentEager;
using Models::TorrentPeer;
using Models::TorrentPeerEager_NoRelations;
using Models::TorrentPreviewableFile;
using Models::TorrentPreviewableFileProperty;
using Models::User;

using Orm::DB;
using Orm::One;
using Orm::Tiny::Exceptions::ModelNotFoundError;
using Orm::Tiny::Relations::Pivot;

namespace TinyPlay::Tests
{

    // NOLINTNEXTLINE(readability-function-cognitive-complexity, readability-function-size)
void TestTinyOrm::run() const
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
        auto torrents = Torrent::query()->get();

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
//    auto nn = std::move(var) | actions::sort(less {}, &QVariant::value<KeyType>)
//              | actions::unique;
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
//        auto torrentFile = TorrentPreviewableFile().query()
//                           ->where("id", "=", 1012)
//                           .first();

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
//        auto torrentFile = TorrentPreviewableFile().query()
//                           ->where("id", "=", 8)
//                           .first();

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
//        auto *fileProperty =
//                file->getRelation<TorrentPreviewableFileProperty, One>("fileProperty");

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
            auto files1 = torrent->getRelation<TorrentPreviewableFile>("torrentFiles");
            auto *file1 = files1.first();
            auto *fileProperty1 =
                    file1->getRelation<TorrentPreviewableFileProperty, One>(
                        "fileProperty");
            qDebug() << torrent->getAttribute("name").value<QString>();
            qDebug() << file1->getAttribute("filepath").value<QString>();
            qDebug() << fileProperty1->getAttribute("name").value<QString>();
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

//                auto fileProperty =
//                        file->getRelation<TorrentPreviewableFileProperty, One>(
//                            "fileProperty");

//                qDebug().nospace().noquote()
//                        << fileProperty->getAttribute("name").value<QString>()
//                        << "("
//                        << fileProperty->getAttribute("id").value<quint64>()
//                        << ")";
//                qt_noop();
//            }
//        }

        // lazy load
        {
            qDebug() << "\nlazy load, !!comment out 'with' relation to prevent eager "
                        "load";
            qDebug() << "\n↓↓↓ lazy load works, if an Executed query is logged under "
                        "this comment:";
            auto files =
                    torrent->getRelationValue<TorrentPreviewableFile>("torrentFiles");

            for (auto *file : files) {
                qDebug().nospace().noquote()
                        << "\nFile : "
                        << file->getAttribute("filepath").value<QString>()
                        << "(" << file->getAttribute("id").value<quint64>() << ")";

                auto *fileProperty =
                        file->getRelationValue<TorrentPreviewableFileProperty, One>(
                            "fileProperty");

                if (fileProperty != nullptr)
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
        qDebug() << updatedAt << "before :"
                 << torrent->getAttribute(updatedAt).toDateTime();

        torrent->save();
        qDebug() << "progress after :" << torrent->getAttribute("progress");
        qDebug() << updatedAt << "after :"
                 << torrent->getAttribute(updatedAt).toDateTime();
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
        qDebug() << updatedAt << "before :"
                 << torrent->getAttribute(updatedAt).toDateTime();

        torrent->save();
        qDebug() << "progress after :" << torrent->getAttribute("progress");
        qDebug() << updatedAt << "after :"
                 << torrent->getAttribute(updatedAt).toDateTime();
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

        const auto &updatedAtFilePropertyProperty =
                filePropertyProperty->getUpdatedAtColumn();
        qDebug() << "FilePropertyProperty" << updatedAtFilePropertyProperty << "before :"
                 << filePropertyProperty
                    ->getAttribute(updatedAtFilePropertyProperty).toDateTime();

        auto fileProperty = TorrentPreviewableFileProperty::find(3);
        const auto &updatedAtFileProperty = fileProperty->getUpdatedAtColumn();
        qDebug() << "TorrentPreviewableFileProperty"
                 << updatedAtFileProperty << "before :"
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
                 << filePropertyProperty1
                    ->getAttribute(updatedAtFilePropertyProperty).toDateTime();

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
        const auto &createdAt = TorrentPreviewableFile::getCreatedAtColumn();
        qDebug() << createdAt << "after :"
                 << torrentFile.getAttribute(createdAt).toDateTime();
        const auto &updatedAt = TorrentPreviewableFile::getUpdatedAtColumn();
        qDebug() << updatedAt << "after :"
                 << torrentFile.getAttribute(updatedAt).toDateTime();
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
        qDebug() << "size after increment:"
                 << torrent->getAttribute("size").value<uint>();
        qDebug() << updatedAt << "after increment:"
                 << torrent->getAttribute(updatedAt).toDateTime();
        qDebug() << "progress after increment:"
                 << (*torrent)["progress"]->value<uint>();

        Torrent::whereEq("id", 1)->decrement("size", 2, {{"progress", 100}});

        torrent = Torrent::find(1);
        qDebug() << "size after decrement:"
                 << torrent->getAttribute("size").value<uint>();
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
        auto *peerOriginal =
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
        auto *peerRefreshed =
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
                 << "color :" << tagProperty->getAttribute("color").value<QString>()
                 << ";"
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
        auto *verifyTorrent5 = file.getRelation<Torrent, One>("torrent");

        /* Have to unset current relationship, this is clearly visible in the Eqloquent's
           associate implementation. */
        fileRef = file.torrent()->associate(2);

//        fileRef.save();
        file.save();

        [[maybe_unused]]
        auto *verifyTorrent4 = file.getRelation<Torrent, One>("torrent");

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
        auto *verifyTorrent = file.getRelation<Torrent, One>("torrent");

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
                auto useTimestamps =
                        role->getRelation<RoleUser, Orm::One>("subscription")
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
                     {"note", QVariant(QMetaType(QMetaType::QString))}},
#else
                     {"note", QVariant(QVariant::String)}},
#endif
                    {{"active", true}});

        qt_noop();
    }

    /* Column expressions in TinyBuilder::select() methods */
    {
        qInfo() << "\n\nColumn expressions in TinyBuilder::select() methods\n---";

        auto r1 = Torrent::query()->get({"id", "name"});
        auto r2 = Torrent::query()->get({"id", DB::raw("name")});
        auto r3 = Torrent::query()->value("name");
        auto r4 = Torrent::query()->value(DB::raw("name"));
        auto r5 = Torrent::query()->find(1, {"id", "name"});
        auto r6 = Torrent::query()->find(1, {"id", DB::raw("name")});
        auto r7 = Torrent::query()->findOrNew(1, {"id", "name"});
        auto r8 = Torrent::query()->findOrNew(1, {"id", DB::raw("name")});
        auto r9 = Torrent::query()->findOrFail(1, {"id", "name"});
        auto r10 = Torrent::query()->findOrFail(1, {"id", DB::raw("name")});
        auto r11 = Torrent::query()->whereEq("id", 1).first({"id", "name"});
        auto r12 = Torrent::query()->whereEq("id", 1).first({"id", DB::raw("name")});
        auto r13 = Torrent::query()->whereEq("id", 1).firstOrFail({"id", "name"});
        auto r14 = Torrent::query()
                   ->whereEq("id", 1).firstOrFail({"id", DB::raw("name")});
        auto r15 = Torrent::query()->firstWhere("id", "=", 1);
        auto r16 = Torrent::query()->firstWhere(DB::raw("id"), "=", 1);

        auto r17 = Torrent::query()->select("id").whereEq("id", 1).get();
        auto r18 = Torrent::query()->select(DB::raw("id")).whereEq("id", 1).get();
        auto r19 = Torrent::query()->select({"id", "name"}).whereEq("id", 1).get();
        auto r20 = Torrent::query()
                   ->select({"id", DB::raw("name")}).whereEq("id", 1).get();
        auto r21 = Torrent::query()->addSelect("id").whereEq("id", 1).get();
        auto r22 = Torrent::query()->addSelect(DB::raw("id")).whereEq("id", 1).get();
        auto r23 = Torrent::query()->addSelect({"id", "name"}).whereEq("id", 1).get();
        auto r24 = Torrent::query()
                   ->addSelect({"id", DB::raw("name")}).whereEq("id", 1).get();

        qt_noop();
    }

    /* Column expressions in Model::select() methods */
    {
        qInfo() << "\n\nColumn expressions in Model::select() methods\n---";

        auto r1 = Torrent::all({"id", "name"});
        auto r2 = Torrent::all({"id", DB::raw("name")});
        auto r3 = Torrent::value("name");
        auto r4 = Torrent::value(DB::raw("name"));
        auto r5 = Torrent::find(1, {"id", "name"});
        auto r6 = Torrent::find(1, {"id", DB::raw("name")});
        auto r7 = Torrent::findOrNew(1, {"id", "name"});
        auto r8 = Torrent::findOrNew(1, {"id", DB::raw("name")});
        auto r9 = Torrent::findOrFail(1, {"id", "name"});
        auto r10 = Torrent::findOrFail(1, {"id", DB::raw("name")});
        auto r11 = Torrent::whereEq("id", 1)->first({"id", "name"});
        auto r12 = Torrent::whereEq("id", 1)->first({"id", DB::raw("name")});
        auto r13 = Torrent::whereEq("id", 1)->firstOrFail({"id", "name"});
        auto r14 = Torrent::whereEq("id", 1)->firstOrFail({"id", DB::raw("name")});
        auto r15 = Torrent::firstWhere("id", "=", 1);
        auto r16 = Torrent::firstWhere(DB::raw("id"), "=", 1);

        auto r17 = Torrent::select("id")->whereEq("id", 1).get();
        auto r18 = Torrent::select(DB::raw("id"))->whereEq("id", 1).get();
        auto r19 = Torrent::select({"id", "name"})->whereEq("id", 1).get();
        auto r20 = Torrent::select({"id", DB::raw("name")})->whereEq("id", 1).get();
        auto r21 = Torrent::addSelect("id")->whereEq("id", 1).get();
        auto r22 = Torrent::addSelect(DB::raw("id"))->whereEq("id", 1).get();
        auto r23 = Torrent::addSelect({"id", "name"})->whereEq("id", 1).get();
        auto r24 = Torrent::addSelect({"id", DB::raw("name")})->whereEq("id", 1).get();

        qt_noop();
    }

    /* Column expressions in Relation::select() methods */
    {
        qInfo() << "\n\nColumn expressions in Relation::select() methods\n---";

        auto r1 = Torrent::find(1)->torrentFiles()->get({"id", "filepath"});
        auto r2 = Torrent::find(1)->torrentFiles()->get({"id", DB::raw("filepath")});
        auto r3 = Torrent::find(1)->torrentFiles()->value("filepath");
        auto r4 = Torrent::find(1)->torrentFiles()->value(DB::raw("filepath"));
        auto r5 = Torrent::find(1)->torrentFiles()->find(1, {"id", "filepath"});
        auto r6 = Torrent::find(1)->torrentFiles()->find(1, {"id", DB::raw("filepath")});
        auto r7 = Torrent::find(1)->torrentFiles()->findOrNew(1, {"id", "filepath"});
        auto r8 = Torrent::find(1)->torrentFiles()
                  ->findOrNew(1, {"id", DB::raw("filepath")});
        auto r9 = Torrent::find(1)->torrentFiles()->findOrFail(1, {"id", "filepath"});
        auto r10 = Torrent::find(1)->torrentFiles()
                   ->findOrFail(1, {"id", DB::raw("filepath")});
        auto r11 = Torrent::find(1)->torrentFiles()
                   ->whereEq("id", 1).first({"id", "filepath"});
        auto r12 = Torrent::find(1)->torrentFiles()
                   ->whereEq("id", 1).first({"id", DB::raw("filepath")});
        auto r13 = Torrent::find(1)->torrentFiles()
                   ->whereEq("id", 1).firstOrFail({"id", "filepath"});
        auto r14 = Torrent::find(1)->torrentFiles()
                   ->whereEq("id", 1).firstOrFail({"id", DB::raw("filepath")});
        auto r15 = Torrent::find(1)->torrentFiles()->firstWhere("id", "=", 1);
        auto r16 = Torrent::find(1)->torrentFiles()->firstWhere(DB::raw("id"), "=", 1);

        auto r17 = Torrent::find(1)->torrentFiles()->select("id").whereEq("id", 1).get();
        auto r18 = Torrent::find(1)->torrentFiles()
                   ->select(DB::raw("id")).whereEq("id", 1).get();
        auto r19 = Torrent::find(1)->torrentFiles()
                   ->select({"id", "filepath"}).whereEq("id", 1).get();
        auto r20 = Torrent::find(1)->torrentFiles()
                   ->select({"id", DB::raw("filepath")}).whereEq("id", 1).get();
        auto r21 = Torrent::find(1)->torrentFiles()
                   ->addSelect("id").whereEq("id", 1).get();
        auto r22 = Torrent::find(1)->torrentFiles()
                   ->addSelect(DB::raw("id")).whereEq("id", 1).get();
        auto r23 = Torrent::find(1)->torrentFiles()
                   ->addSelect({"id", "filepath"}).whereEq("id", 1).get();
        auto r24 = Torrent::find(1)->torrentFiles()
                   ->addSelect({"id", DB::raw("filepath")}).whereEq("id", 1).get();

        qt_noop();
    }

    /* Column expressions in HasMany::findMany() methods */
    {
        qInfo() << "\n\nColumn expressions in HasMany::findMany() methods\n---";

        auto r1 = Torrent::find(2)->torrentFiles()
                  ->findMany({2, 3}, {"id", "filepath"});
        auto r2 = Torrent::find(2)->torrentFiles()
                  ->findMany({2, 3}, {"id", DB::raw("filepath")});

        qt_noop();
    }

    /* Column expressions in BelongsToMany::select() methods */
    {
        qInfo() << "\n\nColumn expressions in BelongsToMany::select() methods\n---";

        auto r1 = Torrent::find(2)->tags()->get({"id", "name"});
        auto r2 = Torrent::find(2)->tags()->get({"id", DB::raw("name")});
        auto r5 = Torrent::find(2)->tags()->find(1, {"id", "name"});
        auto r6 = Torrent::find(2)->tags()->find(1, {"id", DB::raw("name")});
        auto r7 = Torrent::find(2)->tags()->findOrNew(1, {"id", "name"});
        auto r8 = Torrent::find(2)->tags()->findOrNew(1, {"id", DB::raw("name")});
        auto r9 = Torrent::find(2)->tags()->findOrFail(1, {"id", "name"});
        auto r10 = Torrent::find(2)->tags()->findOrFail(1, {"id", DB::raw("name")});
        auto r11 = Torrent::find(2)->tags()->whereEq("id", 1).first({"id", "name"});
        auto r12 = Torrent::find(2)->tags()
                   ->whereEq("id", 1).first({"id", DB::raw("name")});
        auto r13 = Torrent::find(2)->tags()->whereEq("id", 1).firstOrFail({"id", "name"});
        auto r14 = Torrent::find(2)->tags()
                   ->whereEq("id", 1).firstOrFail({"id", DB::raw("name")});
        auto r15 = Torrent::find(2)->tags()->firstWhere("id", "=", 1);
        auto r16 = Torrent::find(2)->tags()->firstWhere(DB::raw("id"), "=", 1);

        auto r17 = Torrent::find(2)->tags()->findMany({1, 2}, {"id", "name"});
        auto r18 = Torrent::find(2)->tags()->findMany({1, 2}, {"id", DB::raw("name")});

        qt_noop();
    }

    /* Model::leftJoinSub() with join on callback */
    {
        qInfo() << "\n\nModel::leftJoinSub() with join on callback\n---";

        auto torrents = Torrent::leftJoinSub([](auto &query)
        {
            query.from("torrent_previewable_files")
                    .select({"id as files_id", "torrent_id", "filepath",
                             "size as files_size"})
                    .where("size", "<", 2050);
        }, "files", [](auto &join)
        {
            join.on("torrents.id", "=", "files.torrent_id");
        })
                ->where("progress", "<", 500)
                .get();

        for (auto &t : torrents)
            qDebug() << "id :" << t.getAttribute("id").value<quint64>() << ";"
                     << "progress :" << t.getAttribute("progress").value<int>() << ";"
                     << "filepath :" << t.getAttribute("filepath").value<QString>() << ";"
                     << "files_size :" << t.getAttribute("files_size").value<qint64>();

        qt_noop();
    }

    /* Model::pluck() - simple overload */
    {
        qInfo() << "\n\nModel::pluck() - simple overload\n---";

        auto names = Torrent::pluck("name");

        for (auto &&name : names)
            qDebug().noquote() << name.value<QString>();

        qt_noop();
    }

    /* Model::pluck() - keyed overload */
    {
        qInfo() << "\n\nModel::pluck() - keyed overload\n---";

        auto result = Torrent::pluck<QString>("hash", "name");

        for (auto &&[name, hash] : result)
            qDebug().noquote() << name << ":" << hash.value<QString>();

        qt_noop();
    }

    /* Model::where() - value subquery - lambda expression */
    {
        qInfo() << "\n\nModel::where() - value subquery - lambda expression\n---";

        auto torrents = Torrent::whereEq("size", 12)
                        ->orWhere("size", ">", [](auto &q)
        {
            q.from("torrents", "t").selectRaw("avg(t.size)");
        }).get();

        qDebug() << "#" << "|" << "name" << "|" << "size";
        qDebug() << "---------------";
        for (auto &torrent : torrents)
            qDebug().noquote()
                    << torrent["id"]->value<quint64>() << "|"
                    << torrent["name"]->value<QString>() << "|"
                    << torrent["size"]->value<quint64>();

        qt_noop();
    }

    /* Model::where() - value subquery - QueryBuilder & */
    {
        qInfo() << "\n\nModel::where() - value subquery - QueryBuilder &\n---";

        auto torrents = Torrent::whereEq("size", 12)
                        ->orWhere("size", ">",
                                  DB::tableAs("torrents", "t")->selectRaw("avg(t.size)"))
                        .get();

        qDebug() << "#" << "|" << "name" << "|" << "size";
        qDebug() << "---------------";
        for (auto &torrent : torrents)
            qDebug().noquote()
                    << torrent["id"]->value<quint64>() << "|"
                    << torrent["name"]->value<QString>() << "|"
                    << torrent["size"]->value<quint64>();

        qt_noop();
    }

    /* Model::where() - column subquery - lambda expression */
    {
        qInfo() << "\n\nModel::where() - column subquery - lambda expression\n---";

        auto torrents = Torrent::whereEq([](auto &q)
        {
            q.from("torrent_previewable_files")
                    .select("file_index")
                    .whereColumnEq("torrent_id", "torrents.id")
                    .orderByDesc("file_index")
                    .limit(1);
        }, 0)
                ->orderBy("id")
                .get();

        qDebug() << "#" << "|" << "name";
        qDebug() << "--------";
        for (auto &torrent : torrents)
            qDebug().noquote() << torrent["id"]->value<quint64>() << "|"
                               << torrent["name"]->value<QString>();

        qt_noop();
    }

    logQueryCounters(timer.elapsed());
}

} // namespace TinyPlay::Tests
