#include "RemoteUser.h"
#include "PrimaryClient.h"

class DatabaseConnection {
private:
    static std::mutex mtx;
    static sqlite3* db;
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

    static void addUserToServerDB(RemoteUser * user, Server * server);

    DatabaseConnection() = delete;
};