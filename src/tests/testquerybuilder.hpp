#pragma once
#ifndef TINYPLAY_TESTS_TESTQUERYBUILDER_HPP
#define TINYPLAY_TESTS_TESTQUERYBUILDER_HPP

#include "tests/test.hpp"

namespace TinyPlay::Tests
{

    /*! Test TinyORM's QueryBuilder. */
    class TestQueryBuilder final : public Test
    {
        Q_DISABLE_COPY(TestQueryBuilder)

        /*! Inherit constructors. */
        using Test::Test;

    public:
        /*! Virtual destructor. */
        inline ~TestQueryBuilder() final = default;

        /*! Execute the TestQueryBuilder PlayTest. */
        void run() const final;
    };

} // namespace TinyPlay::Tests

#endif // TINYPLAY_TESTS_TESTQUERYBUILDER_HPP
