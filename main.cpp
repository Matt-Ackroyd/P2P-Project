#include <QApplication>
#include <QPushButton>
#include <QQmlApplicationEngine>
#include <QQuickView>
#include "CppInterface.h"


void test() {
    qDebug() << "pressed";
}

int main(int argc, char *argv[])
{
    qmlRegisterType<CppInterface>("MyFoo", 1, 0, "Foo");

    QApplication app(argc, argv);

    QQmlApplicationEngine engine;
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("project_fern", "Main");

    return QGuiApplication::exec();
}
