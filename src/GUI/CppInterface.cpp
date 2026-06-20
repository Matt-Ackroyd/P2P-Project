#include "CppInterface.h"


CppInterface* CppInterface::instancePtr = nullptr;

CppInterface* CppInterface::getInstance() {
    return instancePtr;
}

void CppInterface::test() {
    qDebug(qUtf8Printable("AAAAA"));
}
void CppInterface::sendMessage(QString qmessage, QObject* qserver, QObject* qchannel) {
    PrimaryClient* client = PrimaryClient::getInstance();
    std::string serverid = qserver->property("uuid").toString().toStdString();
    std::string channelid = qchannel->property("uuid").toString().toStdString();
    std::string text = qmessage.toStdString();

    Server* server = client->getServer(serverid);
    TextChannel* channel = server->knownChannels[channelid];

    MessageContainer* message = new MessageContainer();
    int len = message->createNew(server->getID(), channel->getID(), client->getClientID(), text);

    channel->messages.emplace_back(message);
    loadMessage(message);

    // REPLACE WITH CHANNEL SPECIFIC RECIPIENTS
    for (auto& [key, recipient]: client->knownConnections) {
        unsigned char data[len];
        message->serialize(data);
        recipient->connection->send(data, len);
    }

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

void CppInterface::requestChannelInfo(QObject* qserver, QObject* qchannel) {
    PrimaryClient* client = PrimaryClient::getInstance();
    std::string serverid = qserver->property("uuid").toString().toStdString();
    std::string channelid = qchannel->property("uuid").toString().toStdString();

    Server* server = client->getServer(serverid);
    TextChannel* channel = server->knownChannels[channelid];

    for (auto& message: channel->messages) { 
        loadMessage(message);
    }
}



// C++ side interface to add a server to the GUI
void CppInterface::loadServer(Server* server) {
    QString id = QString::fromStdString(server->getID()->getString());
    
    emit serverLoad(id);
}

// C++ Side Interface to load a channel into the current server on the GUI
void CppInterface::loadChannel(TextChannel* channel) {
    QString id = QString::fromStdString(channel->getID()->getString());
    
    emit channelLoad(id);
}

// C++ side interface to load a message into the current channel on the GUI
void CppInterface::loadMessage(MessageContainer* message) {
    QString id = QString::fromStdString(message->getMessageID()->getString());
    QString message_text = QString::fromStdString(message->getMessage());

    emit messageLoad(id, message_text);
}