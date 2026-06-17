#pragma once
#include <QApplication>
#include <qqmlintegration.h>
#include <iostream>


class CppInterface : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    // ...

public:
    Q_INVOKABLE void test(QString message);
};


