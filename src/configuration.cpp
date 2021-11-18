#include "configuration.hpp"

#include <QDir>

#include <stdexcept>

namespace TinyPlay
{

Configuration::Configuration()
    : CheckDatabaseExistsFile(getCheckDatabaseExistsFile())
    , MysqlMainThreadConnection(getMySqlMainThreadConnection())
{}

QString Configuration::getCheckDatabaseExistsFile()
{
    auto path = qEnvironmentVariable("DB_SQLITE_DATABASE", "");

    if (path.isEmpty())
        throw std::runtime_error(
                "Undefined environment variable 'DB_SQLITE_DATABASE'.");

    path.truncate(QDir::fromNativeSeparators(path).lastIndexOf(QChar('/')));

    return path + "/q_tinyorm_test-check_exists.sqlite3";
}

const QString &Configuration::getMySqlMainThreadConnection() const
{
    static const QString cached =
            ConnectionsInThreads ? QStringLiteral("mysql_mainthread")
                                   : QStringLiteral("mysql");
    return cached;
}

} // namespace TinyPlay
