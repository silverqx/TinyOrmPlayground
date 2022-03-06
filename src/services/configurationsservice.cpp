#include "services/configurationsservice.hpp"

#include "configuration.hpp"
#include "support/globals.hpp"
#include "support/utils.hpp"

using TinyPlay::Support::Utils;

namespace TinyPlay::Services
{

ConfigurationsService::ConfigurationsService(Configuration &config)
    : Service(config)
{}

ConfigurationsService::OrmConfigurationsType
ConfigurationsService::computeConfigurationsToAdd(const QString &connection)
{
    // Connections in threads are disabled
    if constexpr (!Configuration::ConnectionsInThreads)
        return configurationsWhenSingleThread();

    // All below - Connections in threads are enabled

    Utils::throwIfNonEmptyConn(connection);

    // Called from main thread
    if (!Support::g_inThread)
        return configsForMainThrdWhenMultiThrd();

    Utils::throwIfEmptyConn(connection);

    // Otherwise called from a non-main/worker thread
    return configsForWorkerThrdWhenMultiThrd(connection);
}

QString ConfigurationsService::getDefaultConnectionName() const
{
    if constexpr (Configuration::ConnectionsInThreads)
            return Mysql_MainThread;

    return Mysql;
}

ConfigurationsService::OrmConfigurationsType
ConfigurationsService::configurationsWhenSingleThread() const
{
    OrmConfigurationsType configurations;

    auto itConfig = m_config.Configurations.constBegin();
    while (itConfig != m_config.Configurations.constEnd()) {
        const auto &key = itConfig.key();

        if (key != Mysql_MainThread)
            configurations.insert(key, itConfig.value());

        ++itConfig;
    }

    return configurations;
}

ConfigurationsService::OrmConfigurationsType
ConfigurationsService::configsForMainThrdWhenMultiThrd() const
{
    OrmConfigurationsType configurationsForMainThread;
    configurationsForMainThread.reserve(m_config.Configurations.size());

    auto itConfig = m_config.Configurations.constBegin();
    while (itConfig != m_config.Configurations.constEnd()) {
        const auto &key = itConfig.key();
        const auto &value = itConfig.value();

        if (!m_config.ConnectionsToRunInThread.contains(key))
            configurationsForMainThread.insert(key, value);

        ++itConfig;
    }

    return configurationsForMainThread;
}


ConfigurationsService::OrmConfigurationsType
ConfigurationsService::configsForWorkerThrdWhenMultiThrd(const QString &connection) const
{
    const QStringList mappedConnections = getMappedConnections(connection);

    OrmConfigurationsType configurationsForWorkerThread;
    configurationsForWorkerThread.reserve(mappedConnections.size());

    for (const auto &connection_ : mappedConnections)
        configurationsForWorkerThread.insert(connection_,
                                             m_config.Configurations[connection_]);

    return configurationsForWorkerThread;
}

} // namespace TinyPlay::Services
