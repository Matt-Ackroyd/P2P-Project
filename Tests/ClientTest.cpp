#include "../UDPConection/UDPConnection.cpp"
#include "../UDPConection/Packet.h"
using namespace std;

void ClientTest() {
    UDPConnection test;
    test.connectTo("127.0.0.1");

    Packet a;
    a.innit(1000, "Hello There Server Test", sizeof("Hello There Server Test"), MESSAGE);
    test.send(a.encaplulate());
    //a.cleanup();
    cout << "sent\n";
}
