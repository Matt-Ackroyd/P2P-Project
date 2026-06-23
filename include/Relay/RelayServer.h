#include "RelayTypes.h"
#include "ID.h"

// Adds a user to the data base with a hash of the salted password
// Requires a Encrypted Connection Beforehand 
void onRegisterUser(ID userID, std::string password);

// Updates the stored connection info of a given user, as long as the password hash works
// Requires a Encrypted Connection Beforehand 
void onUpdateUserConnectionInfo(ID userID, std::string password, bool status);

// returns a users connection info
// Does not require an Encrypted Connection
void onUserConnectionInfoReqest(ID userID);


void handleIncomingRequest();