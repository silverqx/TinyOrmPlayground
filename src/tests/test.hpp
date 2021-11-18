#pragma once
#ifndef TINYPLAY_BASETEST_HPP
#define TINYPLAY_BASETEST_HPP

#include <QtGlobal>

namespace TinyPlay
{

    class Configuration;
    class TestOrm;

namespace Tests
{

    /*! PlayTests base class. */
    class Test
    {
        Q_DISABLE_COPY(Test)

    public:
        /*! Default constructor. */
        explicit Test(TestOrm &testOrm);
        /*! Pure virtual destructor. */
        inline virtual ~Test() = 0;

        /*! Execute PlayTest. */
        virtual void run() = 0;

    protected:
        /*! Reference to the TestOrm. */
        TestOrm &m_testOrm;
        /*! Reference to the TinyOrmPlayground Configuration. */
        const Configuration &m_config;
    };

    Test::~Test() = default;

} // namespace Tests
} // namespace TinyPlay

#endif // TINYPLAY_BASETEST_HPP
