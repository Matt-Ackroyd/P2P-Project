#include "UDPConnection.h"

using namespace std;

int main() {
    UDPConnection test;
    test.connectTo();
    test.send();
    cout << "sent\n";
}
