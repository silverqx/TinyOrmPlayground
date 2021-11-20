#include "concerns/logsquerycounters.hpp"

#include <orm/utils/type.hpp>

#include "services/querycountersservice.hpp"

namespace TinyPlay::Concerns
{

LogsQueryCounters::LogsQueryCounters(QueryCountersService &queryCountersService)
    : m_queryCountersService(queryCountersService)
{}

void LogsQueryCounters::logQueryCounters(
        const QString &func,
        const std::optional<qint64> functionElapsed) const
{
    m_queryCountersService.logQueryCounters(func, functionElapsed);
}

void LogsQueryCounters::logQueryCounters(
        const std::optional<qint64> functionElapsed) const
{
    logQueryCounters(Orm::Utils::Type::classPureBasename(*this), functionElapsed);
}

void LogsQueryCounters::resetAllQueryLogCounters() const
{
    m_queryCountersService.resetAllQueryLogCounters();
}

} // namespace TinyPlay::Concerns
