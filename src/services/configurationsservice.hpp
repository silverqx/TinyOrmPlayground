#pragma once
#ifndef TINYPLAY_SERVICES_CONFIGURATIONSSERVICE_HPP
#define TINYPLAY_SERVICES_CONFIGURATIONSSERVICE_HPP

#include <orm/support/databaseconfiguration.hpp>

namespace TinyPlay
{

    class Configuration;

namespace Services
{
    /*! Service class for database connection configurations. */
    class ConfigurationsService
    {
        Q_DISABLE_COPY(ConfigurationsService)

        /*! Type for the Database Configuration. */
        using OrmConfiguration = Orm::Support::DatabaseConfiguration;
        /*! Type used for Database Connections map. */
        using OrmConfigurationsType = OrmConfiguration::ConfigurationsType;

    public:
        /*! Default constructor. */
        explicit ConfigurationsService(const Configuration &configuration);

        /*! Compute connections configurations.
            A common method, the single/(multi main/worker) thread version will be
            called based on whether the connection argument was passed. */
        OrmConfigurationsType
        computeConfigurationsToAdd(const QString &connection = "");

    private:
        /*! Compute connections configurations to use when threading is disabled. */
        OrmConfigurationsType configurationsWhenSingleThread() const;
        /*! Compute connections configurations to use when threading is enabled. */
        OrmConfigurationsType configsForMainThrdWhenMultiThrd() const;
        /*! Compute connections configurations to use in a worker thread. */
        OrmConfigurationsType
        configsForWorkerThrdWhenMultiThrd(const QString &connection) const;

        // CUR temporary because of getMappedConnections(), remove silverqx
        /*! Obtain mapped connections for a given connection. */
        QStringList getMappedConnections(const QString &connection) const;

        /*! Configuration for TinyOrmPlayground. */
        const Configuration &m_config;
    };

} // namespace Services
} // namespace TinyPlay

#endif // TINYPLAY_SERVICES_CONFIGURATIONSSERVICE_HPP
