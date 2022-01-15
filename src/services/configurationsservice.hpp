#pragma once
#ifndef TINYPLAY_SERVICES_CONFIGURATIONSSERVICE_HPP
#define TINYPLAY_SERVICES_CONFIGURATIONSSERVICE_HPP

#include <orm/support/databaseconfiguration.hpp>

#include "services/service.hpp"

namespace TinyPlay::Services
{
    /*! Service class for database connection configurations. */
    class ConfigurationsService : public Service
    {
        Q_DISABLE_COPY(ConfigurationsService)

        /*! Type used for the Database Configuration. */
        using OrmConfiguration = Orm::Support::DatabaseConfiguration;
        /*! Type used for Database Connections map. */
        using OrmConfigurationsType = OrmConfiguration::ConfigurationsType;

    public:
        /*! Default constructor. */
        explicit ConfigurationsService(Configuration &config);

        /*! Compute connections configurations.
            A common method, the single/(multi main/worker) thread version will be
            called based on whether the connection argument was passed. */
        OrmConfigurationsType
        computeConfigurationsToAdd(const QString &connection = "");

        /*! Obtain default connection name. */
        QString getDefaultConnectionName() const;

    private:
        /*! Compute connections configurations to use when threading is disabled. */
        OrmConfigurationsType configurationsWhenSingleThread() const;
        /*! Compute connections configurations to use when threading is enabled. */
        OrmConfigurationsType configsForMainThrdWhenMultiThrd() const;
        /*! Compute connections configurations to use in a worker thread. */
        OrmConfigurationsType
        configsForWorkerThrdWhenMultiThrd(const QString &connection) const;
    };

} // namespace TinyPlay::Services

#endif // TINYPLAY_SERVICES_CONFIGURATIONSSERVICE_HPP
