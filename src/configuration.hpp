#pragma once
#ifndef TINYPLAY_CONFIGURATION_HPP
#define TINYPLAY_CONFIGURATION_HPP

#ifdef __linux__
#include <QDir>
#endif

#include <unordered_set>

#include <orm/macros/threadlocal.hpp>
#include <orm/support/databaseconfiguration.hpp>

namespace TinyPlay
{

    /*! Configuration for TinyOrmPlayground. */
    class Configuration
    {
        Q_DISABLE_COPY(Configuration)

        /*! Type for the Database Configuration. */
        using OrmConfiguration = Orm::Support::DatabaseConfiguration;
        /*! Type used for Database Connections map. */
        using OrmConfigurationsType = OrmConfiguration::ConfigurationsType;

    public:
        /* Database connection names */
        static const QString Mysql;
        static const QString Mysql_Alt;
        static const QString Mysql_MainThread;
        static const QString Mysql_Laravel8;
        static const QString Postgres;
        static const QString Sqlite;
        static const QString Sqlite_Memory;
        static const QString Sqlite_CheckExistsTrue;
        static const QString Sqlite_CheckExistsFalse;

        /*! All supported Tests. */
        enum struct Test
        {
            TestForPlay,
            TestAllOtherTests,
            TestConnection,
            TestQueryBuilderDbSpecific,
            TestQueryBuilder,
            TestTinyOrm,
            TestSchemaBuilder,
        };

        // All supported tests
        constexpr static Test TestForPlay       = Test::TestForPlay;
        constexpr static Test TestAllOtherTests = Test::TestAllOtherTests;
        constexpr static Test TestConnection    = Test::TestConnection;
        constexpr static Test
        TestQueryBuilderDbSpecific              = Test::TestQueryBuilderDbSpecific;
        constexpr static Test TestQueryBuilder  = Test::TestQueryBuilder;
        constexpr static Test TestTinyOrm       = Test::TestTinyOrm;
        constexpr static Test TestSchemaBuilder = Test::TestSchemaBuilder;

        /*! Default constructor. */
        inline Configuration() = default;

        /*! Return true if TinyOrmPlayground was built with debugging enabled, or
            false for release mode. */
        constexpr static bool isDebugBuild();

        /*! Path to the SQLite database file, for testing the 'check_database_exists'
            configuration option. */
        const QString CheckDatabaseExistsFile {initCheckDatabaseExistsFile()};

        /*! All connections configurations. */
        const OrmConfigurationsType Configurations {initDBConfigurations()};

        /*! Tests to invoke. */
        inline static std::unordered_set TestsToInvoke {
            TestForPlay,
            TestAllOtherTests,
            TestConnection,
            TestQueryBuilderDbSpecific,
            TestQueryBuilder,
            TestTinyOrm,
//            TestSchemaBuilder,
        };

        /*! Run connections defined in the ConnectionsToRunInThread in threads. */
        constexpr static const bool ConnectionsInThreads = true;

        /*! Whether log output from connections in threads to a file or to the console. */
        constexpr static const bool IsLoggingToFile = true;
        /*! Filepath to the log file for connections in threads. */
        constexpr static const char *const LogFilepath
#ifdef __linux__
        {"~/Code/tmp/tinyplay.txt"};
#else
        {"E:/tmp/tinyplay.txt"};
#endif

        /*! Connections, for which the testQueryBuilder() and testTinyOrm()
            will be ran on. */
        const QStringList ConnectionsToTest {
            Mysql,
            Sqlite,
            Postgres,
        };

        /*! Which connections will run in separate threads, one connection per thread. */
        QStringList ConnectionsToRunInThread {
            Mysql,
            Sqlite,
            Postgres,
        };

        /*! Connections to count on statements and execution times, computed
            at runtime. */
        T_THREAD_LOCAL
        inline static QStringList
        ConnectionsToCount {};

        /*! All countable connections. */
        const QStringList CountableConnections {
            Mysql, Mysql_Alt, Mysql_MainThread,
            Sqlite, Sqlite_Memory,
            Postgres
        };
        /*! Connections that can run in a thread, so they are removable in the context
            of Configuration::ConnectionsToCount/m_config.CountableConnections. */
        const QStringList RemovableConnections {Mysql, Sqlite, Postgres};

        /*! Map of connection name to more related connections. */
        const std::unordered_map<QString, QStringList> ConnectionsMap {
            {Mysql, {Mysql, Mysql_Alt}},
        };
        /*! Map real connection name to mapped connection name. */
        const std::unordered_map<QString, QString> ConnectionsMapReverse {
            {Mysql,     Mysql},
            {Mysql_Alt, Mysql},
        };

        /*! MySQL connection name used in the main thread, based on whether
            multi-threading is enabled. */
        const QString MysqlMainThreadConnection {initMySqlMainThreadConnection()};

    private:
        /*! Get DB connection configurations hash with all connections. */
        const OrmConfigurationsType &initDBConfigurations() const;

        /*! Get DB connection configuration for Mysql connection. */
        static QVariantHash initMysqlConfiguration();
        /*! Get DB connection configuration for Mysql_Laravel8 connection. */
        static QVariantHash initMysqlLaravel8Configuration();
        /*! Set connect/read/write timeouts to 1. */
        static void minimizeMysqlTimeouts(QVariantHash &connectionOptions);

        /*! Get the filepath to the SQLite database file, for testing
            the 'check_database_exists' configuration option. */
        static QString initCheckDatabaseExistsFile();
        /*! Obtain MySQL connection name, it has a different name when multi-threading
            is enabled to avoid collision in connection names. */
        static QString initMySqlMainThreadConnection();
    };

    /* public */

    constexpr bool Configuration::isDebugBuild()
    {
#ifdef TINYPLAY_DEBUG
        return true;
#else
        return false;
#endif
    }

} // namespace TinyPlay

namespace TinyPlay::DatabaseNames
{
    inline constexpr const auto &
    Mysql                   = Configuration::Mysql;
    inline constexpr const auto &
    Mysql_Alt               = Configuration::Mysql_Alt;
    inline constexpr const auto &
    Mysql_MainThread        = Configuration::Mysql_MainThread;
    inline constexpr const auto &
    Mysql_Laravel8          = Configuration::Mysql_Laravel8;
    inline constexpr const auto &
    Postgres                = Configuration::Postgres;
    inline constexpr const auto &
    Sqlite                  = Configuration::Sqlite;
    inline constexpr const auto &
    Sqlite_Memory           = Configuration::Sqlite_Memory;
    inline constexpr const auto &
    Sqlite_CheckExistsTrue  = Configuration::Sqlite_CheckExistsTrue;
    inline constexpr const auto &
    Sqlite_CheckExistsFalse = Configuration::Sqlite_CheckExistsFalse;
} // namespace TinyPlay::DatabaseNames

// Make these DB names accessible in all our namespaces by default
namespace TinyPlay
{
    using namespace TinyPlay::DatabaseNames; // NOLINT(google-build-using-namespace)
}
namespace Models
{
    using namespace TinyPlay::DatabaseNames; // NOLINT(google-build-using-namespace)
}

#endif // TINYPLAY_CONFIGURATION_HPP
