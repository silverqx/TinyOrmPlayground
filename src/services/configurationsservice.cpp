#include "services/configurationsservice.hpp"

#include "configuration.hpp"
#include "support/globals.hpp"
#include "support/utils.hpp"

using TinyPlay::Support::Utils;

namespace TinyPlay::Services
{

/* public */

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

/* private */

ConfigurationsService::OrmConfigurationsType
ConfigurationsService::configurationsWhenSingleThread() const
{
    OrmConfigurationsType configurations;
    configurations.reserve(m_config.Configurations.size());

    for (const auto &[name, config] : m_config.Configurations)
        if (name != Mysql_MainThread)
            configurations.emplace(name, config);

    return configurations;
}

ConfigurationsService::OrmConfigurationsType
ConfigurationsService::configsForMainThrdWhenMultiThrd() const
{
    OrmConfigurationsType configurationsForMainThread;
    configurationsForMainThread.reserve(m_config.Configurations.size());

    for (const auto &[name, config] : m_config.Configurations)
        if (!m_config.ConnectionsToRunInThread.contains(name))
            configurationsForMainThread.emplace(name, config);

    return configurationsForMainThread;
}


ConfigurationsService::OrmConfigurationsType
ConfigurationsService::configsForWorkerThrdWhenMultiThrd(const QString &connection) const
{
    const QStringList mappedConnections = getMappedConnections(connection);

    OrmConfigurationsType configurationsForWorkerThread;
    configurationsForWorkerThread.reserve(
                static_cast<OrmConfigurationsType::size_type>(mappedConnections.size()));

    for (const auto &connection_ : mappedConnections)
        configurationsForWorkerThread.emplace(connection_,
                                              m_config.Configurations.at(connection_));

    return configurationsForWorkerThread;
}

} // namespace TinyPlay::Services
