#pragma once
#ifndef TINYPLAY_TESTS_TESTSCHEMABUILDER_HPP
#define TINYPLAY_TESTS_TESTSCHEMABUILDER_HPP

#include "tests/test.hpp"

namespace TinyPlay::Tests
{

    /*! Test TinyORM's SchemaBuilder. */
    class TestSchemaBuilder final : public Test
    {
        Q_DISABLE_COPY(TestSchemaBuilder)

        /*! Inherit constructors. */
        using Test::Test;

    public:
        /*! Virtual destructor. */
        inline ~TestSchemaBuilder() final = default;

        /*! Execute the TestSchemaBuilder PlayTest. */
        void run() const final;
    };

} // namespace TinyPlay::Tests

#endif // TINYPLAY_TESTS_TESTSCHEMABUILDER_HPP
