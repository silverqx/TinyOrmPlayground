#ifndef TESTORM_H
#define TESTORM_H

#include "orm/databasemanager.hpp"

class TestOrm
{
public:
//    using KeyType = quint64;

    TestOrm &connectToDatabase();
    TestOrm &run();

private:
    void anotherTests();
    void testTinyOrm();
    void testQueryBuilder();
    void ctorAggregate();
    void jsonConfig();
    void standardPaths();

    std::unique_ptr<Orm::DatabaseManager> m_dm;
};

//struct AggTest
//{
//    QString name;
//    int value;

////    OtherTest toOther()
////    {
////        return {name, value};
////    }
//};

//struct AggTest;

//struct OtherTest
//{
//    QString name;
//    int value;
//    QString comparsion {};

//    explicit OtherTest() = default;
////    explicit OtherTest(AggTest &a);

////    OtherTest &operator=(const AggTest &agg)
////    {
////        name = agg.name;
////        value = agg.value;
////        return *this;
////    }
//};

//struct AggTest
//{
//    QString name;
//    int value;

//    AggTest() = default;
//    AggTest(OtherTest &o)
//    {
//        name = o.name;
//        value = o.value;
//    }

//    operator OtherTest() const
//    {
//        qDebug() << "operator OtherTest()";
//        return {name, value};
//    }

//    OtherTest toOther()
//    {
//        return {name, value};
//    }
//};

//class MyClass : public QObject
//{
//    Q_OBJECT
////    Q_PROPERTY(Priority priority READ priority WRITE setPriority)
//    Q_PROPERTY(Priority priority READ priority WRITE setPriority NOTIFY priorityChanged)

//public:
//    MyClass(QObject *parent = nullptr) {};
//    ~MyClass() = default;

//    enum Priority { High, Low, VeryHigh, VeryLow };
//    Q_ENUM(Priority)

//    void setPriority(Priority priority)
//    {
//        m_priority = priority;
//        emit priorityChanged(priority);
//    }
//    Priority priority() const
//    { return m_priority; }

//signals:
//    void priorityChanged(Priority);

//private:
//    Priority m_priority;
//};

#endif // TESTORM_H
