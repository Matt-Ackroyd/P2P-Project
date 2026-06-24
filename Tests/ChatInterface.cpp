#include "UDPConnection.h"
#include "IncomingHandler.h"
#include "ID.h"
#include <iostream>

void ChatInterface() {
  
    UDPConnection *test = new UDPConnection(NULL);
    IncomingHandler a;
    std::string input;

    // Allow for testing on single machine
    std::cout << "ReceivingPort: ";
    int port;
    std::cin >> port;

    // int sendingPort;
    // cout << "SendingPort: ";
    // cin >> sendingPort;
    std::cin.ignore();

    //test->SendingPort = 5000;
    a.enableIncomingTraffic(port);

    // Setup Connections
    
    //test->SendingPort = sendingPort;
    test->sendHandshakeRequest();

    std::cin.ignore();
    
    while (input != "exit") {
        // User input
        std::getline(std::cin, input);

        

        // Send User Input;
        unsigned char* message = (unsigned char*)input.c_str();        
        
        
        //sendto(test.sock, packetToSend->getData(), packetlen, 0, (struct sockaddr*)NULL, sizeof((struct sockaddr*)NULL));
        if (PrimaryClient::getInstance()->knownConnections.size() > 0) {
        
            for (auto user : PrimaryClient::getInstance()->knownConnections) {
                if (user.second == NULL) {
                    PrimaryClient::getInstance()->knownConnections.erase(user.first);
                }
                else {
                    user.second->connection->send(message, input.length()+1);
                }
            }
        }
        
    }

    a.recvThread.join();
}