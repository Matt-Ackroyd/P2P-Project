#include "ID.h"


// Requests this user to be added to the database with a hash of the salted password
// Requires a Encrypted Connection Beforehand 
void RegisterUser(std::string password);

// Requests to update the database with this users connection infomation
// Requires a Encrypted Connection Beforehand 
void UpdateUserConnectionInfo(std::string password, bool status);

// returns a given users connection info
// Does not require an Encrypted Connection
void UserConnectionInfoReqest(ID userID);