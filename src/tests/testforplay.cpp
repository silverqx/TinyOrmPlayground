#include "tests/testforplay.hpp"

//#include <QObject>
#include <QDebug>
#include <QElapsedTimer>
//#include <QHostInfo>

#include <filesystem> // IWYU pragma: keep
#include <iostream> // IWYU pragma: keep
#include <typeindex> // IWYU pragma: keep
#include <typeinfo> // IWYU pragma: keep

#include <range/v3/all.hpp>

#include <orm/db.hpp>
//#include <orm/postgresconnection.hpp>
#include <orm/schema.hpp>
#include <orm/tiny/tinytypes.hpp>
#include <orm/utils/helpers.hpp>
#include <orm/utils/query.hpp>
#include <orm/version.hpp>

#include "models/album.hpp" // IWYU pragma: keep
#include "models/datetime.hpp" // IWYU pragma: keep
#include "models/torrent.hpp" // IWYU pragma: keep
#include "models/torrenteager.hpp" // IWYU pragma: keep

#include "configuration.hpp" // IWYU pragma: keep
#include "version.hpp"

//using namespace std::chrono_literals;

// BUG this causes crash of code model after upgrade to QtCreator 5 with clang 12 silverqx
using namespace ranges; // NOLINT(google-build-using-namespace)

//using namespace Orm::Support::TimerCategories; // NOLINT(google-build-using-namespace)

using Orm::DB;                            // NOLINT(misc-unused-using-decls)
using Orm::Query::Builder;                // NOLINT(misc-unused-using-decls)
using Orm::Schema;                        // NOLINT(misc-unused-using-decls)
using Orm::SchemaNs::Blueprint;           // NOLINT(misc-unused-using-decls)
//using Orm::Support::Timer;              // NOLINT(misc-unused-using-decls)
using Orm::Tiny::AttributeItem;           // NOLINT(misc-unused-using-decls)
using Orm::Tiny::Types::ModelsCollection; // NOLINT(misc-unused-using-decls)
using Orm::Utils::Helpers;                // NOLINT(misc-unused-using-decls)
using Orm::Utils::NullVariant;            // NOLINT(misc-unused-using-decls)

using ContainerUtils = Orm::Utils::Container; // NOLINT(misc-unused-using-decls)
using QueryUtils     = Orm::Utils::Query;     // NOLINT(misc-unused-using-decls)
using StringUtils    = Orm::Utils::String;    // NOLINT(misc-unused-using-decls)

using AttributeUtils = Orm::Tiny::Utils::Attribute; // NOLINT(misc-unused-using-decls)

namespace TinyPlay::Tests
{

void TestForPlay::run() const
{
    using namespace Orm::Constants; // NOLINT(google-build-using-namespace)
    using namespace Models;         // NOLINT(google-build-using-namespace)

    QElapsedTimer timer;
    timer.start();

    resetAllQueryLogCounters();

    qInfo().nospace()
            << "================="
            << "\n  Test for Play  "
            << "\n=================\n";

    /*! Local connection override, helps to switch connection in the test code. */
//    const auto &LocalConnection = Mysql;
//    const auto &LocalConnection = Postgres;

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
        QList<AttributeItem> attributes {
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
        // GCC : __PRETTY_FUNCTION__ ; MSVC : __FUNCSIG__
        printf("Function signature Q_FUNC_INFO: %s\n", Q_FUNC_INFO);
#ifdef _MSC_VER
        printf("Decorated function name __FUNCDNAME__ : %s\n", __FUNCDNAME__);
#elif defined(__GNUG__) || defined(__clang__)
        printf("Function name __PRETTY_FUNCTION__ : %s\n", __PRETTY_FUNCTION__);
#endif

        qt_noop();
    }

    /* Formatting with std::format() */
#if defined(__cpp_lib_format) && __cpp_lib_format >= 201907
    {
        qInfo().nospace()
            << "\n\n=================="
            << "\n    std::format   "
            << "\n==================\n\n";

        const int num = 100;
        std::string s = "hello";

        printf("before: %i\n", num);
//        printf("before: %f\n", num);
        printf("before: %s\n", s.c_str());

        const std::string m1 = std::format("The answer is {}.", 42);
        const std::string m2 = std::format("The answer is {}.", s);

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
