#pragma once
#ifndef TINYPLAY_SERVICES_SERVICE_HPP
#define TINYPLAY_SERVICES_SERVICE_HPP

#include <QStringList>

namespace TinyPlay
{

    class Configuration;

namespace Services
{
    /*! Base class for TinyPlay services. */
    class Service
    {
        Q_DISABLE_COPY(Service)

    public:
        /*! Default constructor. */
        explicit Service(Configuration &config);

        /*! Obtain mapped connections for a given connection. */
        QStringList getMappedConnections(const QString &connection) const;

    protected:
        /*! Configuration for TinyOrmPlayground. */
        Configuration &m_config;
    };

} // namespace Services
} // namespace TinyPlay

#endif // TINYPLAY_SERVICES_SERVICE_HPP
