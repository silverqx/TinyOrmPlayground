#include "configuration.hpp"

#include <QDir>
#include <QTimeZone>

#include <orm/constants.hpp>
#include <orm/exceptions/invalidargumenterror.hpp>
#include <orm/exceptions/runtimeerror.hpp>
#include <orm/ormtypes.hpp>
#include <orm/utils/configuration.hpp>
#include <orm/utils/helpers.hpp>
#include <orm/utils/type.hpp>

using Orm::Constants::EMPTY;
using Orm::Constants::H127001;
using Orm::Constants::InnoDB;
using Orm::Constants::LOCALHOST;
using Orm::Constants::P3306;
using Orm::Constants::P5432;
using Orm::Constants::PUBLIC;
using Orm::Constants::QMYSQL;
using Orm::Constants::QPSQL;
using Orm::Constants::QSQLITE;
using Orm::Constants::TZ00;
using Orm::Constants::UTC;
using Orm::Constants::UTF8;
using Orm::Constants::UTF8MB4;
using Orm::Constants::UTF8MB40900aici;
using Orm::Constants::Version;
using Orm::Constants::application_name;
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
using Orm::Constants::postgres_;
using Orm::Constants::prefix_;
using Orm::Constants::prefix_indexes;
using Orm::Constants::qt_timezone;
using Orm::Constants::return_qdatetime;
using Orm::Constants::search_path;
using Orm::Constants::strict_;
using Orm::Constants::timezone_;
using Orm::Constants::username_;

using Orm::Utils::Helpers;

using ConfigUtils = Orm::Utils::Configuration;

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
            {driver_,                 QSQLITE},
            {database_,               qEnvironmentVariable("DB_SQLITE_DATABASE", EMPTY)},
            {foreign_key_constraints, qEnvironmentVariable("DB_SQLITE_FOREIGN_KEYS",
                                                           QStringLiteral("true"))},
            {check_database_exists,   true},
            // Specifies what time zone all QDateTime-s will have
            {qt_timezone,             QVariant::fromValue(Qt::UTC)},
            /* Return a QDateTime with the correct time zone instead of the QString,
               only works when the qt_timezone isn't set to the DontConvert. */
            {return_qdatetime,        true},
            {prefix_,                 EMPTY},
            // Prefixing indexes also works with the SQLite database
            {prefix_indexes,          false},
            {options_,                QVariantHash()},
        }},

        // Used in the testConnection() only to test SQLite :memory: driver
        {Sqlite_Memory, {
            {driver_,                 QSQLITE},
            {database_,               QStringLiteral(":memory:")},
            {foreign_key_constraints, qEnvironmentVariable("DB_SQLITE_FOREIGN_KEYS",
                                                           QStringLiteral("true"))},
            // Specifies what time zone all QDateTime-s will have
            {qt_timezone,             QVariant::fromValue(Qt::UTC)},
            /* Return a QDateTime with the correct time zone instead of the QString,
               only works when the qt_timezone isn't set to the DontConvert. */
            {return_qdatetime,        true},
            {prefix_,                 EMPTY},
            // Prefixing indexes also works with the SQLite database
            {prefix_indexes,          false},
            {options_,                QVariantHash()},
        }},

        /* Used in the testConnection() only to test behavior when the configuration
           option check_database_exists = true. */
        {Sqlite_CheckExistsTrue, {
            {driver_,               QSQLITE},
            {database_,             CheckDatabaseExistsFile},
            {check_database_exists, true},
        }},

        /* Used in the testConnection() only to test behavior when the configuration
           option check_database_exists = false. */
        {Sqlite_CheckExistsFalse, {
            {driver_,               QSQLITE},
            {database_,             CheckDatabaseExistsFile},
            {check_database_exists, false},
        }},

        // Main PostgreSQL connection in test loop
        {Postgres, {
            {driver_,            QPSQL},
            {application_name,   QStringLiteral("TinyOrmPlayground")},
            {host_,              qEnvironmentVariable("DB_PGSQL_HOST",       H127001)},
            {port_,              qEnvironmentVariable("DB_PGSQL_PORT",       P5432)},
            {database_,          qEnvironmentVariable("DB_PGSQL_DATABASE",   EMPTY)},
            {search_path,        qEnvironmentVariable("DB_PGSQL_SEARCHPATH", PUBLIC)},
            {username_,          qEnvironmentVariable("DB_PGSQL_USERNAME",   postgres_)},
            {password_,          qEnvironmentVariable("DB_PGSQL_PASSWORD",   EMPTY)},
            {charset_,           qEnvironmentVariable("DB_PGSQL_CHARSET",    UTF8)},
            // I don't use timezone types in postgres anyway
            {timezone_,          UTC},
            // Specifies what time zone all QDateTime-s will have
            {qt_timezone,        QVariant::fromValue(Qt::UTC)},
            {prefix_,            EMPTY},
            {prefix_indexes,     false},
            // ConnectionFactory provides a default value for this, this is only for reference
//            {dont_drop,          QStringList {spatial_ref_sys}},
            {options_,           ConfigUtils::postgresSslOptions()},
        }},
    };

    return cached;
}

