#ifndef TORRENT_H
#define TORRENT_H

#include "orm/tiny/model.hpp"

#include "models/forwards.hpp"

#include "models/tag.hpp"
#include "models/tagged.hpp"
#include "models/torrentpeer.hpp"
#include "models/torrentpreviewablefile.hpp"

//using Orm::AttributeItem;
using Orm::Tiny::Model;
using Orm::Tiny::Relations::BelongsToMany;
using Orm::Tiny::Relations::HasOne;
using Orm::Tiny::Relations::HasMany;
using Orm::Tiny::Relations::Pivot;
using Orm::Tiny::Relations::Relation;
using Orm::WithItem;

/* This class serves as a showcase, so all possible features are defined / used. */

class Torrent final :
        public Model<Torrent, TorrentPreviewableFile, TorrentPeer, Tag, Pivot>
//        public Model<Torrent, TorrentPreviewableFile, TorrentPeer, Tag, Tagged>
{
    friend Model;
    using Model::Model;

public:
    /*! The "type" of the primary key ID. */
    using KeyType = quint64;

//    explicit Torrent(const QVector<AttributeItem> &attributes = {});

    /*! Get the previewable files associated with the torrent. */
    std::unique_ptr<HasMany<Torrent, TorrentPreviewableFile>>
    torrentFiles()
    {
        return hasMany<TorrentPreviewableFile>();
//        return hasMany<TorrentPreviewableFile>("torrent_id", "id");
    }

    /*! Get the torrent peer associated with the torrent. */
    std::unique_ptr<HasOne<Torrent, TorrentPeer>>
    torrentPeer()
    {
        return hasOne<TorrentPeer>();
//        return hasOne<TorrentPeer>("torrent_id", "id");

        // Default Model example
//        auto relation = hasOne<TorrentPeer>();
//        relation->withDefault();
//        relation->withDefault({{"seeds", 0}, {"total_seeds", -1}});
        // This callback variant is not yet implemented 😟
//        relation->withDefault([](Torrent &torrent,
//                              const TorrentPreviewableFile &/*torrentFile*/)
//        {
//            torrent["name"] = "default_model_name";
//        });
//        return relation;
    }

    /*! Get tags that belong to the torrent. */
    std::unique_ptr<BelongsToMany<Torrent, Tag, Tagged>>
    tags()
    {
        // Ownership of a unique_ptr()
//        auto relation = belongsToMany<Tag>();
//        relation->as("tagged")
//                .withPivot("active")
//                .withTimestamps();

        // Ownership of a unique_ptr()
        // Custom 'Tagged' pivot model ✨
        auto relation = belongsToMany<Tag, Tagged>();
        relation->as("tagged")
                .withPivot("active")
                .withTimestamps(/*"created_at_custom", "updated_at_custom"*/);

        return relation;
//        return belongsToMany<Tag>("tag_torrent", "torrent_id", "tag_id", "id", "id",
//                                  "tags");
    }

private:
    /*! The name of the "created at" column. */
    inline static const QString CREATED_AT = QStringLiteral("created_at");
    /*! The name of the "updated at" column. */
    inline static const QString UPDATED_AT = QStringLiteral("updated_at");

    /*! The table associated with the model. */
    QString u_table {"torrents"};

    /*! Indicates if the model's ID is auto-incrementing. */
//    bool u_incrementing = true;
    /*! The primary key associated with the table. */
//    QString u_primaryKey {"id"};

    /*! Map of relation names to methods. */
    QHash<QString, RelationVisitor> u_relations {
        {"torrentFiles", [](auto &v) { v(&Torrent::torrentFiles); }},
        {"torrentPeer",  [](auto &v) { v(&Torrent::torrentPeer); }},
        {"tags",         [](auto &v) { v(&Torrent::tags); }},
    };

    /*! The relations to eager load on every query. */
    QVector<WithItem> u_with {
//        {"torrentFiles"},
//        {"torrentPeer"},
//        {"torrentFiles.fileProperty"},
//        {"tags"},
    };

#ifdef PROJECT_TINYORM_PLAYGROUND
    /*! The connection name for the model. */
    QString u_connection {"mysql_alt"};
#endif

    /*! The connection name for the model. */
//    QString u_connection {"sqlite"};

    /*! The model's default values for attributes. */
//    inline static const QVector<AttributeItem> u_attributes {
//        {"size",     0},
//        {"progress", 0},
//        {"added_on", QDateTime::fromString("2021-04-01 15:10:10", Qt::ISODate)},
//    };

    /*! The attributes that are mass assignable. */
    inline static QStringList u_fillable {
        "id",
        "name",
        "size",
        "progress",
        "added_on",
        "hash",
        "note",
        "updated_at",
    };

    /*! The attributes that aren't mass assignable. */
//    inline static QStringList u_guarded {
//        "password",
//    };

    /*! Indicates if the model should be timestamped. */
//    bool u_timestamps = true;

    /*! The storage format of the model's date columns. */
//    inline static QString u_dateFormat {"yyyy-MM-dd HH:mm:ss"};

    /*! The attributes that should be mutated to dates. @deprecated */
    inline static QStringList u_dates {"added_on"};

    /*! All of the relationships to be touched. */
//    QStringList u_touches {"tags"};
//    QStringList u_touches {"relation_name"};
};

// TODO finish this, move to base class and test eg in qvector, qhash, etc silverqx
//QDebug operator<<(QDebug debug, const Torrent &c);

#endif // TORRENT_H
