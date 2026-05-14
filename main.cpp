#include "Tests/ClientTest.cpp"
#include "Tests/ServerTest.cpp"
#include "Tests/PacketTest.cpp"
#include "Tests/ChatInterface.cpp"
#include "UDPConnection/UDPConnection.h"

using namespace std;

int main() {

    ChatInterface();

    char input;
    cout << "0 for server, 1 for client\n";
    cin >> input;
    cout << "\n\n\n\n";

    switch (input)
    {
    case '0':
        ServerTest();
        break;
    case '1':
        ClientTest();
        break;
    case '2':
        PacketTest();
        break;
    default:
        cout << "Pick one\n";
        break;
    }
    return 0;
}
