#include <QCoreApplication>
#include <QDebug>

#ifdef _MSC_VER
#include <qt_windows.h>
#endif

#include <orm/utils/type.hpp>

#include "testorm.hpp"

int main(int, char *[])
{
#ifdef _MSC_VER
    SetConsoleOutputCP(CP_UTF8);
//    SetConsoleOutputCP(1250);
#endif

    QCoreApplication::setOrganizationName("crystal");
    QCoreApplication::setOrganizationDomain("crystal-studio.fun");
    QCoreApplication::setApplicationName("TinyOrmPlayground");

    qInfo() << "";

    try {
        TestOrm().connectToDatabase()
                 .run();

    } catch (const std::exception &e) {
        // TODO future decide how qCritical()/qFatal() really works, also regarding to the Qt Creator's settings 'Ignore first chance access violations' and similar silverqx
        // TODO future alse how to correctly setup this in prod/dev envs. silverqx
        qCritical().nospace().noquote()
                << "\n\nCaught '"
                << Orm::Utils::Type::classPureBasename(e, true)
                << "' Exception:\n" << e.what();
    }

    qInfo() << "";
}
