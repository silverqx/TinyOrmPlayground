#include "tests/test.hpp"

namespace TinyPlay::Tests
{

Test::Test(const Configuration &config, ConnectionsService &connectionsService)
    : Concerns::LogsQueryCounters(connectionsService)
    , m_config(config)
{}

} // namespace TinyPlay::Tests
