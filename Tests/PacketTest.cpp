#include <iostream>
#include <stdio.h>
#include "../UDPConection/Packet.cpp"


using namespace std;

void PacketTest() {
    Packet a;
    char data[4] = {1, 2, 4, 5};
    a.innit(1000, data, sizeof(data), MESSAGE);

    char* test;
    test = a.encaplulate();

    cout << "Data: \n";
    for (unsigned long int i = 0; i < a.getPacketLength(); i++) {
        printf("0x%02x", test[i]);
        cout << "  " << test[i];
        cout << "\n";
        
    }
   
    cout << "\n";
}
