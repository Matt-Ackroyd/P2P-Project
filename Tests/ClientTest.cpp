#include "../UDPConnection/UDPConnection.cpp"
#include "../UDPConnection/Packet.h"
using namespace std;

void ClientTest() {
    UDPConnection test;
    test.connectTo("127.0.0.1");

    Packet b;
    b.innit(1000, "Hello There Server Test", sizeof("Hello There Server Test"), MESSAGE);
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
    test.send(b.encaplulate());
    b.cleanupAfterSend();
    cout << "sent\n";
}
