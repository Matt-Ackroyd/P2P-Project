#include "RemoteUser.h"
#include "PrimaryClient.h"

class DatabaseConnection {
public:
    static void sqlitetest();
    static void addUserToDB(RemoteUser* user);
    static void getUsersFromDB();

    void addServerToDB(Server *server);
    void getServersFromDB();

    void addTextChannelToDB(Server *server, TextChannel *channel);
    void getTextChannelsFromDB(Server *server);

    void addMessageToDB(TextChannel *channel, MessageContainer *message);
    void getMessagesFromDB(TextChannel *channel, int amount);

    void addUserToServerDB(RemoteUser * user, Server * server);

    DatabaseConnection() = delete;
};