#include "configuration.hpp"

#include <QDir>

#include <stdexcept>

#include <orm/constants.hpp>

#include "config.hpp"

using Orm::Constants::EMPTY;
using Orm::Constants::H127001;
using Orm::Constants::InnoDB;
using Orm::Constants::LOCAL;
using Orm::Constants::LOCALHOST;
using Orm::Constants::P3306;
using Orm::Constants::P5432;
using Orm::Constants::PUBLIC;
using Orm::Constants::QMYSQL;
using Orm::Constants::QPSQL;
using Orm::Constants::QSQLITE;
using Orm::Constants::SYSTEM;
using Orm::Constants::UTF8;
using Orm::Constants::UTF8MB4;
using Orm::Constants::charset_;
using Orm::Constants::check_database_exists;
using Orm::Constants::collation_;
using Orm::Constants::database_;
using Orm::Constants::driver_;
using Orm::Constants::engine_;
using Orm::Constants::foreign_key_constraints;
using Orm::Constants::host_;
using Orm::Constants::isolation_level;
using Orm::Constants::options_;
using Orm::Constants::password_;
using Orm::Constants::port_;
using Orm::Constants::prefix_;
using Orm::Constants::prefix_indexes;
using Orm::Constants::schema_;
using Orm::Constants::strict_;
using Orm::Constants::timezone_;
using Orm::Constants::username_;

