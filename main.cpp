#include <QApplication>
#include <QPushButton>
#include <QQmlApplicationEngine>
#include <QQuickView>
#include "CppInterface.h"
#include "PrimaryClient.h"
#include "RelayClient.h"
#include "OutgoingHandler.h"
#include "DatabaseConnection.h"
#include "Sync.h"

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
    
    // Server* test = new Server();
    // client->loadServer(test);

    // TextChannel* t1 = new TextChannel();
    // TextChannel* t2 = new TextChannel();
    // TextChannel* t3 = new TextChannel();
    // test->knownChannels[t1->getID()] = t1;
    // test->knownChannels[t2->getID()] = t2;
    // test->knownChannels[t3->getID()] = t3;

    // Server* test2 = new Server();
    // client->loadServer(test2);

    // TextChannel* t4 = new TextChannel();
    // test2->knownChannels[t4->getID()] = t4;

    // SyncRequest asdasd = SyncRequest("1111111111111111", DataTypes::MESSAGETYPE, 0, 100);

    // SyncRequest aasasddfgdg = SyncRequest::deserialize(asdasd.getData());

    unsigned char buffer[ML_DSA_87_SIGNATURE_BYTE_SIZE];
    signMessage(client->getDSAkey(), (unsigned char*)"Hia", 3, buffer);
    int ret = verifyMessage(client->getDSAkey(), buffer, (unsigned char*)"Hia", 3);
    

    DatabaseConnection::startup();
    DatabaseConnection::getUsersFromDB();
    DatabaseConnection::getServersFromDB();
    RelayClient::RegisterUser("192.168.0.17", 7777);

    for (auto& [userid, user]: client->knownConnections) {
        if (userid != *client->getClientID()) {
            sockaddr_in a;
            a.sin_addr.s_addr = user->contactAddress;
            RelayClient::UserConnectionInfoRequest(client->socketfd, inet_ntoa(a.sin_addr), ntohs(user->contactPort), *user->getID(), client->getClientID());
        }
    }
    // PrimaryClient::getInstance()->registerNewUser(&larryid);
    // RemoteUser* larry = PrimaryClient::getInstance()->getUser(larryid);
    
    
    // larry.connection.setAddr("68.146.39.61", 1024);
    // larry.connection.sendKeepAlive();
    // larry.connection.sendHandshakeRequest();

    // client->getOutgoingHandler()->enableConnection(&larry);

    
    //aasda->createNewServer();

    

    

    //int a = ntohs(connectionAddr.sin_port);
    //char *ip = inet_ntoa(connectionAddr.sin_addr);


    return QGuiApplication::exec();
}
