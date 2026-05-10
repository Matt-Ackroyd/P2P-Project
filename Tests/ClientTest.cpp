#include "../UDPConection/UDPConnection.cpp"
using namespace std;

void ClientTest() {
    UDPConnection test;
    test.connectTo("127.0.0.1");
    test.send("Hello There Server Test");
    cout << "sent\n";
}
