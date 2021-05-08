#include <QCoreApplication>
#include <QDebug>

#ifdef _MSC_VER
#include <qt_windows.h>
#endif

#include <orm/utils/type.hpp>

#include "testorm.hpp"

// TODO investigate ctrl+shift+alt+b build errors about can not delete TinyOrmPlayground.vc.pdb, .ilk, TinyOrmPlayground_pch.pch and similar silverqx
int main(int, char *[])
{
#ifdef _MSC_VER
    SetConsoleOutputCP(CP_UTF8);
//    SetConsoleOutputCP(1250);
#endif

    QCoreApplication::setOrganizationName("crystal");
    QCoreApplication::setOrganizationDomain("crystal-studio.fun");
    QCoreApplication::setApplicationName("TinyOrmPlayground");

    qDebug() << "";

    try {
        TestOrm().connectToDatabase()
                 .run();

    } catch (const std::exception &e) {
        // TODO future decide how qCritical()/qFatal() really works, also regarding to the Qt Creator's settings 'Ignore first chance access violations' and similar silverqx
        // TODO future alse how to correctly setup this in prod/dev envs. silverqx
        qCritical().nospace().noquote()
                << "\n\nCaught '"
                // CUR still returns exception type eg for exceptions in MySqlConnector::getMySqlVersion silverqx
                << Orm::Utils::Type::classPureBasename(e, true)
                << "' Exception:\n" << e.what();
    }

    qDebug() << "";
}
