#pragma once
#ifndef TINYPLAY_CONFIGURATION_HPP
#define TINYPLAY_CONFIGURATION_HPP

#include <QStringList>

namespace TinyPlay
{

    /*! Configuration for TinyOrmPlayground. */
    class Configuration
    {
        Q_DISABLE_COPY(Configuration)

    public:
        /*! Default constructor. */
        Configuration();

        /*! Run connections defined in m_inThreads in threads. */
        const bool ConnectionsInThreads = true;

        /*! Whether log output from connections in threads to a file or to the console. */
        const bool IsLoggingToFile = true;
        /*! Filepath to the log file for connections in threads. */
        inline static const QString LogFilepath {"E:/tmp/tinyplay.txt"};

        /*! Connections, for which the testQueryBuilder() and testTinyOrm()
            will be ran on. */
        inline static const QStringList
//    CONNECTIONS_TO_TEST {"mysql"};
        CONNECTIONS_TO_TEST {"mysql", "sqlite", "postgres"};

        /*! Which connections will run in separate threads, one connection per thread. */
        QStringList ConnectionsToRunInThread {"mysql", "sqlite", "postgres"};

        /*! Connections to count on statements and execution times, computed
            at runtime. */
        thread_local
        inline static QStringList
        CONNECTIONS_TO_COUNT {};

        /*! All countable connections. */
        const QStringList CountableConnections {
            "mysql", "mysql_alt", "mysql_mainthread",
            "sqlite", "sqlite_memory",
            "postgres"
        };
        /*! Connections that can run in a thread, so they are removable in the context
            of Configuration::CONNECTIONS_TO_COUNT/m_config.CountableConnections. */
        const QStringList RemovableConnections {"mysql", "sqlite", "postgres"};

        /*! Map of connection name to more related connections. */
        const std::unordered_map<QString, QStringList> ConnectionsMap {
            {"mysql", {"mysql", "mysql_alt"}},
        };
        /*! Map real connection name to mapped connection name. */
        const std::unordered_map<QString, QString> ConnectionsMapReverse {
            {"mysql",     "mysql"},
            {"mysql_alt", "mysql"},
        };

        /*! Path to the SQLite database file, for testing the 'check_database_exists'
            configuration option. */
        const QString CheckDatabaseExistsFile;
        /*! MySQL connection name used in the main thread, based on whether
            multi-threading is enabled. */
        const QString &MysqlMainThreadConnection;

    private:
        /*! Get the filepath to the SQLite database file, for testing
            the 'check_database_exists' configuration option. */
        QString getCheckDatabaseExistsFile();
        /*! Obtain MySQL connection name, it has a different name when multi-threading
            is enabled to avoid collision in connection names. */
        const QString &getMySqlMainThreadConnection() const;
    };

} // namespace TinyPlay

#endif // TINYPLAY_CONFIGURATION_HPP
