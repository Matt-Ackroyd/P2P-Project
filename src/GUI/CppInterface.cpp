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

    MessageContainer* message = new MessageContainer(DataTypes::MESSAGETYPE, *server->getID(), *channel->getID(), *client->getClientID(), text);

    GUIloadMessage(message);
    channel->sendMessage(message);

}

#include "RelayClient.h"
void CppInterface::requestServerInfo(QString Qid) {
    PrimaryClient* client = PrimaryClient::getInstance();
    std::string id = Qid.toStdString();

    Server* server = client->getServer(id);

    // Loop over all known channels
    for (auto& [key, channel]: server->knownChannels) { 
        GUIloadChannel(channel);
    }
    
}

void CppInterface::requestChannelInfo(QObject* qserver, QObject* qchannel) {
    PrimaryClient* client = PrimaryClient::getInstance();
    std::string serverid = qserver->property("uuid").toString().toStdString();
    std::string channelid = qchannel->property("uuid").toString().toStdString();

    Server* server = client->getServer(serverid);
    TextChannel* channel = server->knownChannels[channelid];

    this->currentChannel = channelid;

    for (auto& message: channel->messages) { 
        GUIloadMessage(message);
    }
}

// Q_INVOKABLE 
void CppInterface::createNewServer() {
    PrimaryClient::getInstance()->createNewServer();
}
// Q_INVOKABLE 
void CppInterface::createNewTextChannel(QString serverid) {
    PrimaryClient::getInstance()->getServer(serverid.toStdString())->createNewTextChannel();
}

Q_INVOKABLE void CppInterface::joinServer(QString addr, QString port, QString id, QString inviation) {
    int contactAdress = inet_addr(addr.toUtf8());;
    short int contactPort = htons(port.toShort());
    std::string userid = id.toStdString();
    std::string serverInviation = inviation.toStdString();


    // Get/Make the user asosiated with this invitation code
    PrimaryClient* client = PrimaryClient::getInstance();
    RemoteUser *serverAccsess = PrimaryClient::getInstance()->getUser(userid);
    if (serverAccsess == NULL) {
        if (!PrimaryClient::getInstance()->registerNewUser(userid)) {
            return;
        }
        serverAccsess = PrimaryClient::getInstance()->getUser(userid);
    }

    // if the user doesn't have a shared secret with us yet schendule a handshake
    if (serverAccsess->connection.getSharedSecret() == nullptr) {
        serverAccsess->connection.requestHandshakeOnceConnected = true;
    }

    serverAccsess->connection.bufferedServerInvitation = serverInviation;

    RelayClient::UserConnectionInfoReqest(PrimaryClient::getInstance()->socketfd, addr.toStdString(), port.toShort(), userid, PrimaryClient::getInstance()->getClientID());
}

Q_INVOKABLE void CppInterface::createServerInvitation(QString serverid)
{
    PrimaryClient::getInstance()->getServer(serverid.toStdString())->createNewInvitation();
}

// C++ side interface to add a server to the GUI
void CppInterface::GUIloadServer(Server* server) {
    QString id = QString::fromStdString(*server->getID());
    
    emit serverLoad(id);
}

// C++ Side Interface to load a channel into the current server on the GUI
void CppInterface::GUIloadChannel(TextChannel* channel) {
    QString id = QString::fromStdString(*channel->getID());
    
    emit channelLoad(id);
}

// C++ side interface to load a message into the current channel on the GUI
void CppInterface::GUIloadMessage(MessageContainer* message) {
    RemoteUser* sender = PrimaryClient::getInstance()->getUser(*message->getAuthor());
    QString authorName;
    if (sender == nullptr) {
        authorName = "Unknown";
    } else {
        authorName = QString::fromStdString(sender->Username);
    }
   
    
    QString channel_id = QString::fromStdString(*message->getChannelID());
    QString message_text = QString::fromStdString(message->getMessage());

    if (*message->getChannelID() == this->currentChannel) {
        emit messageLoad(channel_id, message_text, authorName, "");
    }
}