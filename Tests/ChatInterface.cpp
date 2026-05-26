#include "../UDPConnection/SendingPackets/UDPConnection.h"
#include "../UDPConnection/ReceivingPackets/IncomingHandler.h"

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

    
    while (input != "exit") {
        // User input
        getline(cin, input);

        // Send User Input;
        unsigned char* message = (unsigned char*)input.c_str();


        Packet packetToSend(0, PacketType::PACKET);
        
        sendto(test.sock, packetToSend.serialize(message, sizeof(message), NULL, NULL), MAXLINE, 0, (struct sockaddr*)NULL, sizeof((struct sockaddr*)NULL));
        //test.send(packetToSend.serialize(message, sizeof(message), NULL, NULL));
    }

    a.recvThread.join();
}