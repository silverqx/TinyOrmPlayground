#include "tests/testallothertests.hpp"

#include <QDebug>
#include <QStandardPaths>

#if defined(__cpp_lib_format) && __cpp_lib_format >= 201907
#  include <format>
#endif
#include <fstream>
#include <iomanip>
#include <iostream>
#ifdef __has_include
#  if __has_include(<version>)
#    include <version>
#  endif
#endif

#include <nlohmann/json.hpp>

#include <orm/db.hpp>

#include "configuration.hpp"

using json = nlohmann::json;

namespace TinyPlay::Tests
{

void TestAllOtherTests::run() const
{
    jsonConfig();
    standardPaths();
//    ctorAggregate();
}

void TestAllOtherTests::jsonConfig() const
{
    qInfo().nospace()
            << "\n\n=================="
            << "\n  nlohmann::json  "
            << "\n==================\n\n";

//    {
//        auto j = R"(
//{
//    "default": "mysql",

//    "connections": {

//        "sqlite": {
//            "driver": "sqlite",
//            "url": "",
//            "database": "database.sqlite",
//            "prefix": "",
//            "foreign_key_constraints": true
//        },
//        "mysql": {
//            "driver": "mysql",
//            "url": "xx",
//            "database": "database.mysql",
//            "prefix": ""
//        }
//    }
//}
//)"_json;

//        std::string s = j.dump();

//        std::cout << j.dump(4) << std::endl;
//    }

    /* Parse JSON from const char * */
    {
        const auto *s = R"(
{
    "default": "mysql",

    "connections": {

        "sqlite": {
            "driver": "sqlite",
            "url": "",
            "database": "database.sqlite",
            "prefix": "",
            "foreign_key_constraints": true
        },
        "mysql": {
            "driver": "mysql",
            "url": "xx",
            "database": "database.mysql",
            "prefix": ""
        }
    }
}
)";

        auto j = json::parse(s);
//        std::string s = j.dump();

//        std::cout << j.dump(4) << std::endl;

//        std::cout << j["default"].get<std::string>();
//        std::cout << j["connections"]["mysql"]["database"].get<std::string>();

        std::cout << j["connections"]["mysql"]["database"];
        std::cout << j["/connections/mysql/database"_json_pointer];

        auto x = j["connections"]["mysql"]["database"];
        std::cout << j["/connections/mysql/database"_json_pointer];

//        qDebug() << QString::fromStdString(j["default"].get<std::string>());

//        for (auto &[key, value] : j.items()) {
//            std::cout << key << " : " << value << "\n";
//        }

        qt_noop();
    }

    /* Read a JSON file */
//    {
//        // copy resources/config_test.json to the build directory root
//        std::ifstream i("config_test.json");
//        if (!i.is_open())
//            throw std::runtime_error("failed to open config_test.json");

//        json j;
//        i >> j;

//        // write prettified JSON to another file
//        std::ofstream o("pretty.json");
//        o << std::setw(4) << j << std::endl;

//        qt_noop();
//    }

    std::cout << std::endl;
}

void TestAllOtherTests::standardPaths() const
{
    // Exclude from prod. builds for better performance measurement accuracy
    if (!Configuration::isDebugBuild())
        return;

    qInfo().nospace()
            << "\n\n=================="
            << "\n  QStandardPaths  "
            << "\n==================\n\n";

    qDebug() << QStandardPaths::displayName(QStandardPaths::AppDataLocation);
    qDebug() << QStandardPaths::displayName(QStandardPaths::AppLocalDataLocation);
    qDebug() << QStandardPaths::displayName(QStandardPaths::ApplicationsLocation);

    qDebug() << QStandardPaths::findExecutable("php");

    qDebug() << "\nAppDataLocation:";
    qDebug() << QStandardPaths::standardLocations(QStandardPaths::AppDataLocation);
    qDebug() << QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    qDebug() << "\nAppLocalDataLocation:";
    qDebug() << QStandardPaths::standardLocations(QStandardPaths::AppLocalDataLocation);
    qDebug() << QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);

    qDebug() << "\nAppConfigLocation:";
    qDebug() << QStandardPaths::standardLocations(QStandardPaths::AppConfigLocation);
    qDebug() << QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);

    qDebug() << "\nGenericConfigLocation:";
    qDebug() << QStandardPaths::standardLocations(QStandardPaths::GenericConfigLocation);
    qDebug() << QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation);

    qDebug() << "\nGenericDataLocation:";
    qDebug() << QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation);
    qDebug() << QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);

    qDebug() << "\nGenericCacheLocation:";
    qDebug() << QStandardPaths::standardLocations(QStandardPaths::GenericCacheLocation);
    qDebug() << QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation);

    qDebug() << "\nCacheLocation:";
    qDebug() << QStandardPaths::standardLocations(QStandardPaths::CacheLocation);
    qDebug() << QStandardPaths::writableLocation(QStandardPaths::CacheLocation);

    qDebug() << "\nTempLocation:";
    qDebug() << QStandardPaths::standardLocations(QStandardPaths::TempLocation);
    qDebug() << QStandardPaths::writableLocation(QStandardPaths::TempLocation);

    qt_noop();
}

//namespace
//{
//    struct AggTest
//    {
//        QString name;
//        int value;

////        OtherTest toOther()
////        {
////            return {name, value};
////        }
//    };

//    struct AggTest;

//    struct OtherTest
//    {
//        QString name;
//        int value;
//        QString comparsion {};

//        explicit OtherTest() = default;
////        explicit OtherTest(AggTest &a);

////        OtherTest &operator=(const AggTest &agg)
////        {
////            name = agg.name;
////            value = agg.value;
////            return *this;
////        }
//    };

//    struct AggTest
//    {
//        QString name;
//        int value;

//        AggTest() = default;
//        AggTest(OtherTest &o)
//        {
//            name = o.name;
//            value = o.value;
//        }

//        operator OtherTest() const
//        {
//            qDebug() << "operator OtherTest()";
//            return {name, value};
//        }

//        OtherTest toOther()
//        {
//            return {name, value};
//        }
//    };
//} // namespace

void TestAllOtherTests::ctorAggregate() const
{
//    {
//        AggTest a {.name = "xx", .value = 1};
//        AggTest a;
//        a.name = "xx";
//        a.value = 1;
//        OtherTest ot {.name = "zz", .value = 10};
//        OtherTest ot;
//        ot.name = "zz";
//        ot.value = 10;
//        OtherTest o = a.toOther();

//        a.name = std::move(ot.name);
//        ot = std::move(a);

//        OtherTest o = a;

//        std::vector<AggTest> va {{"x", 1}, {"y", 2}, {"z", 3}};
//        std::vector<OtherTest> vo;

//        //        std::vector<OtherTest> vo {{"a", 10}, {"b", 20}, {"c", 30}};
//        //        std::copy(va.begin(), va.end(), std::back_inserter(vo));
//        std::move(va.begin(), va.end(), std::back_inserter(vo));
//        ranges::move(std::move(va), ranges::back_inserter(vo));
//        qt_noop();
//    }
}


} // namespace TinyPlay::Tests
