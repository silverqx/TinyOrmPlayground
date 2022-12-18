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

        /*! Alias for the Configuration::Test enum. */
        using Test = Configuration::Test;

        // All supported tests
        constexpr static Test TestForPlay       = Test::TestForPlay;
        constexpr static Test TestAllOtherTests = Test::TestAllOtherTests;
        constexpr static Test TestConnection    = Test::TestConnection;
        constexpr static Test
        TestQueryBuilderDbSpecific              = Test::TestQueryBuilderDbSpecific;
        constexpr static Test TestQueryBuilder  = Test::TestQueryBuilder;
        constexpr static Test TestTinyOrm       = Test::TestTinyOrm;
        constexpr static Test TestSchemaBuilder = Test::TestSchemaBuilder;

        /*! Whether to invoke the given test. */
        inline static bool shouldTest(Configuration::Test test);

        /*! Throw when m_configurations hash does not contain a given connection. */
        void throwIfNoConfig(const QString &connection) const;
        /*! Throw if connectToDatabase() was already called. */
        void throwIfAlreadyCalled() const;

        /*! Throw when unsupported environment detected. */
        static void throwIfUnsupportedEnv();

        /*! Configuration for TinyOrmPlayground. */
        Configuration m_config {};
        /*! Database connection configurations service. */
        ConfigurationsService m_configurationsService {m_config};
        /*! Database connections query counters service. */
        QueryCountersService m_queryCountersService {m_config};

        /*! Database manager instance. */
        std::shared_ptr<Orm::DatabaseManager> m_db = nullptr;
    };

    /* public */

    const Configuration &TestOrm::config() const
    {
        return m_config;
    }

    /* private */

    bool TestOrm::shouldTest(Configuration::Test test)
    {
        return Configuration::TestsToInvoke.contains(test);
    }

} // namespace TinyPlay

#endif // TINYPLAY_TESTORM_HPP
