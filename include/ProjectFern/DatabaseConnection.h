#include "RemoteUser.h"
#include "PrimaryClient.h"

class DatabaseConnection {
private:
    static std::mutex mtx;
public:
    static void startup();
    static void addUserToDB(RemoteUser* user);
    static void getUsersFromDB();

    static void addServerToDB(Server *server);
    static void getServersFromDB();

    static void addTextChannelToDB(Server *server, TextChannel *channel);
    static void getTextChannelsFromDB(Server *server);

    static void addMessageToDB(TextChannel *channel, MessageContainer *message);
    static void getMessagesFromDB(TextChannel *channel, int amount);

    static MessageContainer* getMessageFromDB(std::string id);

    static void addUserToServerDB(RemoteUser * user, Server * server);

    static void getUsersInServerFromDB(Server *server);

    static void addServerInvitationToDB(std::string invitation, Server *server);

    static std::string getInvitationsServerFromDB(std::string invitationCode);

    DatabaseConnection() = delete;
};