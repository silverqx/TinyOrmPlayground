#include <QCoreApplication>
#include <QDebug>
#include <QElapsedTimer>

#ifdef _WIN32
#include <qt_windows.h>

#include <fcntl.h>
#include <io.h>
#endif

#include <iostream>

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
    /* UTF-8 encoding is corrupted for narrow input functions, needed to use wcin/wstring
       for an input, input will be in the unicode encoding then is needed to translate
       unicode to utf8, eg. by QString::fromStdWString(), WideCharToMultiByte(), or
       std::codecvt(). It also works with msys2 ucrt64 gcc/clang. */
    SetConsoleCP(CP_UTF8);
    _setmode(_fileno(stdin), _O_WTEXT);
#endif

    Thread::nameThreadForDebugging("main");

    qInstallMessageHandler(&TinyPlay::Support::tinyMessageHandler);

    QCoreApplication::setOrganizationName("TinyORM");
    QCoreApplication::setOrganizationDomain("tinyorm.org");
    QCoreApplication::setApplicationName("TinyOrmPlayground");

    qInfo() << "";

    auto exitCode = EXIT_SUCCESS;

    QElapsedTimer timer;
    timer.start();

    try {
        TestOrm().connectToDatabase()
                 .run();

    } catch (const std::exception &e) {

        Utils::logException(e);

        exitCode = EXIT_FAILURE;
    }

    qInfo().nospace() << "⚡ Total TestOrm instance execution time : "
                      << timer.elapsed() << "ms\n"
                      << "---";

    qInfo() << "";

    return exitCode;
}