namespace TinyPlay
{

/* public */

const QString Configuration::Mysql            = QStringLiteral("mysql");
const QString Configuration::Mysql_Alt        = QStringLiteral("mysql_alt");
const QString Configuration::Mysql_MainThread = QStringLiteral("mysql_mainthread");
const QString Configuration::Mysql_Laravel8   = QStringLiteral("mysql_laravel8");
const QString Configuration::Postgres         = QStringLiteral("postgres");
const QString Configuration::Sqlite           = QStringLiteral("sqlite");
const QString Configuration::Sqlite_Memory    = QStringLiteral("sqlite_memory");
const QString
Configuration::Sqlite_CheckExistsTrue  = QStringLiteral("sqlite_check_exists_true");
const QString
Configuration::Sqlite_CheckExistsFalse = QStringLiteral("sqlite_check_exists_false");

/* private */

const Configuration::OrmConfigurationsType &Configuration::initDBConfigurations() const
{
    static const QVariantHash mysqlConnection =         initMysqlConfiguration();
    static const QVariantHash mysqlLaravel8Connection = initMysqlLaravel8Configuration();

    static const OrmConfigurationsType cached {
        // Main MySQL connection in test loop
        {Mysql, mysqlConnection},

        // Used in the Torrent model as u_connection
        {Mysql_Alt, mysqlConnection},

        /* Used as MySQL connection name in the main thread when connections in threads
           is enabled to avoid MySQL connection name collision. */
        {Mysql_MainThread, mysqlConnection},

        /* Used in the testQueryBuilderDbSpecific() only to test a cross-database query,
           a connection to the "laravel_8" database. */
        {Mysql_Laravel8, mysqlLaravel8Connection},

        // Main SQLite connection in test loop
        {Sqlite, {
            {driver_,    QSQLITE},
            {database_,  qEnvironmentVariable("DB_SQLITE_DATABASE", EMPTY)},
            {prefix_,    EMPTY},
            {options_,   QVariantHash()},
            {foreign_key_constraints, qEnvironmentVariable("DB_SQLITE_FOREIGN_KEYS",
                                                           QStringLiteral("true"))},
            {check_database_exists,   true},
        }},

        // Used in the testConnection() only to test SQLite :memory: driver
        {Sqlite_Memory, {
            {driver_,    QSQLITE},
            {database_,  QStringLiteral(":memory:")},
            {prefix_,    EMPTY},
            {options_,   QVariantHash()},
            {foreign_key_constraints, qEnvironmentVariable("DB_SQLITE_FOREIGN_KEYS",
                                                           QStringLiteral("true"))},
        }},

        /* Used in the testConnection() only to test behavior when the configuration
           option check_database_exists = true. */
        {Sqlite_CheckExistsTrue, {
            {driver_,    QSQLITE},
            {database_,  CheckDatabaseExistsFile},
            {check_database_exists, true},
        }},

        /* Used in the testConnection() only to test behavior when the configuration
           option check_database_exists = true. */
        {Sqlite_CheckExistsFalse, {
            {driver_,    QSQLITE},
            {database_,  CheckDatabaseExistsFile},
            {check_database_exists, false},
        }},

        // Main PostgreSQL connection in test loop
        {Postgres, {
            {driver_,   QPSQL},
            {host_,     qEnvironmentVariable("DB_PGSQL_HOST",     H127001)},
            {port_,     qEnvironmentVariable("DB_PGSQL_PORT",     P5432)},
            {database_, qEnvironmentVariable("DB_PGSQL_DATABASE", EMPTY)},
            {schema_,   qEnvironmentVariable("DB_PGSQL_SCHEMA",   PUBLIC)},
            {username_, qEnvironmentVariable("DB_PGSQL_USERNAME", "postgres")},
            {password_, qEnvironmentVariable("DB_PGSQL_PASSWORD", EMPTY)},
            {charset_,  qEnvironmentVariable("DB_PGSQL_CHARSET",  UTF8)},
            // I don't use timezone types in postgres anyway
            {timezone_,       LOCAL},
            {prefix_,         EMPTY},
            {prefix_indexes,  true},
            // ConnectionFactory provides a default value for this, this is only for reference
//            {dont_drop,      QStringList {QStringLiteral("spatial_ref_sys")}},
            {options_,        QVariantHash(/*{{"requiressl", 1}}*/)},
        }},
    };

    return cached;
}

QVariantHash Configuration::initMysqlConfiguration()
{
    QVariantHash mysqlConnection {
        {driver_,    QMYSQL},
        {host_,      qEnvironmentVariable("DB_MYSQL_HOST", H127001)},
        {port_,      qEnvironmentVariable("DB_MYSQL_PORT", P3306)},
        {database_,  qEnvironmentVariable("DB_MYSQL_DATABASE", EMPTY)},
        {username_,  qEnvironmentVariable("DB_MYSQL_USERNAME", EMPTY)},
        {password_,  qEnvironmentVariable("DB_MYSQL_PASSWORD", EMPTY)},
        {charset_,   qEnvironmentVariable("DB_MYSQL_CHARSET", UTF8MB4)},
        {collation_, qEnvironmentVariable("DB_MYSQL_COLLATION",
                                          QStringLiteral("utf8mb4_0900_ai_ci"))},
        // CUR add timezone names to the MySQL server and test them silverqx
        {timezone_,       SYSTEM},
        {prefix_,         EMPTY},
        {prefix_indexes,  true},
        {strict_,         true},
        {isolation_level, QStringLiteral("REPEATABLE READ")},
        {engine_,         InnoDB},
        {options_,        QVariantHash()},
    };

    // Minimize all timeouts, on localhost it's ok
    minimizeMysqlTimeouts(mysqlConnection);

    return mysqlConnection;
}

QVariantHash Configuration::initMysqlLaravel8Configuration()
{
    QVariantHash mysqlLaravel8Connection {
        {driver_,    QMYSQL},
        {host_,      qEnvironmentVariable("DB_MYSQL_LARAVEL_HOST", H127001)},
        {port_,      qEnvironmentVariable("DB_MYSQL_LARAVEL_PORT", P3306)},
        {database_,  qEnvironmentVariable("DB_MYSQL_LARAVEL_DATABASE", EMPTY)},
        {username_,  qEnvironmentVariable("DB_MYSQL_LARAVEL_USERNAME", EMPTY)},
        {password_,  qEnvironmentVariable("DB_MYSQL_LARAVEL_PASSWORD", EMPTY)},
        {charset_,   qEnvironmentVariable("DB_MYSQL_LARAVEL_CHARSET", UTF8MB4)},
        {collation_, qEnvironmentVariable("DB_MYSQL_LARAVEL_COLLATION",
                                          QStringLiteral("utf8mb4_0900_ai_ci"))},
        {timezone_,       SYSTEM},
        {prefix_,         EMPTY},
        {prefix_indexes,  true},
        {strict_,         true},
        {isolation_level, QStringLiteral("REPEATABLE READ")},
        {engine_,         InnoDB},
        {options_,        QVariantHash()},
    };

    // Minimize all timeouts, on localhost it's ok
    minimizeMysqlTimeouts(mysqlLaravel8Connection);

    return mysqlLaravel8Connection;
}

void Configuration::minimizeMysqlTimeouts(QVariantHash &connectionOptions)
{
    if (const auto &host = connectionOptions.find(host_).value();
        host != H127001 && host != LOCALHOST
    )
        return;

    auto &options = connectionOptions.find(options_).value();

    auto newOptions = options.value<QVariantHash>();
    newOptions.insert({{"MYSQL_OPT_CONNECT_TIMEOUT", 1},
                       {"MYSQL_OPT_READ_TIMEOUT",    1},
                       {"MYSQL_OPT_WRITE_TIMEOUT",   1}});

    options = std::move(newOptions);
}

QString Configuration::initCheckDatabaseExistsFile()
{
    auto path = qEnvironmentVariable("DB_SQLITE_DATABASE", EMPTY);

    if (path.isEmpty())
        throw std::runtime_error(
                "Undefined environment variable 'DB_SQLITE_DATABASE'.");

    path.truncate(QDir::fromNativeSeparators(path).lastIndexOf(QChar('/')));

    return path + "/q_tinyorm_test-check_exists.sqlite3";
}

QString Configuration::initMySqlMainThreadConnection() const
{
    if constexpr (Configuration::ConnectionsInThreads)
        return Mysql_MainThread;

    return Mysql;
}

} // namespace TinyPlay
