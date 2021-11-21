#include <QCoreApplication>
#include <QDebug>
#include <QElapsedTimer>

#ifdef _WIN32
#include <qt_windows.h>
#endif

#include <orm/utils/thread.hpp>

#include "support/messagehandler.hpp"
#include "support/utils.hpp"
#include "testorm.hpp"

using Orm::Utils::Thread;

using TinyPlay::TestOrm;
using TinyPlay::Support::Utils;

int main(int /*unused*/, char */*unused*/[])
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
//    SetConsoleOutputCP(1250);
#endif

    Thread::nameThreadForDebugging("main");

    qInstallMessageHandler(&TinyPlay::Support::tinyMessageHandler);

    QCoreApplication::setOrganizationName("tinyorm");
    QCoreApplication::setOrganizationDomain("silverqx.github.io");
    QCoreApplication::setApplicationName("TinyOrmPlayground");

    qInfo() << "";

    auto excpetion = false;

    QElapsedTimer timer;
    timer.start();

    try {
        TestOrm().connectToDatabase()
                 .run();

    } catch (const std::exception &e) {
        Utils::logException(e);

        excpetion = true;
    }

    qInfo().nospace() << "⚡ Total TestOrm instance execution time : "
                      << timer.elapsed() << "ms\n"
                      << "---";

    qInfo() << "";

    return excpetion ? 1 : 0;
}
