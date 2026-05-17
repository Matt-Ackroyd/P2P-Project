#include "UDPConnection.h"

void UDPConnection::connectTo(char const *addr) {
    struct sockaddr_in servaddr;
    
    // clear servaddr
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_addr.s_addr = inet_addr(addr);
    servaddr.sin_port = htons(this->SendingPort);
    servaddr.sin_family = AF_INET;

    this->sock = socket(AF_INET, SOCK_DGRAM, 0);

    // connect to server
    if(connect(this->sock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        printf("\n Error : Connect Failed \n");
        exit(0);
    }
    cout << "Connected\n";
}

void UDPConnection::send(Packet packet) {
    // Update SeqNumbers
    packet.setSeqNum(this->currentSeqNum);
    this->currentSeqNum += 1;

    // Add Packet to buffer until its acknowlaged
    this->sendingBuffer.push_back(packet);

    // Send Over Socket
    sendto(this->sock, packet.encaplulate(), MAXLINE, 0, (struct sockaddr*)NULL, sizeof((struct sockaddr*)NULL));

}
