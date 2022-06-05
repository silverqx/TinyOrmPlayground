#include "testorm.hpp"

#include <QDateTime>

#include <thread>

#include <orm/db.hpp>
#include <orm/exceptions/invalidargumenterror.hpp>
#include <orm/utils/thread.hpp>

#include "support/globals.hpp"
#include "support/utils.hpp"
#include "tests/testallothertests.hpp"
#include "tests/testconnection.hpp"
#include "tests/testforplay.hpp"
#include "tests/testquerybuilder.hpp"
#include "tests/testquerybuilderdbspecific.hpp"
#include "tests/testschemabuilder.hpp"
#include "tests/testtinyorm.hpp"

using Orm::DB;
using Orm::Exceptions::InvalidArgumentError;
using Orm::Exceptions::LogicError;
using Orm::Utils::Thread;

using TinyPlay::Support::Utils;

/*
   Notes:
   - text in [] names connection name, eg. [sqlite]
   - InvokeXTimes.ps1 - Average execution times in single-threaded mode:
     - F - Function, Q - Queries, A - TestOrm instance

   Performance:
   ---
   - only on MySQL connection Configuration::ConnectionsToTest {Mysql} and ms are best
     values I saw:
     - 13. jul 2021 ( 325 queries executed, TinyORM 749c4014 )
       - Qt 5.15.2 ; msvc 16.10 x64
         - debug build
           - under 945ms all functions
           - 273ms all queries
         - prod. build with disabled debug output ( QT_NO_DEBUG_OUTPUT )
           - under 413ms all functions
           - 247ms all queries
         - tst_model prod. build on MySQL connection only ( createConnections({Mysql}) )
           - 181ms
     - 10. oct 2021 ( 621 queries executed, TinyORM 878bb1f0 )
       - Qt 5.15.2 ; msvc 16.11.4 x64
         - prod. build LTO with disabled debug output ( QT_NO_DEBUG_OUTPUT )
           - around 670ms / min. 630ms all functions
           - around 350ms / min. 305ms all queries
   - InvokeXTimes.ps1 100 ( TinyPlay invoked 100 times, ms are average execution times,
                            TinyORM 92dd1e33, TinyOrmPlayground eb11db93)
     - 21. nov 2021 ( all connections, may be not accurate!! )
       - multi-thread
         - Qt 5.15.2 ; msvc 16.11.7 x64
           - debug build - 1331ms (I don't know how the heck I did this value)
         - Qt 5.15.2 ; gcc 11.2 x64 on Gentoo
           - debug build - 1064ms
       - single-thread
         - Qt 5.15.2 ; msvc 16.11.7 x64
           - debug build - F - 3834ms, Q - 1680ms, A - 3884ms
         - Qt 5.15.2 ; clang 12 x64 on Gentoo
           - debug build - F - 2422ms, Q - 993ms, A - 2429ms
         - Qt 5.15.2 ; clang 13 x64 on MSYS2 UCRT64
           - debug build - F - 2935ms, Q - 1635ms, A - 2983ms
     - 22. nov 2021 ( all connections, IsLoggingToFile = true )
       - multi-thread
         - Qt 5.15.2 ; msvc 16.11.7 x64
           - debug build   - 1999ms
           - release build - 1392ms
         - Qt 5.15.2 ; gcc 11.2 x64 on Gentoo
           - release build - 1019ms
         - Qt 5.15.2 ; clang 13 x64 on Gentoo
           - release build -  980ms, 1020ms, 961ms
       - single-thread
         - Qt 5.15.2 ; msvc 16.11.7 x64
           - debug build   - F - 3834ms, Q - 1680ms, A - 3884ms
           - release build - F - 2255ms, Q - 1592ms, A - 2261ms
         - Qt 5.15.2 ; gcc 11.2 x64 on Gentoo
           - release build - F - 2037ms, Q -  923ms, A - 2043ms
         - Qt 5.15.2 ; clang 12 x64 on Gentoo
           - release build - F - 2058ms, Q -  933ms, A - 2063ms
         - Qt 5.15.2 ; clang 13 x64 on Gentoo
           - release build - F - 2093ms, Q -  961ms, A - 2099ms
         - Qt 5.15.2 ; clang 13 x64 on MSYS2 UCRT64
           - release build - F - 2197ms, Q - 1582ms, A - 2220ms
         - Qt 5.15.2 ; gcc 11.2 x64 on MSYS2 UCRT64
           - release build - F - 2212ms, Q - 1584ms, A - 2235ms
*/

