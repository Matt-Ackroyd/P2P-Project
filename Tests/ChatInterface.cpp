#include "UDPConnection.h"
#include "IncomingHandler.h"
#include "UUID.h"

using namespace std;

void ChatInterface() {
    UUID idTest;
    idTest.GenerateNewID();

    UDPConnection *test = new UDPConnection(NULL);
    IncomingHandler a;
    string input;

    // Allow for testing on single machine
    // cout << "ReceivingPort: ";
    // int port;
    // cin >> port;

    // cout << "SendingPort: ";
    // cin >> test->SendingPort;
    test->SendingPort = 5000;
    a.enableIncomingTraffic(5000);

    // Setup Connections
    
    test->connectTo("127.0.0.1");
    test->sendConnectionRequest();

    cin.ignore();
    
    while (input != "exit") {
        // User input
        getline(cin, input);

        

        // Send User Input;
        unsigned char* message = (unsigned char*)input.c_str();        
        
        
        //sendto(test.sock, packetToSend->getData(), packetlen, 0, (struct sockaddr*)NULL, sizeof((struct sockaddr*)NULL));
        test->send(message, input.length()+1);
    }

    a.recvThread.join();
}