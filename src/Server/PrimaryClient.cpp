#include "PrimaryClient.h"
#include "IncomingHandler.h"
#include "OutgoingHandler.h"
#include "FileHandler.h"
#include "Server.h"
#include "DatabaseConnection.h"

PrimaryClient* PrimaryClient::instancePtr = nullptr;
std::mutex PrimaryClient::mtx;

// Static method to get the Singleton instance
PrimaryClient* PrimaryClient::getInstance() {
    if (instancePtr == nullptr) {
        std::lock_guard<std::mutex> lock(mtx);
        if (instancePtr == nullptr) {
            instancePtr = new PrimaryClient();
            instancePtr->init();
        }
    }
    return instancePtr;
}

PrimaryClient::~PrimaryClient() {
    delete this->incomingHandler;
    delete this->outgoingHandler;
}

int PrimaryClient::init() {
    // Innital Values 
    this->keyPair = NULL;
    this->ML_DSAkey = getDSAkeyFromFile();
    // Load from file later

    

    std::filesystem::path uuidPath("Configs/PrimaryClient/uuid");
    if (std::filesystem::exists(uuidPath)) {
        char uuid[UUID_BYTE_SIZE];
        ConfigLoader::getInstance()->ReadBinaryFile("Configs/PrimaryClient/uuid", uuid, UUID_BYTE_SIZE);
        this->clientID = ID::stringFromBytes((unsigned char*)uuid);
    } else {
        char uuid[UUID_BYTE_SIZE];
        ID::BytesFromString(ID::GenerateNewID(), (unsigned char*)uuid);
        ConfigLoader::getInstance()->WriteBinaryFile("Configs/PrimaryClient/uuid", uuid, UUID_BYTE_SIZE);
    }
    this->registerNewUser(this->clientID);
    this->thisUser = this->getUser(this->clientID);
    
    std::cout << "Your ID: " << this->clientID << "\n";
    
    // Socket Compatibility Stuff
    #ifdef _WIN32
        WSADATA wsaData;
        WSAStartup(MAKEWORD(2,2), &wsaData);
        int optVal = 1;
        setsockopt(this->socketfd, SOL_SOCKET, SO_REUSEADDR, (char*)&optVal, sizeof(optVal));
    #endif

    this->socketfd = socket(AF_INET, SOCK_DGRAM, 0);  


    //Temp Until we load from file
    this->preferedRelayAdress.sin_family = AF_INET;
    this->preferedRelayAdress.sin_port = htons(15777);
    this->preferedRelayAdress.sin_addr.s_addr = inet_addr("192.168.0.17");

    // TODO clean this up
    this->thisUser->contactAddress = preferedRelayAdress.sin_addr.s_addr;
    this->thisUser->contactPort = preferedRelayAdress.sin_port;

    this->incomingHandler = new IncomingHandler(10346);
    this->outgoingHandler = new OutgoingHandler();
    this->fileHandler = new FileHandler();
   
    return 0;
}

EVP_PKEY* PrimaryClient::getKeyPair() {
    // TEMP TODO Save KEYPAIR
    if (this->keyPair == NULL) {
        this->keyPair = EVP_PKEY_Q_keygen(NULL, NULL, "ML-KEM-1024");
    }
    return this->keyPair;
}

EVP_PKEY *PrimaryClient::getDSAkey()
{
    return this->ML_DSAkey;
}

std::string* PrimaryClient::getClientID() {
    return &this->clientID;
}


int PrimaryClient::registerNewUser(std::string id) {
    if (id == NULL_ID) {
        std::cout << "NULL ID\n";
        return 0;
    }
    // Guard Clause to not overwrite a user
    if (this->knownConnections.contains(id)) {
        std::cout << "User " << id << " already Exists\n";
        return 0;
    }

    // Create a temperary user to asosiate incoming packets from this user will be lost on reset if not proporly added to a server
    RemoteUser* test = new RemoteUser(id);
    // TODO link remote user connection  

    // add to the list of all known connections
    std::cout << "New User Added: " << id << " \n";
    this->knownConnections[id] = test;
    return 1;  // return sucsess 
}

void PrimaryClient::enableConnection(RemoteUser *user) {
    user->connection.connected = true;

    std::lock_guard<std::mutex> lock(mtx);
    if (!instancePtr->onlineConnections.contains(*user->getID())) {
        instancePtr->onlineConnections.emplace(*user->getID());
    }
}

void PrimaryClient::disableConnection(RemoteUser *user) {
    user->connection.connected = false;

    std::lock_guard<std::mutex> lock(mtx);
    if (instancePtr->onlineConnections.contains(*user->getID())) {
        instancePtr->onlineConnections.erase(*user->getID());
    }
}

RemoteUser* PrimaryClient::getUser(std::string userID) {
    // Make sure the user exists
    if (knownConnections.contains(userID) == 0) {
        return NULL;
    }

    return this->knownConnections[userID];
}


void PrimaryClient::loadServer(Server *server) {

    server->addNewUser(getUser(clientID));

    this->allServers[*server->getID()] = server;
    CppInterface::instancePtr->GUIloadServer(server);
}

void PrimaryClient::createNewServer(std::string id) {
    std::string serverid = ID::clean(id);
    Server* server = new Server(serverid);

    DatabaseConnection::addServerToDB(server);
    loadServer(server);
}

Server* PrimaryClient::getServer(std::string id) {
    // make sure the use exists
    if (!allServers.contains(id)) {
        return NULL;
    }

    return this->allServers[id];
}

RemoteUser *PrimaryClient::getThisUser()
{
    return this->thisUser;
}

void PrimaryClient::loadUser(RemoteUser* user) {
    if (this->knownConnections.contains(*user->getID())) {
        std::cout << "User " << user->getID() << " already Exists\n";
        delete user;
        return;
    }

    // add to the list of all known connections
    this->knownConnections[*user->getID()] = user;
}

// loads a server from file
void PrimaryClient::loadServer(std::string id) {

    Server* server = new Server(id);
    
    loadServer(server);
}
//loads all servers
void PrimaryClient::loadAllServers() {
    DatabaseConnection::getServersFromDB();
}


sockaddr_in PrimaryClient::getPreferedRelay() {
    return this->preferedRelayAdress;
}

OutgoingHandler* PrimaryClient::getOutgoingHandler() {
    return this->outgoingHandler;
}