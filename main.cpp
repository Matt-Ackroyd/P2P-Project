#include <QApplication>
#include <QPushButton>
#include <QQmlApplicationEngine>
#include <QQuickView>
#include "CppInterface.h"
#include "PrimaryClient.h"


void test() {
    qDebug() << "pressed";
}

int main(int argc, char *argv[])
{
    //qmlRegisterType<CppInterface>("", "CppInterface");

    QApplication app(argc, argv);

    QQmlApplicationEngine engine;
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("project_fern", "Main");

    IncomingHandler a;

    CppInterface::instancePtr = engine.singletonInstance<CppInterface*>("project_fern", "CppInterface");
    //a.enableIncomingTraffic(5000);

    PrimaryClient* client = PrimaryClient::getInstance();
    
    Server* test = new Server();
    client->addNewServer("server id placeholder", test);

    return QGuiApplication::exec();
}
