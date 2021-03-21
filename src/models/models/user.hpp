#ifndef USER_H
#define USER_H

#include <orm/tiny/basemodel.hpp>

#include "models/role.hpp"

using Orm::Tiny::BaseModel;
using Orm::Tiny::Relations::BelongsToMany;
using Orm::Tiny::Relations::Pivot;
using Orm::Tiny::Relations::Relation;

class User final : public BaseModel<User, Role, Pivot>
{
    friend BaseModel;
    using BaseModel::BaseModel;

public:
    /*! The roles that belong to the user. */
    std::unique_ptr<Relation<User, Role>>
    roles()
    {
        // Ownership of a unique_ptr()
        auto relation = belongsToMany<Role, RoleUser>();
        dynamic_cast<BelongsToMany<User, Role, RoleUser> &>(*relation)
                .as("subscription")
                .withPivot("active");

        return relation;
    }

private:
    /*! The visitor to obtain a type for Related template parameter. */
    void relationVisitor(const QString &relation)
    {
        if (relation == "roles")
            relationVisited<Role>();
        else if (relation == "pivot") // Pivot
            relationVisited<Pivot>();
    }

    /*! Map of relation names to methods. */
    QHash<QString, std::any> u_relations {
        {"roles", &User::roles},
    };

#ifdef PROJECT_TINYORM_TEST
    /*! The connection name for the model. */
    QString u_connection {"tinyorm_mysql_tests"};
#endif
};

#endif // USER_H
