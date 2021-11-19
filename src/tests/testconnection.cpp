#include "tests/testconnection.hpp"

#include <QDebug>
#include <QElapsedTimer>
#include <QFile>

#include <orm/db.hpp>
#include <orm/exceptions/invalidargumenterror.hpp>
#include <orm/mysqlconnection.hpp>

#include "macros.hpp"
#include "testorm.hpp"

using Orm::Exceptions::InvalidArgumentError;
using Orm::MySqlConnection;

namespace TinyPlay::Tests
{

void TestConnection::run() const
{
    QElapsedTimer timer;
    timer.start();

    resetAllQueryLogCounters();

    qInfo().nospace()
            << "\n\n======================"
            << "\n  DatabaseConnection  "
            << "\n======================";

    /* DatabaseConnection::isMaria() - on MySQL connection */
    {
        qInfo() << "\n\nDatabaseConnection::isMaria() - on MySQL connection\n---";

        [[maybe_unused]]
        const auto isMaria = dynamic_cast<MySqlConnection &>(
                                 DB::connection(m_config.MysqlMainThreadConnection))
                             .isMaria();
        Q_ASSERT(!isMaria);

        qt_noop();
    }

    /* SQLite :memory: driver [sqlite_memory] */
    {
        qInfo() << "\n\nSQLite :memory: driver [sqlite_memory]\n---";

        auto &conn = DB::connection("sqlite_memory");

        conn.statement("create table tbl1 (one varchar(10), two smallint)");
        conn.insert("insert into tbl1 values(?, ?)", {"hello!", 10});
        conn.insert("insert into tbl1 values(?, ?)", {"goodbye", 20});

        auto query = conn.selectOne("select * from tbl1 where two = ?", {10});

        Q_ASSERT(query.isValid());
        Q_ASSERT(query.value("one").value<QString>() == "hello!");
        Q_ASSERT(query.value("two").value<int>() == 10);

        qt_noop();
    }

    /* SQLite database - check_database_exists */
    {
        // Remove the SQLite database file
        QFile::remove(m_config.CheckDatabaseExistsFile);
        Q_ASSERT(!QFile::exists(m_config.CheckDatabaseExistsFile));

        /* SQLite database [sqlite_check_exists_true] - check_database_exists - true */
        {
            qInfo() << "\n\nSQLite database [sqlite_check_exists_true] - "
                       "check_database_exists - true\n---";

            TINY_VERIFY_EXCEPTION_THROWN(
                        DB::connection("sqlite_check_exists_true")
                        .statement("create table tbl1 (one varchar(10), two smallint)"),
                        InvalidArgumentError);

            qt_noop();
        }

        /* SQLite database [sqlite_check_exists_false] - check_database_exists - false */
        {
            qInfo() << "\n\nSQLite database [sqlite_check_exists_false] - "
                       "check_database_exists - false\n---";

            // QSqlDatabase automatically creates a SQLite database file
            DB::connection("sqlite_check_exists_false")
                    .statement("create table tbl1 (one varchar(10), two smallint)");

            Q_ASSERT(QFile::exists(m_config.CheckDatabaseExistsFile));

            qt_noop();
        }

        QSqlDatabase::database("sqlite_check_exists_true").close();
        QSqlDatabase::database("sqlite_check_exists_false").close();

        // Remove the SQLite database file
        QFile::remove(m_config.CheckDatabaseExistsFile);
        Q_ASSERT(!QFile::exists(m_config.CheckDatabaseExistsFile));
    }

    logQueryCounters(__FUNCTION__, timer.elapsed());
}

} // namespace TinyPlay::Tests