QVariantHash Configuration::initMysqlConfiguration()
{
    QVariantHash mysqlConnection {
        {driver_,         QMYSQL},
        {host_,           qEnvironmentVariable("DB_MYSQL_HOST",      H127001)},
        {port_,           qEnvironmentVariable("DB_MYSQL_PORT",      P3306)},
        {database_,       qEnvironmentVariable("DB_MYSQL_DATABASE",  EMPTY)},
        {username_,       qEnvironmentVariable("DB_MYSQL_USERNAME",  EMPTY)},
        {password_,       qEnvironmentVariable("DB_MYSQL_PASSWORD",  EMPTY)},
        {charset_,        qEnvironmentVariable("DB_MYSQL_CHARSET",   UTF8MB4)},
        {collation_,      qEnvironmentVariable("DB_MYSQL_COLLATION", UTF8MB40900aici)},
        // Very important for tests
        {timezone_,       TZ00},
        // Specifies what time zone all QDateTime-s will have
        {qt_timezone,     QVariant::fromValue(Qt::UTC)},
        {prefix_,         EMPTY},
        {prefix_indexes,  false},
        {strict_,         true},
        {isolation_level, QStringLiteral("REPEATABLE READ")}, // MySQL default is REPEATABLE READ for InnoDB
        {engine_,         InnoDB},
        {Version,         {}}, // Autodetect
        {options_,        QVariantHash()},
    };

    // Minimize timeouts and setup SSL connection
    commonMySqlOptions(mysqlConnection);

    return mysqlConnection;
}

QVariantHash Configuration::initMysqlLaravel8Configuration()
{
    QVariantHash mysqlLaravel8Connection {
        {driver_,         QMYSQL},
        {host_,           qEnvironmentVariable("DB_MYSQL_LARAVEL_HOST",      H127001)},
        {port_,           qEnvironmentVariable("DB_MYSQL_LARAVEL_PORT",      P3306)},
        {database_,       qEnvironmentVariable("DB_MYSQL_LARAVEL_DATABASE",  EMPTY)},
        {username_,       qEnvironmentVariable("DB_MYSQL_LARAVEL_USERNAME",  EMPTY)},
        {password_,       qEnvironmentVariable("DB_MYSQL_LARAVEL_PASSWORD",  EMPTY)},
        {charset_,        qEnvironmentVariable("DB_MYSQL_LARAVEL_CHARSET",   UTF8MB4)},
        {collation_,      qEnvironmentVariable("DB_MYSQL_LARAVEL_COLLATION",
                                               UTF8MB40900aici)},
        // Very important for tests
        {timezone_,       TZ00},
        // Specifies what time zone all QDateTime-s will have
        {qt_timezone,     QVariant::fromValue(Qt::UTC)},
        {prefix_,         EMPTY},
        {prefix_indexes,  false},
        {strict_,         true},
        {isolation_level, QStringLiteral("REPEATABLE READ")}, // MySQL default is REPEATABLE READ for InnoDB
        {engine_,         InnoDB},
        {Version,         {}}, // Autodetect
        {options_,        QVariantHash()},
    };

    // Minimize timeouts and setup SSL connection
    commonMySqlOptions(mysqlLaravel8Connection);

    return mysqlLaravel8Connection;
}

void Configuration::commonMySqlOptions(QVariantHash &connectionOptions)
{
    auto &options = connectionOptions.find(options_).value();

    /* Validate, because the options.value<QVariantHash>() call returns empty hash if
       the value is the non-empty QString. */
    throwIfMySqlOptionsNotHash(options);

    auto newOptions = options.value<QVariantHash>();
    newOptions.reserve(6);

    // Minimize all timeouts, on localhost it's ok
#if defined(__GNUG__) && !defined(__clang__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wdangling-reference"
#endif
    if (const auto &host = connectionOptions.find(host_).value();
        host == H127001 || host == LOCALHOST
    )
#if defined(__GNUG__) && !defined(__clang__)
#  pragma GCC diagnostic pop
#endif
        ConfigUtils::minimizeMySqlTimeouts(newOptions);

    ConfigUtils::insertMySqlSslOptions(newOptions);

    options = newOptions;
}

void Configuration::throwIfMySqlOptionsNotHash(const QVariant &optionsVariant)
{
    if (Helpers::qVariantTypeId(optionsVariant) == QMetaType::QVariantHash)
        return;

    throw Orm::Exceptions::InvalidArgumentError(
                QStringLiteral(
                    "MySQL 'options' value must be the QVariantHash, TinyORM also "
                    "supports a QString, but I'm not going to implement this case "
                    "here, in %1().")
                .arg(__tiny_func__));
}

QString Configuration::initCheckDatabaseExistsFile()
{
    auto path = qEnvironmentVariable("DB_SQLITE_DATABASE", EMPTY);

    if (path.isEmpty())
        throw Orm::Exceptions::RuntimeError(
                "Undefined environment variable 'DB_SQLITE_DATABASE'.");

    path.truncate(QDir::fromNativeSeparators(path).lastIndexOf(QChar('/')));

    return path + "/tinyorm_test-check_exists.sqlite3";
}

QString Configuration::initMySqlMainThreadConnection()
{
    if constexpr (Configuration::ConnectionsInThreads)
        return Mysql_MainThread;

    return Mysql;
}

} // namespace TinyPlay
