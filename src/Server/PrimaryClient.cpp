#include "PrimaryClient.h"
#include "IncomingHandler.h"
#include "OutgoingHandler.h"
#include "Server.h"

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
    // Load from file later

    

    std::filesystem::path uuidPath("Configs/PrimaryClient/uuid");
    if (std::filesystem::exists(uuidPath)) {
        char uuid[UUID_BYTE_SIZE];
        ConfigLoader::getInstance()->ReadBinaryFile("Configs/PrimaryClient/uuid", uuid, UUID_BYTE_SIZE);
        this->clientID = ID::fromBytes((unsigned char*)uuid);
    } else {
        ConfigLoader::getInstance()->WriteBinaryFile("Configs/PrimaryClient/uuid", (char*)this->clientID.getRaw(), UUID_BYTE_SIZE);
    }
    
    std::cout << "Your ID: " << this->clientID.getString() << "\n";
    
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
    this->preferedRelayAdress.sin_port = htons(7777);
    this->preferedRelayAdress.sin_addr.s_addr = inet_addr("68.146.39.61");

    this->incomingHandler = new IncomingHandler(10346);
    this->outgoingHandler = new OutgoingHandler();
   
    return 0;
}

EVP_PKEY* PrimaryClient::getKeyPair() {
    // TEMP TODO Save KEYPAIR
    if (this->keyPair == NULL) {
        this->keyPair = EVP_PKEY_Q_keygen(NULL, NULL, "ML-KEM-1024");
    }
    return this->keyPair;
}

ID* PrimaryClient::getClientID() {
    return &this->clientID;
}


int PrimaryClient::registerNewUser(ID* id) {
    // Guard clause to not add oneself as a new user
    // if (id->getString() == this->clientID.getString()) {
    //     std::cout << "Cannot Register Yourself\n";
    //     return 0;
    // }
    
    // Guard Clause to not overwrite a user
    if (this->knownConnections.contains(id->getString())) {
        std::cout << "User " << id->getString() << " already Exists\n";
        return 0;
    }

    // Create a temperary user to asosiate incoming packets from this user will be lost on reset if not proporly added to a server
    RemoteUser* test = new RemoteUser(id);
    // TODO link remote user connection  

    // add to the list of all known connections
    std::cout << "New User Added: " << id->getString() << " \n";
    this->knownConnections[id->getString()] = test;
    return 1;  // return sucsess 
}

RemoteUser* PrimaryClient::getUser(std::string userID) {
    // Make sure the user exists
    if (knownConnections.contains(userID) == 0) {
        return NULL;
    }

    return this->knownConnections[userID];
}


void PrimaryClient::addNewServer(Server *server) {
    this->allServers[server->getID()->getString()] = server;
    CppInterface::instancePtr->loadServer(server);
}

Server* PrimaryClient::getServer(std::string id) {
    // make sure the use exists
    if (!allServers.contains(id)) {
        return NULL;
    }

    return this->allServers[id];
}

sockaddr_in PrimaryClient::getPreferedRelay() {
    return this->preferedRelayAdress;
}

OutgoingHandler* PrimaryClient::getOutgoingHandler() {
    return this->outgoingHandler;
}