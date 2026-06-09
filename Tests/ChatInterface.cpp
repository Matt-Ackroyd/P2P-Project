#include "UDPConnection.h"
#include "IncomingHandler.h"
#include "UUID.h"

using namespace std;

void ChatInterface() {
  
    UDPConnection *test = new UDPConnection(NULL);
    IncomingHandler a;
    string input;

    // Allow for testing on single machine
    cout << "ReceivingPort: ";
    int port;
    cin >> port;

    // int sendingPort;
    // cout << "SendingPort: ";
    // cin >> sendingPort;
    cin.ignore();

    //test->SendingPort = 5000;
    a.enableIncomingTraffic(port);

    // Setup Connections
    
    //test->SendingPort = sendingPort;
    test->sendConnectionRequest();

    cin.ignore();
    
    while (input != "exit") {
        // User input
        getline(cin, input);

        

        // Send User Input;
        unsigned char* message = (unsigned char*)input.c_str();        
        
        
        //sendto(test.sock, packetToSend->getData(), packetlen, 0, (struct sockaddr*)NULL, sizeof((struct sockaddr*)NULL));
        
        for (auto user : PrimaryClient::getInstance()->knownConnections) {
            //user.second->connection->SendingPort = sendingPort;
            user.second->connection->send(message, input.length()+1);
        }
        
    }

    a.recvThread.join();
}