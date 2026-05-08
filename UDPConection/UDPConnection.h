#include <iostream>
#include <stdio.h>
#include <strings.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 5000

using namespace std;

class UDPConnection {
    // Socket s
    int s;
    public:       
        void connectTo();  
        void send();
};