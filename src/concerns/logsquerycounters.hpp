#pragma once
#ifndef TINYPLAY_CONCERNS_LOGSQUERYCOUNTERS_HPP
#define TINYPLAY_CONCERNS_LOGSQUERYCOUNTERS_HPP

#include <QtGlobal>

#include <optional>

namespace TinyPlay
{

    class ConnectionsService;

namespace Concerns
{

    /*! Class used to log and reset query counters. */
    class LogsQueryCounters
    {
        Q_DISABLE_COPY(LogsQueryCounters)

    public:
        /*! Default constructor. */
        explicit LogsQueryCounters(ConnectionsService &connectionsService);

        /*! Log queries execution time counter and Query statements counter. */
        void logQueryCounters(
                const QString &func,
                std::optional<qint64> functionElapsed = std::nullopt) const;
        /*! Reset all counters on counted connections. */
        void resetAllQueryLogCounters() const;

    private:
        /*! Database connections query counters service. */
        ConnectionsService &m_connectionsService;
    };

} // namespace Concerns
} // namespace TinyPlay

#endif // TINYPLAY_CONCERNS_LOGSQUERYCOUNTERS_HPP
