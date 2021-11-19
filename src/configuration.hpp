#pragma once
#ifndef TINYPLAY_CONFIGURATION_HPP
#define TINYPLAY_CONFIGURATION_HPP

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
        inline static const QString MYSQL            = QStringLiteral("mysql");
        inline static const QString MYSQL_ALT        = QStringLiteral("mysql_alt");
        inline static const QString MYSQL_MAINTHREAD = QStringLiteral("mysql_mainthread");
        inline static const QString MYSQL_LARAVEL8   = QStringLiteral("mysql_laravel8");
        inline static const QString POSTGRES         = QStringLiteral("postgres");
        inline static const QString SQLITE           = QStringLiteral("sqlite");
        inline static const QString SQLITE_MEMORY    = QStringLiteral("sqlite_memory");
        inline static const QString
        SQLITE_CHECK_EXISTS_TRUE  = QStringLiteral("sqlite_check_exists_true");
        inline static const QString
        SQLITE_CHECK_EXISTS_FALSE = QStringLiteral("sqlite_check_exists_false");

        /*! Default constructor. */
        inline Configuration() = default;

        /*! Get configurations hash with all connections. */
        const OrmConfigurationsType &initConfigurations() const;

        /*! Path to the SQLite database file, for testing the 'check_database_exists'
            configuration option. */
        const QString CheckDatabaseExistsFile {initCheckDatabaseExistsFile()};

        /*! All connections configurations. */
        const OrmConfigurationsType Configurations {initConfigurations()};

        /*! Run connections defined in the ConnectionsToRunInThread in threads. */
        const bool ConnectionsInThreads = true;

        /*! Whether log output from connections in threads to a file or to the console. */
        const bool IsLoggingToFile = true;
        /*! Filepath to the log file for connections in threads. */
        inline static const QString LogFilepath {"E:/tmp/tinyplay.txt"};

        /*! Connections, for which the testQueryBuilder() and testTinyOrm()
            will be ran on. */
        inline static const QStringList
//    CONNECTIONS_TO_TEST {MYSQL};
        CONNECTIONS_TO_TEST {MYSQL, SQLITE, POSTGRES};

        /*! Which connections will run in separate threads, one connection per thread. */
        QStringList ConnectionsToRunInThread {MYSQL, SQLITE, POSTGRES};

        /*! Connections to count on statements and execution times, computed
            at runtime. */
        T_THREAD_LOCAL
        inline static QStringList
        CONNECTIONS_TO_COUNT {};

        /*! All countable connections. */
        const QStringList CountableConnections {
            MYSQL, MYSQL_ALT, MYSQL_MAINTHREAD,
            SQLITE, SQLITE_MEMORY,
            POSTGRES
        };
        /*! Connections that can run in a thread, so they are removable in the context
            of Configuration::CONNECTIONS_TO_COUNT/m_config.CountableConnections. */
        const QStringList RemovableConnections {MYSQL, SQLITE, POSTGRES};

        /*! Map of connection name to more related connections. */
        const std::unordered_map<QString, QStringList> ConnectionsMap {
            {MYSQL, {MYSQL, MYSQL_ALT}},
        };
        /*! Map real connection name to mapped connection name. */
        const std::unordered_map<QString, QString> ConnectionsMapReverse {
            {MYSQL,     MYSQL},
            {MYSQL_ALT, MYSQL},
        };

        /*! MySQL connection name used in the main thread, based on whether
            multi-threading is enabled. */
        const QString &MysqlMainThreadConnection {initMySqlMainThreadConnection()};

    private:
        /*! Get the filepath to the SQLite database file, for testing
            the 'check_database_exists' configuration option. */
        QString initCheckDatabaseExistsFile();
        /*! Obtain MySQL connection name, it has a different name when multi-threading
            is enabled to avoid collision in connection names. */
        const QString &initMySqlMainThreadConnection() const;
    };

} // namespace TinyPlay

#endif // TINYPLAY_CONFIGURATION_HPP
