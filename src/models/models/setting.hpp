#ifndef SETTING_H
#define SETTING_H

#include "orm/tiny/basemodel.hpp"

class Setting final : public Orm::Tiny::BaseModel<Setting>
{
public:
    friend class BaseModel;

    explicit Setting(const QVector<Orm::AttributeItem> &attributes = {});

private:
    /*! The table associated with the model. */
    QString u_table {"settings"};

    /*! Indicates if the IDs are auto-incrementing. */
    bool u_incrementing = false;
};

#endif // SETTING_H
