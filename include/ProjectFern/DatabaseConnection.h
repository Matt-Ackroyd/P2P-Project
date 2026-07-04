#include "RemoteUser.h"
#include "PrimaryClient.h"

class DatabaseConnection {
public:
    static void sqlitetest();
    static void addUserToDB(RemoteUser* user);
    static void getUsersFromDB();

    DatabaseConnection() = delete;
};