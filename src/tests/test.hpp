#pragma once
#ifndef TINYPLAY_TESTS_TEST_HPP
#define TINYPLAY_TESTS_TEST_HPP

#include "concerns/logsquerycounters.hpp"

namespace TinyPlay
{

    class Configuration;

namespace Tests
{

    /*! PlayTests base class. */
    class Test : public Concerns::LogsQueryCounters
    {
        Q_DISABLE_COPY(Test)

    public:
        /*! Default constructor. */
        explicit Test(const Configuration &config,
                      QueryCountersService &queryCountersService);
        /*! Pure virtual destructor. */
        inline virtual ~Test() = 0;

        /*! Execute PlayTest. */
        virtual void run() const = 0;

    protected:
        /*! Reference to the TinyOrmPlayground Configuration. */
        const Configuration &m_config;
    };

    Test::~Test() = default;

} // namespace Tests
} // namespace TinyPlay

#endif // TINYPLAY_TESTS_TEST_HPP
