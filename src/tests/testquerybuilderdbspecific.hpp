#pragma once
#ifndef TINYPLAY_TESTS_TESTQUERYBUILDERDBSPECIFIC_HPP
#define TINYPLAY_TESTS_TESTQUERYBUILDERDBSPECIFIC_HPP

#include "tests/test.hpp"

namespace TinyPlay::Tests
{

    /*! Database-specific tests related to the QueryBuilder. */
    class TestQueryBuilderDbSpecific final : public Test
    {
        Q_DISABLE_COPY(TestQueryBuilderDbSpecific)

        /*! Inherit constructors. */
        using Test::Test;

    public:
        /*! Virtual destructor. */
        inline ~TestQueryBuilderDbSpecific() final = default;

        /*! Execute the TestQueryBuilderDbSpecific PlayTest. */
        void run() final;
    };

} // namespace TinyPlay::Tests

#endif // TINYPLAY_TESTS_TESTQUERYBUILDERDBSPECIFIC_HPP
