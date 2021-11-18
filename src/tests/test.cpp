#include "tests/test.hpp"

#include "testorm.hpp"

namespace TinyPlay::Tests
{

Test::Test(TestOrm &testOrm)
    : m_testOrm(testOrm)
    , m_config(testOrm.config())
{}

} // namespace TinyPlay::Tests
