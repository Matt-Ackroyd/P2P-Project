#include "Tests/ChatInterface.cpp"
//#include "UDPConnection/SendingPackets/UDPConnection.h"

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
        break;
    case '1':
        break;
    case '2':
        break;
    default:
        cout << "Pick one\n";
        break;
    }
    return 0;
}
