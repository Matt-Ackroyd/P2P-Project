#include "UDPConnection.h"

void UDPConnection::connectTo() {
    struct sockaddr_in servaddr;
    
    // clear servaddr
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);
    servaddr.sin_family = AF_INET;

    this->s = socket(AF_INET, SOCK_DGRAM, 0);

    // connect to server
    if(connect(this->s, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        printf("\n Error : Connect Failed \n");
        exit(0);
    }
    cout << "Connected\n";
}

void UDPConnection::send() {
    sendto(this->s, "FUCK", sizeof("FUCK"), 0, (struct sockaddr*)NULL, sizeof((struct sockaddr*)NULL));
}