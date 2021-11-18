#pragma once
#ifndef TINYPLAY_CONFIGURATION_HPP
#define TINYPLAY_CONFIGURATION_HPP

#include <QString>

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
