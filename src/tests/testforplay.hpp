#pragma once
#ifndef TINYPLAY_TESTS_ANOTHERTESTS_HPP
#define TINYPLAY_TESTS_ANOTHERTESTS_HPP

#include "tests/test.hpp"

namespace TinyPlay::Tests
{

    /*! Test intended to play. */
    class TestForPlay final : public Test
    {
        Q_DISABLE_COPY(TestForPlay)

        /*! Inherit constructors. */
        using Test::Test;

    public:
        /*! Virtual destructor. */
        inline ~TestForPlay() final = default;

        /*! Execute the TestForPlay PlayTest. */
        void run() const final;
    };

} // namespace TinyPlay::Tests

#endif // TINYPLAY_TESTS_ANOTHERTESTS_HPP
