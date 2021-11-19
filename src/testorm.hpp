#pragma once
#ifndef TINYPLAY_TESTORM_HPP
#define TINYPLAY_TESTORM_HPP

#include <orm/databasemanager.hpp>

#include "configuration.hpp"
#include "services/configurationsservice.hpp"
#include "services/querycountersservice.hpp"

namespace TinyPlay
{

    /*! Main playground class to test TinyORM library. */
    class TestOrm
    {
        Q_DISABLE_COPY(TestOrm)

    public:
//        using KeyType = quint64;

        /*! Default constructor. */
        inline TestOrm() = default;

        /*! Create connections, connect to DB and enable connections counters. */
        TestOrm &connectToDatabase();
        /*! Fire up TinyOrmPlayground. */
        TestOrm &run();

        /*! Get TinyOrmPlayground Configuration. */
        inline const Configuration &config() const;

    private:
        /*! ConfigurationsService alias. */
        using ConfigurationsService = Services::ConfigurationsService;
        /*! QueryCountersService alias. */
        using QueryCountersService = Services::QueryCountersService;

        /*! Main testing method. */
        void testAllConnections();
        /*! Testing method used when database connection will run in the main thread. */
        void testConnectionInMainThread(const QString &connection);
        /*! Testing method used when database connection will run in a worker thread. */
        void testConnectionInWorkerThread(const QString &connection);

        /*! Throw when m_configurations hash does not contain a given connection. */
        void throwIfNoConfig(const QString &connection) const;
        /*! Throw if connectToDatabase() was already called. */
        void throwIfAlreadyCalled() const;

        /*! Configuration for TinyOrmPlayground. */
        Configuration m_config {};
        /*! Database connection configurations service. */
        ConfigurationsService m_configurationsService {m_config};
        /*! Database connections query counters service. */
        QueryCountersService m_queryCountersService {m_config};

        /*! Database manager instance. */
        std::unique_ptr<Orm::DatabaseManager> m_db = nullptr;
    };

    const Configuration &TestOrm::config() const
    {
        return m_config;
    }

} // namespace TinyPlay

#endif // TINYPLAY_TESTORM_HPP
