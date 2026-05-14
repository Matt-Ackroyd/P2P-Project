#include <iostream>
#include <stdio.h>
#include "../UDPConnection/Packet.cpp"


using namespace std;

void PacketTest() {
    Packet a;

    cout << "Pre-encapsulation: \n";
    a.innit(1000, "TEsting this thing", sizeof("TEsting this thing"), MESSAGE);
    cout << "Seq Num: " << a.getSeqNum() << "\n";
    cout << "DataType: " << a.getDataType() << "\n";
    cout << "DataLength: " << a.getDataLength() << "\n";
    cout << "Data: \n";
    for (unsigned long int i = 0; i < a.getDataLength(); i++) {
        printf("0x%02x", a.getData()[i]);
        cout << "  " << a.getData()[i];
        cout << "  " << i;
        cout << "\n";
    }

    char* test;
    test = a.encaplulate();

    cout << "\nencapsulated: \n";
    cout << "Data: \n";
    for (unsigned long int i = 0; i < a.getPacketLength(); i++) {
        printf("0x%02x", test[i]);
        cout << "  " << test[i];
        cout << "\n";
        
    }
    Packet b;
    cout << "\nDencapsulated: \n";
    b.dencapsulate(test);

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
   
    cout << "\n";
}
