#ifndef TINYPLAY_TESTS_TESTCONNECTION_HPP
#define TINYPLAY_TESTS_TESTCONNECTION_HPP

#include "tests/test.hpp"

namespace TinyPlay::Tests
{

    /*! Test TinyORM's Database Connections. */
    class TestConnection final : public Test
    {
        Q_DISABLE_COPY(TestConnection)

        /*! Inherit constructors. */
        using Test::Test;

    public:
        /*! Virtual destructor. */
        inline ~TestConnection() final = default;

        /*! Execute the TestConnection PlayTest. */
        void run() final;
    };

} // namespace TinyPlay::Tests

#endif // TINYPLAY_TESTS_TESTCONNECTION_HPP
