#include <QApplication>
#include <QPushButton>
#include <QQmlApplicationEngine>
#include <QQuickView>
#include "CppInterface.h"
#include "PrimaryClient.h"
#include "RelayClient.h"
#include "OutgoingHandler.h"

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


    CppInterface::instancePtr = engine.singletonInstance<CppInterface*>("project_fern", "CppInterface");

    PrimaryClient* client = PrimaryClient::getInstance();
    
    Server* test = new Server();
    client->addNewServer(test);

    TextChannel* t1 = new TextChannel();
    TextChannel* t2 = new TextChannel();
    TextChannel* t3 = new TextChannel();
    test->knownChannels[t1->getID()->getString()] = t1;
    test->knownChannels[t2->getID()->getString()] = t2;
    test->knownChannels[t3->getID()->getString()] = t3;

    Server* test2 = new Server();
    client->addNewServer(test2);

    TextChannel* t4 = new TextChannel();
    test2->knownChannels[t4->getID()->getString()] = t4;

    ID larryid;
    larryid.GenerateNewID();
    PrimaryClient::getInstance()->registerNewUser(&larryid);
    RemoteUser* larry = PrimaryClient::getInstance()->getUser(larryid.getString());
    larry->connection.setAddr("192.168.0.27", 10346);
    larry->connection.sendKeepAlive();
    larry->connection.sendHandshakeRequest();

    client->getOutgoingHandler()->enableConnection(larry);

    

    //int a = ntohs(connectionAddr.sin_port);
    //char *ip = inet_ntoa(connectionAddr.sin_addr);


    return QGuiApplication::exec();
}
