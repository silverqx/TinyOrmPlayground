#pragma once
#ifndef TINYPLAY_TESTS_TESTTINYORM_HPP
#define TINYPLAY_TESTS_TESTTINYORM_HPP

#include "tests/test.hpp"

namespace TinyPlay::Tests
{

    /*! Test TinyORM. */
    class TestTinyOrm final : public Test
    {
        Q_DISABLE_COPY(TestTinyOrm)

        /*! Inherit constructors. */
        using Test::Test;

        /*! Type used for the primary key ID. */
//        using KeyType = quint64;

    public:
        /*! Virtual destructor. */
        inline ~TestTinyOrm() final = default;

        /*! Execute the TestTinyOrm PlayTest. */
        void run() const final;
    };

} // namespace TinyPlay::Tests

#endif // TINYPLAY_TESTS_TESTTINYORM_HPP
