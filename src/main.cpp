#include <QDebug>

#include <qt_windows.h>

#include "testorm.hpp"

int main(int, char *[])
{
    SetConsoleOutputCP(CP_UTF8);
//    SetConsoleOutputCP(1250);

    qDebug() << "";

    TestOrm testOrm;
    testOrm.run();

    qDebug() << "\n";
}
