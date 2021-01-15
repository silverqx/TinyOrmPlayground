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

    TestOrm testOrm;
    testOrm.run();

    qDebug() << "\n";
}
