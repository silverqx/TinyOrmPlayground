#include "concerns/logsquerycounters.hpp"

#include "connectionsservice.hpp"

namespace TinyPlay::Concerns
{

LogsQueryCounters::LogsQueryCounters(ConnectionsService &connectionsService)
    : m_connectionsService(connectionsService)
{}

void LogsQueryCounters::logQueryCounters(
        const QString &func,
        const std::optional<qint64> functionElapsed) const
{
    m_connectionsService.logQueryCounters(func, functionElapsed);
}

void LogsQueryCounters::resetAllQueryLogCounters() const
{
    m_connectionsService.resetAllQueryLogCounters();
}

} // namespace TinyPlay::Concerns
