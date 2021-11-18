#include "configurationsservice.hpp"

#include "configuration.hpp"
#include "support/globals.hpp"
#include "support/utils.hpp"

using TinyPlay::Support::Utils;

namespace TinyPlay
{

ConfigurationsService::ConfigurationsService(const Configuration &configuration)
    : m_config(configuration)
{}

ConfigurationsService::OrmConfigurationsType
ConfigurationsService::computeConfigurationsToAdd(const QString &connection)
{
    // Connections in threads are disabled
    if (!m_config.ConnectionsInThreads)
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

ConfigurationsService::OrmConfigurationsType
ConfigurationsService::configurationsWhenSingleThread() const
{
    OrmConfigurationsType configurations;

    auto itConfig = m_config.Configurations.constBegin();
    while (itConfig != m_config.Configurations.constEnd()) {
        const auto &key = itConfig.key();

        if (key != "mysql_mainthread")
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

    for (const auto &connection : mappedConnections)
        configurationsForWorkerThread.insert(connection,
                                             m_config.Configurations[connection]);

    return configurationsForWorkerThread;
}

QStringList ConfigurationsService::getMappedConnections(const QString &connection) const
{
    if (m_config.ConnectionsMap.contains(connection))
        return m_config.ConnectionsMap.find(connection)->second;

    return {connection};
}

} // namespace TinyPlay