namespace TinyPlay
{

TestOrm &TestOrm::connectToDatabase()
{
    throwIfAlreadyCalled();

    /* Create database connections: mysql, sqlite and mysql_alt, and make
       mysql default database connection. */
    m_db = DB::create(m_configurationsService.computeConfigurationsToAdd(),
                      m_configurationsService.getDefaultConnectionName());

    Configuration::ConnectionsToCount =
            m_queryCountersService.computeConnectionsToCount();

    // Enable counters on all database connections
    m_queryCountersService.enableAllQueryLogCounters();

    return *this;
}

TestOrm &TestOrm::run()
{
    // Test intended for play
    if (shouldTest(TestForPlay))
        Tests::TestForPlay(m_config, m_queryCountersService).run();

    // Throw when unsupported environment detected
    throwIfUnsupportedEnv();

    // All other tests - jsonConfig(), standardPaths()
    if (shouldTest(TestAllOtherTests))
        Tests::TestAllOtherTests(m_config, m_queryCountersService).run();

    // Test TinyORM's Database Connections
    if (shouldTest(TestConnection))
        Tests::TestConnection(m_config, m_queryCountersService).run();

    // Database-specific tests related to the QueryBuilder
    if (shouldTest(TestQueryBuilderDbSpecific))
        Tests::TestQueryBuilderDbSpecific(m_config, m_queryCountersService).run();

    /* Main Playground's test code */
    testAllConnections();

    // Add counters for main thread
    m_queryCountersService.saveCountersForAppSummary("main");

    // Whole application Summary
    m_queryCountersService.logApplicationSummary();

    return *this;
}

void TestOrm::testAllConnections()
{
    // Initialize the connections service for testing connections in threads
    m_queryCountersService.initConnsInThreadsTesting();

    {
        // BUG QDateTime is not reentrant, this avoids crashes in threads during added calendar to the Registry silverqx
        [[maybe_unused]]
        auto _ = QDateTime::fromString("st jan 6 14:51:23 2021"); // clazy:exclude=unused-non-trivial-variable

        // To join threads at the current block
        std::vector<std::jthread> threads;

        for (const auto &connection : m_config.ConnectionsToTest) {
            // Run connection in the worker thread
            if (Configuration::ConnectionsInThreads &&
                m_config.ConnectionsToRunInThread.contains(connection)
            ) {
                threads.emplace_back(&TestOrm::testConnectionInWorkerThread, this,
                                     connection);
                continue;
            }

            // Otherwise run in the main thread
            testConnectionInMainThread(connection);
        }
    } // join threads

    // Restore default connection
    DB::setDefaultConnection(m_configurationsService.getDefaultConnectionName());

    if constexpr (!Configuration::IsLoggingToFile)
        m_queryCountersService.replayThrdLogToConsole();
}

void TestOrm::testConnectionInMainThread(const QString &connection)
{
    DB::setDefaultConnection(connection);

    if (shouldTest(TestQueryBuilder))
        Tests::TestQueryBuilder(m_config, m_queryCountersService).run();

    if (shouldTest(TestTinyOrm))
        Tests::TestTinyOrm(m_config, m_queryCountersService).run();

    // Schema builder currently supports MySQL and PostgreSQL database only
    if (shouldTest(TestSchemaBuilder) &&
        (connection == Mysql || connection == Postgres)
    )
        Tests::TestSchemaBuilder(m_config, m_queryCountersService).run();
}

void TestOrm::testConnectionInWorkerThread(const QString &connection)
{
    try {
        Thread::nameThreadForDebugging(connection);

        throwIfNoConfig(connection);

        Support::g_inThread = true;

        DB::addConnections(
                    m_configurationsService.computeConfigurationsToAdd(connection));

        DB::setDefaultConnection(connection);

        Configuration::ConnectionsToCount =
                m_queryCountersService.computeConnectionsToCount(connection);

        // Enable counters on all database connections to count
        m_queryCountersService.enableAllQueryLogCounters();

        if (shouldTest(TestQueryBuilder))
            Tests::TestQueryBuilder(m_config, m_queryCountersService).run();

        if (shouldTest(TestTinyOrm))
            Tests::TestTinyOrm(m_config, m_queryCountersService).run();

        // Schema builder doesn't support multi-threading

        // Save counters from a current thread for Application Summary
        m_queryCountersService.saveCountersForAppSummary(connection);

        // Save log messages for a current thread, to the file or vector
        m_queryCountersService.saveLogsFromThread(connection);

    } catch (const std::exception &e) {
        // Secure that an exception will be logged to the console
        Support::g_inThread = false;

        Utils::logException(e);
    }
}

void TestOrm::throwIfNoConfig(const QString &connection) const
{
    if (!m_config.Configurations.contains(connection))
        throw InvalidArgumentError(
                QStringLiteral("Configurations::Configurations does not contain '%1' "
                               "configuration.")
                .arg(connection));
}

void TestOrm::throwIfAlreadyCalled() const
{
    if (m_db != nullptr)
        throw LogicError("TestOrm::connectToDatabase() can be called only once.");
}

void TestOrm::throwIfUnsupportedEnv() const
{
    /* thread_local is not used on unsupported compilers, T_THREAD_LOCAL macro is empty
       in this case, so running TinyPlay in single thread works good on all compilers. */
    if constexpr (!Configuration::ConnectionsInThreads)
        return;

#if defined(__clang__) && defined(__MINGW32__)
    throw Orm::Exceptions::RuntimeError(
                "Multi-threading is not supported for Clang on MinGW because of TLS "
                "wrapper bugs and crashes.");
#elif defined(__clang__) && !defined(__MINGW32__) && defined(_MSC_VER)
    throw Orm::Exceptions::RuntimeError(
                "Multi-threading is not supported for clang-cl with MSVC because of "
                "TLS wrapper bugs and crashes.");
// I have disable multi-threading for Clang on Linux
//#elif defined(__clang__) && !defined(__MINGW32__) && __clang_major__ < 13
//    throw Orm::Exceptions::RuntimeError(
//                "Multi-threading is not supported for Clang <13 on Linux because of "
//                "TLS wrapper bugs and crashes.");
#elif defined(__GNUG__) && !defined(__clang__) && defined(__MINGW32__)
    throw Orm::Exceptions::RuntimeError(
                "Multi-threading is not supported for GCC on MinGW because of TLS "
                "wrapper bugs, problems with duplicit TLS wrappers during linkage "
                "with ld 2.3.7 or lld 13.");
#endif
}

} // namespace TinyPlay
