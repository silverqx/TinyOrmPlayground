#include <QDebug>

#include <qt_windows.h>

#include "testorm.hpp"

int main(int, char *[])
{
    SetConsoleOutputCP(CP_UTF8);
//    SetConsoleOutputCP(1250);

    QCoreApplication::setOrganizationName("crystal");
    QCoreApplication::setOrganizationDomain("crystal-studio.fun");
    QCoreApplication::setApplicationName("TinyOrmPlayground");

    qDebug() << "";

    try {
        TestOrm().connectToDatabase()
                 .run();

    }  catch (const std::exception &e) {
        // TODO future decide how qCritical()/qFatal() really works, also regarding to the Qt Creator's settings 'Ignore first chance access violations' and similar silverqx
        // TODO future alse how to correctly setup this in prod/dev envs. silverqx
        qCritical() << e.what();
    }

    qDebug() << "\n";
}
