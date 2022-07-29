#pragma once
#ifndef TINYPLAY_TESTS_TESTALLOTHERTESTS_HPP
#define TINYPLAY_TESTS_TESTALLOTHERTESTS_HPP

#include "tests/test.hpp"

namespace TinyPlay::Tests
{

    /*! All other tests. */
    class TestAllOtherTests final : public Test
    {
        Q_DISABLE_COPY(TestAllOtherTests)

        /*! Inherit constructors. */
        using Test::Test;

    public:
        /*! Virtual destructor. */
        inline ~TestAllOtherTests() final = default;

        /*! Execute the TestAllOtherTests PlayTest. */
        void run() const final;

    private:
        /*! Test TinyORM functions that cannot be included anywhere else. */
        void tinyOrmOthers() const;
        /*! Test nlohmann-json library. */
        void jsonConfig() const;
        /*! Test Qt's QStandardPaths. */
        void standardPaths() const;
        /*! C++ aggregates related tests. */
        void ctorAggregate() const;
    };

} // namespace TinyPlay::Tests

#endif // TINYPLAY_TESTS_TESTALLOTHERTESTS_HPP
