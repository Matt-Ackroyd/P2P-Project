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

// loads a server from file
void PrimaryClient::loadServer(std::string id) {

    Server* server = new Server(id);

    server->loadAllChannels();
    
    addNewServer(server);
}
//loads all servers
void PrimaryClient::loadAllServers() {
    std::filesystem::path path(SERVER_PATH);

    for (const auto & entry : std::filesystem::directory_iterator(path))
        loadServer(entry.path().filename().string());
}

// Makes a brand new server 
void PrimaryClient::createNewServer(std::string id) {
    std::filesystem::path path(SERVER_PATH + id + "/");
    std::filesystem::path settingsPath(path.string() + "serverSettings.txt");
    std::filesystem::path rolesPath(path.string() + "serverRoles.txt");
    std::filesystem::path invitationsPath(path.string() + "serverInvitations.txt");
    std::filesystem::path membersPath(path.string() + "memberList.txt");
    std::filesystem::path voicePath(path.string() + "voiceChannels.txt");
    std::filesystem::path channelPath(path.string() + "TextChannels/");

    // Attempt to create the path to this server if it doesn't exist yet
    if (!std::filesystem::exists(path)) {
        if (!std::filesystem::create_directories(path)) {
            return;
        }
    }
    
    // Attempt to make the Channel folder
    if (!std::filesystem::exists(channelPath)) {
        if (!std::filesystem::create_directories(channelPath)) {
            return;
        }
    }

    // Base Text For each File;
    std::string memberString = "#(ID) RelayIP RelayPort RequiresRelay(1/0) Roles\n";
    std::string settingsString = "#IDK YET\n";
    std::string roleString = "#Role-Name perms . . .\n";
    std::string voiceString = "#UUID channelsettings\n";


    // Make the Rest of the files with their default text
    std::ofstream memberFile(membersPath);
    if (!memberFile.is_open()) {
        return;
    }
    memberFile << memberString;
    memberFile.close();

    // Make the Rest of the files with their default text
    std::ofstream settingsFile(settingsPath);
    if (!settingsFile.is_open()) {
        return;
    }
    settingsFile << settingsString;
    settingsFile.close();

    // Make the Rest of the files with their default text
    std::ofstream roleFile(rolesPath);
    if (!roleFile.is_open()) {
        return;
    }
    roleFile << roleString;
    roleFile.close();
    
    // Make the Rest of the files with their default text
    std::ofstream voiceFile(voicePath);
    if (!voiceFile.is_open()) {
        return;
    }
    voiceFile << voiceString;
    voiceFile.close();
}



sockaddr_in PrimaryClient::getPreferedRelay() {
    return this->preferedRelayAdress;
}

OutgoingHandler* PrimaryClient::getOutgoingHandler() {
    return this->outgoingHandler;
}