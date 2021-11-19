#include "configuration.hpp"

#include <QDir>

#include <stdexcept>

#include <orm/constants.hpp>

using Orm::Constants::H127001;
using Orm::Constants::P3306;
using Orm::Constants::P5432;
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
using Orm::Constants::foreign_key_constraints;
using Orm::Constants::host_;
using Orm::Constants::isolation_level;
using Orm::Constants::LOCAL;
using Orm::Constants::options_;
using Orm::Constants::password_;
using Orm::Constants::port_;
using Orm::Constants::prefix_;
using Orm::Constants::PUBLIC;
using Orm::Constants::schema_;
using Orm::Constants::strict_;
using Orm::Constants::timezone_;
using Orm::Constants::username_;

namespace TinyPlay
{

const Configuration::OrmConfigurationsType &Configuration::initConfigurations() const
{
    static const QVariantHash mysqlConnection {
        {driver_,    QMYSQL},
        {host_,      qEnvironmentVariable("DB_MYSQL_HOST", H127001)},
        {port_,      qEnvironmentVariable("DB_MYSQL_PORT", P3306)},
        {database_,  qEnvironmentVariable("DB_MYSQL_DATABASE", "")},
        {username_,  qEnvironmentVariable("DB_MYSQL_USERNAME", "")},
        {password_,  qEnvironmentVariable("DB_MYSQL_PASSWORD", "")},
        {charset_,   qEnvironmentVariable("DB_MYSQL_CHARSET", UTF8MB4)},
        {collation_, qEnvironmentVariable("DB_MYSQL_COLLATION",
                                          QStringLiteral("utf8mb4_0900_ai_ci"))},
        // CUR add timezone names to the MySQL server and test them silverqx
        {timezone_,       SYSTEM},
        {prefix_,         ""},
        {strict_,         true},
        {isolation_level, QStringLiteral("REPEATABLE READ")},
        {options_,        QVariantHash()},
    };

    static const OrmConfigurationsType cached {
        // Main MySQL connection in test loop
        {"mysql", mysqlConnection},

        // Used in the Torrent model as u_connection
        {"mysql_alt", mysqlConnection},

        /* Used as MySQL connection name in the main thread when connections in threads
           is enabled to avoid MySQL connection name collision. */
        {"mysql_mainthread", mysqlConnection},

        /* Used in the testQueryBuilderDbSpecific() only to test a cross-database query,
           a connection to the "laravel_8" database. */
        {"mysql_laravel8", {
            {driver_,    QMYSQL},
            {host_,      qEnvironmentVariable("DB_MYSQL_LARAVEL_HOST", H127001)},
            {port_,      qEnvironmentVariable("DB_MYSQL_LARAVEL_PORT", P3306)},
            {database_,  qEnvironmentVariable("DB_MYSQL_LARAVEL_DATABASE", "")},
            {username_,  qEnvironmentVariable("DB_MYSQL_LARAVEL_USERNAME", "")},
            {password_,  qEnvironmentVariable("DB_MYSQL_LARAVEL_PASSWORD", "")},
            {charset_,   qEnvironmentVariable("DB_MYSQL_LARAVEL_CHARSET", UTF8MB4)},
            {collation_, qEnvironmentVariable("DB_MYSQL_LARAVEL_COLLATION",
                                              QStringLiteral("utf8mb4_0900_ai_ci"))},
            {timezone_,       SYSTEM},
            {prefix_,         ""},
            {strict_,         true},
            {isolation_level, QStringLiteral("REPEATABLE READ")},
            {options_,        QVariantHash()},
        }},

        // Main SQLite connection in test loop
        {"sqlite", {
            {driver_,    QSQLITE},
            {database_,  qEnvironmentVariable("DB_SQLITE_DATABASE", "")},
            {prefix_,    ""},
            {options_,   QVariantHash()},
            {foreign_key_constraints, qEnvironmentVariable("DB_SQLITE_FOREIGN_KEYS",
                                                           QStringLiteral("true"))},
            {check_database_exists,   true},
        }},

        // Used in the testConnection() only to test SQLite :memory: driver
        {"sqlite_memory", {
            {driver_,    QSQLITE},
            {database_,  QStringLiteral(":memory:")},
            {prefix_,    ""},
            {options_,   QVariantHash()},
            {foreign_key_constraints, qEnvironmentVariable("DB_SQLITE_FOREIGN_KEYS",
                                                           QStringLiteral("true"))},
        }},

        /* Used in the testConnection() only to test behavior when the configuration
           option check_database_exists = true. */
        {"sqlite_check_exists_true", {
            {driver_,    QSQLITE},
            {database_,  CheckDatabaseExistsFile},
            {check_database_exists, true},
        }},

        /* Used in the testConnection() only to test behavior when the configuration
           option check_database_exists = true. */
        {"sqlite_check_exists_false", {
            {driver_,    QSQLITE},
            {database_,  CheckDatabaseExistsFile},
            {check_database_exists, false},
        }},

        // Main PostgreSQL connection in test loop
        {"postgres", {
            {driver_,   QPSQL},
            {host_,     qEnvironmentVariable("DB_PGSQL_HOST",     H127001)},
            {port_,     qEnvironmentVariable("DB_PGSQL_PORT",     P5432)},
            {database_, qEnvironmentVariable("DB_PGSQL_DATABASE", "")},
            {schema_,   qEnvironmentVariable("DB_PGSQL_SCHEMA",   PUBLIC)},
            {username_, qEnvironmentVariable("DB_PGSQL_USERNAME", "postgres")},
            {password_, qEnvironmentVariable("DB_PGSQL_PASSWORD", "")},
            {charset_,  qEnvironmentVariable("DB_PGSQL_CHARSET",  UTF8)},
            // I don't use timezone types in postgres anyway
            {timezone_, LOCAL},
            {prefix_,   ""},
            {options_,  QVariantHash(/*{{"requiressl", 1}}*/)},
        }},
    };

    return cached;
}

QString Configuration::initCheckDatabaseExistsFile()
{
    auto path = qEnvironmentVariable("DB_SQLITE_DATABASE", "");

    if (path.isEmpty())
        throw std::runtime_error(
                "Undefined environment variable 'DB_SQLITE_DATABASE'.");

    path.truncate(QDir::fromNativeSeparators(path).lastIndexOf(QChar('/')));

    return path + "/q_tinyorm_test-check_exists.sqlite3";
}

const QString &Configuration::initMySqlMainThreadConnection() const
{
    static const QString cached = ConnectionsInThreads
                                  ? QStringLiteral("mysql_mainthread")
                                  : QStringLiteral("mysql");
    return cached;
}

} // namespace TinyPlay
