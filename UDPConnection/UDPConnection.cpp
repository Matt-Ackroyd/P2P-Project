#include "UDPConnection.h"

void UDPConnection::connectTo(char const *addr) {
    struct sockaddr_in servaddr;
    
    // clear servaddr
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_addr.s_addr = inet_addr(addr);
    servaddr.sin_port = htons(this->SendingPort);
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

void UDPConnection::send(char const *message) {
    sendto(this->s, message, MAXLINE, 0, (struct sockaddr*)NULL, sizeof((struct sockaddr*)NULL));
}


void UDPConnection::startReceiving(int ReceivingPort)
{
    char buffer[MAXLINE]; 
    struct sockaddr_in servaddr, cliaddr; 
    socklen_t clientlen = sizeof(cliaddr);

    int socketfd = socket(AF_INET, SOCK_DGRAM, 0);  

    bzero(&servaddr, sizeof(servaddr));
    bzero(&cliaddr, sizeof(cliaddr));

    // Socket: IPV4, Any connection, PORTNumber
    servaddr.sin_family    = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY; 
    servaddr.sin_port = htons(ReceivingPort);

    int a = bind(socketfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    cout << " Bind Return: \n" << a;

    while (true)
    {
        recvfrom(socketfd, buffer, sizeof(buffer),
            0, (struct sockaddr*)&cliaddr, &clientlen);

        Packet incomingPacket;
        incomingPacket.dencapsulate(buffer);

        // Packet Handling

        cout << "\nOther: " << incomingPacket.getData() << "\n";



        incomingPacket.cleanupAfterReceive();
    }
    
}
