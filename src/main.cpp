#include "../Tests/ChatInterface.cpp"


int main() {

    ChatInterface();

    char input;
    std::cout << "0 for server, 1 for client\n";
    std::cin >> input;
    std::cout << "\n\n\n\n";

    switch (input)
    {
    case '0':
        break;
    case '1':
        break;
    case '2':
        break;
    default:
        std::cout << "Pick one\n";
        break;
    }
    return 0;
}
