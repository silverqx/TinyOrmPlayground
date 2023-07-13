#include <QCoreApplication>
#include <QDebug>

#ifdef _WIN32
#include <qt_windows.h>

#include <fcntl.h>
#include <io.h>
#endif

#include <iostream>

#include <orm/utils/helpers.hpp>
#include <orm/utils/thread.hpp>

#include "support/messagehandler.hpp"
#include "testorm.hpp"

using Orm::Utils::Helpers;

using ThreadUtils = Orm::Utils::Thread;

using TinyPlay::TestOrm;

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

    ThreadUtils::nameThreadForDebugging("main");

    qInstallMessageHandler(&TinyPlay::Support::tinyMessageHandler);

    QCoreApplication::setOrganizationName("TinyORM");
    QCoreApplication::setOrganizationDomain("tinyorm.org");
    QCoreApplication::setApplicationName("TinyOrmPlayground");

    qInfo() << "";

    QElapsedTimer timer;
    timer.start();

    try {
        TestOrm().connectToDatabase()
                 .run();

    } catch (const std::exception &e) {

        timer.invalidate();

        Helpers::logException(e);

        return EXIT_FAILURE;
    }

    qInfo().nospace() << "⚡ Total TestOrm instance execution time : "
                      << timer.elapsed() << "ms\n"
                      << "---";

    qInfo() << "";

    return EXIT_SUCCESS;
}
