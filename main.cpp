#include "Tests/ClientTest.cpp"
#include "Tests/ServerTest.cpp"

using namespace std;

int main() {
    char input;
    cout << "0 for server, 1 for client\n";
    cin >> input;

    switch (input)
    {
    case '0':
        ServerTest();
        break;
    case '1':
        ClientTest();
        break;
    default:
        cout << "Pick one";
        break;
    }
    return 0;
}
