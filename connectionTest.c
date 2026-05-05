#include <stdio.h>
#include <strings.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <openssl/evp.h>
#define PORT 5000
#define MAXLINE 1000

int main(int argc, char const* argv[]) {

    
    EVP_PKEY *pkey = NULL;

    pkey = EVP_PKEY_Q_keygen(NULL, NULL, "ML-KEM-768");



    // Create a UDP Socket
    // char buffer[100];
    // char *message = "Hello Client";
    // int listenfd, len;
    // struct sockaddr_in servaddr, cliaddr;
    // bzero(&servaddr, sizeof(servaddr));

    // listenfd = socket(AF_INET, SOCK_DGRAM, 0);        
    // servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    // servaddr.sin_port = htons(PORT);
    // servaddr.sin_family = AF_INET;

}