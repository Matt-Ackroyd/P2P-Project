#include "User.h"
#include <vector>

using namespace std;


class Server {
    vector<User> knownUsers;
    vector<User> onlineUsers;

    public: 
        int addUser(User user);
        int removeUser(User user);
};