#include "CppInterface.h"


CppInterface* CppInterface::instancePtr = nullptr;

CppInterface* CppInterface::getInstance() {
    return instancePtr;
}

void CppInterface::test() {
    qDebug(qUtf8Printable("AAAAA"));
}
void CppInterface::sendMessage(QString Message, QObject* server, QObject* channel) {
    qDebug(qUtf8Printable(Message));
    std::string a = server->property("uuid").toString().toStdString();
    qDebug(qUtf8Printable(QString::fromStdString(a)));
}

void CppInterface::requestServerInfo(QString Qid) {
    PrimaryClient* client = PrimaryClient::getInstance();
    std::string id = Qid.toStdString();

    Server* server = client->getServer(id);

    // Loop over all known channels
    for (auto& [key, channel]: server->knownChannels) { 
        loadChannel(channel);
    }
    
}









// C++ side interface to add a server to the GUI
void CppInterface::loadServer(Server* server) {
    QString id = QString::fromStdString(server->getID());
    
    emit serverLoad(id);
}

// C++ Side Interface to load a channel into the current server on the GUI
void CppInterface::loadChannel(TextChannel* channel) {
    QString id = QString::fromStdString(channel->getID());
    
    emit channelLoad(id);
}