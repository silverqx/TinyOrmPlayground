#include <QDebug>

#include <qt_windows.h>

#include <orm/utils/type.hpp>

#include "testorm.hpp"

// TODO investigate ctrl+shift+alt+b build errors about can not delete TinyOrmPlayground.vc.pdb, .ilk, TinyOrmPlayground_pch.pch and similar silverqx
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

    // BUG if I add catch eg for Orm::RuntimeError, it didn't catch it, it will be std::exception type instead, I don't understand why, investigate silverqx
    } catch (const std::exception &e) {
        // TODO future decide how qCritical()/qFatal() really works, also regarding to the Qt Creator's settings 'Ignore first chance access violations' and similar silverqx
        // TODO future alse how to correctly setup this in prod/dev envs. silverqx
        qCritical().nospace().noquote()
                << "\n\nCaught '"
                // CUR still returns exception type eg for exceptions in MySqlConnector::getMySqlVersion silverqx
                << Orm::Utils::Type::classPureBasename(e)
                << "' Exception:\n" << e.what();
    }

    qDebug() << "";
}
