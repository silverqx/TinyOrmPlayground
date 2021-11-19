#include "testorm.hpp"

#include <QDateTime>

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
#include "tests/testtinyorm.hpp"

using Orm::Exceptions::InvalidArgumentError;
using Orm::Exceptions::LogicError;
using Orm::Utils::Thread;

using TinyPlay::Support::Utils;

/*
   Notes:
   - text in [] names connection name, eg. [sqlite]

   Performance:
   ---
   - only on MySQL connection CONNECTIONS_TO_TEST {mysql} and ms are best values I saw
     - 13. jul 2021 ( 325 queries executed, TinyORM 749c4014 )
       - Qt 5.15.2 ; msvc 16.10 x64
         - debug build
           - under 945ms all functions
           - 273ms all queries
         - prod. build with disabled debug output ( QT_NO_DEBUG_OUTPUT )
           - under 413ms all functions
           - 247ms all queries
         - tst_model prod. build on MySQL connection only ( createConnections({MYSQL}) )
           - 181ms
     - 10. oct 2021 ( 621 queries executed, TinyORM 878bb1f0 )
       - Qt 5.15.2 ; msvc 16.11.4 x64
         - prod. build LTO with disabled debug output ( QT_NO_DEBUG_OUTPUT )
           - around 670ms / min. 630ms all functions
           - around 350ms / min. 305ms all queries
*/

namespace TinyPlay
{

TestOrm &TestOrm::connectToDatabase()
{
    throwIfAlreadyCalled();

    /* Create database connections: mysql, sqlite and mysql_alt, and make
       mysql default database connection. */
    m_db = DB::create(m_configurationsService.computeConfigurationsToAdd(), "mysql");

    Configuration::CONNECTIONS_TO_COUNT =
            m_queryCountersService.computeConnectionsToCount();

    // Enable counters on all database connections
    m_queryCountersService.enableAllQueryLogCounters();

    return *this;
}

TestOrm &TestOrm::run()
{
    // Test intended for play
    Tests::TestForPlay(m_config, m_queryCountersService).run();

    // All other tests
    Tests::TestAllOtherTests(m_config, m_queryCountersService).run();

    // Test TinyORM's Database Connections
    Tests::TestConnection(m_config, m_queryCountersService).run();
    // Database-specific tests related to the QueryBuilder
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

        for (const auto &connection : Configuration::CONNECTIONS_TO_TEST) {
            // Run connection in the worker thread
            if (m_config.ConnectionsInThreads &&
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
    DB::setDefaultConnection("mysql");

    if (!m_config.IsLoggingToFile)
        m_queryCountersService.replayThrdLogToConsole();
}

void TestOrm::testConnectionInMainThread(const QString &connection)
{
    DB::setDefaultConnection(connection);

    Tests::TestQueryBuilder(m_config, m_queryCountersService).run();
    Tests::TestTinyOrm(m_config, m_queryCountersService).run();
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

        Configuration::CONNECTIONS_TO_COUNT =
                m_queryCountersService.computeConnectionsToCount(connection);

        // Enable counters on all database connections to count
        m_queryCountersService.enableAllQueryLogCounters();

        Tests::TestQueryBuilder(m_config, m_queryCountersService).run();
        Tests::TestTinyOrm(m_config, m_queryCountersService).run();

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

} // namespace TinyPlay
