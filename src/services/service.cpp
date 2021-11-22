#include "services/service.hpp"

#include "configuration.hpp"

namespace TinyPlay::Services
{

Service::Service(Configuration &config)
    : m_config(config)
{}

QStringList Service::getMappedConnections(const QString &connection) const
{
    if (m_config.ConnectionsMap.contains(connection))
        return m_config.ConnectionsMap.find(connection)->second;

    return {connection};
}

} // namespace TinyPlay::Services
