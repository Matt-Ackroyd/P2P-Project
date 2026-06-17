#include <QApplication>
#include <QPushButton>
#include <QQmlApplicationEngine>
#include <QQuickView>


void test() {
    qDebug() << "pressed";
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QQmlApplicationEngine engine;
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("untitled", "Main");


    QObject* rootObject = engine.rootObjects().first();
    QObject* button = rootObject->findChild<QObject*>("rect");
    //QObject::connect(button,&QPushButton::clicked,test);


    return QGuiApplication::exec();
}
