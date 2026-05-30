#include "UDPConnection.h"
#include "IncomingHandler.h"

using namespace std;

void ChatInterface() {
    UDPConnection test;
    IncomingHandler a;
    string input;

    // Allow for testing on single machine
    cout << "ReceivingPort: ";
    int port;
    cin >> port;

    cout << "SendingPort: ";
    cin >> test.SendingPort;

    a.enableIncomingTraffic(port);


    // Setup Connections
    
    test.connectTo("127.0.0.1");

    cin.ignore();
    
    while (input != "exit") {
        // User input
        getline(cin, input);

        test.sendConnectionRequest();

        // Send User Input;
        unsigned char* message = (unsigned char*)input.c_str();        
        
        
        //sendto(test.sock, packetToSend->getData(), packetlen, 0, (struct sockaddr*)NULL, sizeof((struct sockaddr*)NULL));
        //test.send(message, input.length()+1);
    }

    a.recvThread.join();
}