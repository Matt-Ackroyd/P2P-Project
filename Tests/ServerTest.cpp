#include <stdio.h>
#include <strings.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "../UDPConnection/Packet.h"

#include <openssl/evp.h>
#define PORT 5000
#define MAXLINE 1000

void ServerTest() {


    // EVP_PKEY *pkey = EVP_PKEY_Q_keygen(NULL, NULL, "ML-KEM-768");

    // unsigned char *out = NULL;
    // size_t secretlen = 0;

    // EVP_PKEY_get_raw_private_key(pkey, out, &secretlen);

    // printf("");

    //Create a UDP Socket
    char buffer[100];
    int listenfd;
    struct sockaddr_in servaddr, cliaddr;
    bzero(&servaddr, sizeof(servaddr));

    // Create a UDP Socket
    listenfd = socket(AF_INET, SOCK_DGRAM, 0);        
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);
    servaddr.sin_family = AF_INET; 
 
    // bind server address to socket descriptor
    bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
     
    //receive the datagram
    socklen_t len = sizeof(cliaddr);
    int n = recvfrom(listenfd, buffer, sizeof(buffer),
            0, (struct sockaddr*)&cliaddr,&len);

    cout << "Data: \n";
    for (unsigned long int i = 0; i < n; i++) {
        printf("0x%02x", buffer[i]);
        cout << "  " << buffer[i];
        cout << "\n";
        
    }

    Packet b;
    b.dencapsulate(buffer);
    
    cout << "Seq Num: " << b.getSeqNum() << "\n";
    cout << "DataType: " << b.getDataType() << "\n";
    cout << "DataLength: " << b.getDataLength() << "\n";
    cout << "Data: \n";
    for (unsigned long int i = 0; i < b.getDataLength(); i++) {
        printf("0x%02x", b.getData()[i]);
        cout << "  " << b.getData()[i];
        cout << "  " << i;
        cout << "\n";
    }
    cout << "Output: " << b.getData() << "\n";
}