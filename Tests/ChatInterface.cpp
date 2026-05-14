#include "../UDPConnection/UDPConnection.h"

using namespace std;

void ChatInterface() {
    UDPConnection test;
    string input;

    // Allow for testing on single machine
    cout << "ReceivingPort: ";
    int port;
    cin >> port;

    cout << "SendingPort: ";
    cin >> test.SendingPort;


    // Setup Connections
    thread t(test.startReceiving, port);
    test.connectTo("127.0.0.1");

    
    while (input != "exit") {
        // User input
        getline(cin, input);

        // Send User Input
        char message[input.length()+1];
        strcpy(message, input.c_str());


        Packet packetToSend;
        packetToSend.innit(0, message, sizeof(message), MESSAGE);
        test.send(packetToSend.encaplulate());
        packetToSend.cleanupAfterSend();
    }

    t.join();
}