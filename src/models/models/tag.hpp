#pragma once
#ifndef MODELS_TAG_HPP
#define MODELS_TAG_HPP

#include "orm/tiny/model.hpp"
//#include "orm/tiny/relations/pivot.hpp"

#include "models/tagged.hpp"
#include "models/tagproperty.hpp"
#include "models/torrent.hpp"

namespace Models
{

using Orm::Constants::NAME;
using Orm::Constants::NOTE;

using Orm::Tiny::Model;
//using Orm::Tiny::Relations::Pivot;

class Role;
class Torrent;

class Tag final : public Model<Tag, Torrent, TagProperty, Role, Tagged> // NOLINT(bugprone-exception-escape, misc-no-recursion)
//class Tag final : public Model<Tag, Torrent, TagProperty, Role, Pivot>
{
    friend Model;
    using Model::Model;

public:
    /*! Get torrents that belong to the tag. */
    std::unique_ptr<BelongsToMany<Tag, Torrent, Pivot>>
    torrents()
    {
        // Ownership of a unique_ptr()
        auto relation = belongsToMany<Torrent, Pivot>();

        relation/*->as("tagged")*/
                ->withPivot("active")
                .withTimestamps();

        return relation;
    }

    /*! Get torrents that belong to the tag. */
    std::unique_ptr<BelongsToMany<Tag, Torrent, Pivot>>
    torrents_WithoutPivotAttributes()
    {
        // Ownership of a unique_ptr()
        return belongsToMany<Torrent, Pivot>();
    }

    /*! Get a tag property associated with the tag. */
    std::unique_ptr<HasOne<Tag, TagProperty>>
    tagProperty()
    {
        return hasOne<TagProperty>();
    }

    /*! The roles that belong to the tag. */
    std::unique_ptr<BelongsToMany<Tag, Role>>
    roles()
    {
        // Ownership of a unique_ptr()
        auto relation = belongsToMany<Role>();

        relation->as("acl")
                .withPivot("active");

        return relation;
    }

private:
    /*! The table associated with the model. */
    QString u_table {"torrent_tags"};

    /*! Map of relation names to methods. */
    QHash<QString, RelationVisitor> u_relations {
        {"torrents",                        [](auto &v) { v(&Tag::torrents); }},
        {"torrents_WithoutPivotAttributes", [](auto &v) { v(&Tag::torrents_WithoutPivotAttributes); }},
        {"tagProperty",                     [](auto &v) { v(&Tag::tagProperty); }},
        {"roles",                           [](auto &v) { v(&Tag::roles); }},
    };

    /*! The relations to eager load on every query. */
    QList<QString> u_with {
        "tagProperty",
    };

    /*! The attributes that are mass assignable. */
    inline static const QStringList u_fillable { // NOLINT(cppcoreguidelines-interfaces-global-init)
        NAME,
        NOTE,
    };

    /*! All of the relationships to be touched. */
    QStringList u_touches {"torrents"};
};

} // namespace Models

#endif // MODELS_TAG_HPP
