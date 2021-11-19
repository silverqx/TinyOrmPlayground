#include "tests/test.hpp"

namespace TinyPlay::Tests
{

Test::Test(const Configuration &config, QueryCountersService &queryCountersService)
    : Concerns::LogsQueryCounters(queryCountersService)
    , m_config(config)
{}

} // namespace TinyPlay::Tests
