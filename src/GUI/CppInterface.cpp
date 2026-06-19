#include "CppInterface.h"


CppInterface* CppInterface::instancePtr = nullptr;

CppInterface* CppInterface::getInstance() {
    int typeId = qmlTypeId("MyFoo", 1, 0, "Foo");
    QQmlEngine engine;
    CppInterface *singleton = engine.singletonInstance<CppInterface *>(typeId);
    CppInterface::instancePtr = singleton;
    return instancePtr;
}

void CppInterface::test() {
    qDebug(qUtf8Printable("AAAAA"));
}
void CppInterface::sendMessage(QString Message, QObject* server, QObject* channel) {
    qDebug(qUtf8Printable(Message));
    std::string a = server->property("uuid").toString().toStdString();
    addServer(NULL);
}


void CppInterface::addServer(Server* server) {

    emit serverAdd("Shoot");
    //STD STring Crashes TODO later problem
}