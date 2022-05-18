#include "tests/testforplay.hpp"

#include <QDebug>
#include <QElapsedTimer>

#include <filesystem>
#include <iostream>
#include <typeindex>
#include <typeinfo>

#include <range/v3/all.hpp>

#include <orm/db.hpp>
#include <orm/schema.hpp>
#include <orm/tiny/tinytypes.hpp>
#include <orm/version.hpp>

#include "configuration.hpp"
#include "models/torrent.hpp"
#include "models/torrenteager.hpp"
#include "version.hpp"

// BUG this causes crash of code model after upgrade to QtCreator 5 with clang 12 silverqx
//using namespace ranges;

using Orm::Constants::ASTERISK;
using Orm::Constants::COLON;
using Orm::Constants::COMMA;
using Orm::Constants::DOT;
using Orm::Constants::INNER;
using Orm::Constants::UNDERSCORE;

using Orm::DB; // NOLINT(misc-unused-using-decls)
using Orm::Schema; // NOLINT(misc-unused-using-decls)
using Orm::SchemaNs::Blueprint; // NOLINT(misc-unused-using-decls)
using Orm::Tiny::AttributeItem;

// NOLINTNEXTLINE(google-build-using-namespace)
using namespace Models;

namespace TinyPlay::Tests
{

void TestForPlay::run() const
{
    QElapsedTimer timer;
    timer.start();

    resetAllQueryLogCounters();

    qInfo().nospace()
            << "================="
            << "\n  Another Tests  "
            << "\n=================\n";

    {
        {

            qt_noop();
        }

        {

            qt_noop();
        }

        {

            qt_noop();
        }

        {

            qt_noop();
        }

        {

            qt_noop();
        }

        {

            qt_noop();
        }

        {

            qt_noop();
        }

//        while (users.next())
//            qDebug() << "id :" << users.value("id").value<quint64>() << ";"
//                     << "name :" << users.value("name").value<QString>();
////            qDebug() << "id :" << users.value("id").value<quint64>();

//        for (auto &user : users)
//            qDebug() << "id :" << user.getAttribute("id").value<quint64>();
////            qDebug() << "id :" << user.getAttribute("id").value<quint64>() << ";"
////                     << "name :" << user.getAttribute("name").value<QString>();

        // Log summary counters if needed
//        logQueryCounters(timer.elapsed());
//        return;

        qt_noop();
    }

    /* Ranges */
    {
        QVector<AttributeItem> attributes {
            {"one", 1},
            {"two", 2},
            {"three", 3},
            {"four", 4},
        };

//        const auto itAttribute = ranges::find_if(attributes,
//                                                 [](const auto &attribute)
//        {
//            return attribute.key == "three";
//        });

        const auto itAttribute = ranges::find(attributes, true,
                                              [](const auto &attribute)
        {
            return attribute.key == "three";
        });

        // Not found
        if (itAttribute == ranges::end(attributes))
            qDebug() << "not found";

        qt_noop();
    }

    /* __FUNCTION__ */
    {
        qInfo().nospace()
                << "\n\n=================="
                << "\n   __FUNCTION__   "
                << "\n==================\n\n";

        printf("Function name __tiny_func__ : %s\n", __tiny_func__.toUtf8().constData());
        printf("Function name __FUNCTION__ : %s\n", __FUNCTION__);
        printf("Function name __func__ : %s\n", static_cast<const char *>(__func__));
#ifdef _MSC_VER
        printf("Decorated function name __FUNCDNAME__ : %s\n", __FUNCDNAME__);
#endif
        // GCC : __PRETTY_FUNCTION__ ; MSVC : __FUNCSIG__
        printf("Function signature Q_FUNC_INFO: %s\n", Q_FUNC_INFO);
        qt_noop();
    }

    /* Formatting with std::format() */
#if defined(__cpp_lib_format) && __cpp_lib_format >= 201907
    {
        qInfo().nospace()
            << "\n\n=================="
            << "\n    std::format   "
            << "\n==================\n\n";

        int num = 100;
        std::string s = "hello";

        printf("before: %i\n", num);
//        printf("before: %f\n", num);
        printf("before: %s\n", s.c_str());

        std::string m1 = std::format("The answer is {}.", 42);
        std::string m2 = std::format("The answer is {}.", s);

        qDebug() << QString::fromStdString(m1);
        qDebug() << QString::fromStdString(m2);

        qt_noop();
    }
#endif

    /* TinyORM and TinyOrmPlayground version info */
    {
        qInfo() << "\n\nTinyORM and TinyOrmPlayground version info\n---\n";

        qInfo() << "TINYORM_VERSION :" << TINYORM_VERSION;
        qInfo() << "TINYORM_VERSION_STR :" << TINYORM_VERSION_STR;
        qInfo() << "TINYORM_VERSION_STR_2 :" << TINYORM_VERSION_STR_2;
        qInfo() << "TINYORM_FILEVERSION_STR :" << TINYORM_FILEVERSION_STR;

        qInfo() << "TINYPLAY_VERSION :" << TINYPLAY_VERSION;
        qInfo() << "TINYPLAY_VERSION_STR :" << TINYPLAY_VERSION_STR;
        qInfo() << "TINYPLAY_VERSION_STR_2 :" << TINYPLAY_VERSION_STR_2;
        qInfo() << "TINYPLAY_FILEVERSION_STR :" << TINYPLAY_FILEVERSION_STR;
    }

    /* TinyORM constants */
    {
        qInfo() << "\n\nTinyORM constants\n---\n";

        qInfo() << "COLON (QChar)      :" << COLON;
        qInfo() << "DOT (QChar)        :" << DOT;
        qInfo() << "UNDERSCORE (QChar) :" << UNDERSCORE;
        qInfo() << "ASTERISK (QString) :" << ASTERISK;
        qInfo() << "COMMA (QString)    :" << COMMA;
        qInfo() << "INNER (QString)    :" << INNER;
    }

    logQueryCounters(timer.elapsed());
}

} // namespace TinyPlay::Tests
