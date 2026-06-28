#include "OutgoingHandler.h"

OutgoingHandler::OutgoingHandler() {
    this->OutgoingHandlerThread = std::thread(&OutgoingHandler::OutgoingLoop, this);
};


void OutgoingHandler::OutgoingLoop() {
    PrimaryClient* client = PrimaryClient::getInstance();
    SOCKTYPE socketfd = client->socketfd;

    sockaddr_in relay = client->getPreferedRelay();

    char keepAlive[0] = {};
    while (true) {
        // Stay Connected to your prefered relay in order to be informed of incoming connections
        int a = sendto(socketfd, keepAlive, 0, 0, (struct sockaddr*)&relay, sizeof(relay));

        // Send KeepAlive packets to other targets ( Maybe replace this list with known connections)
        for (auto target: this->keepAliveTargets) {
            target->connection.sendKeepAlive();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(this->keepAliveInterval));
    }
}

void OutgoingHandler::enableConnection(RemoteUser* target) {
    mtx.lock();
    this->keepAliveTargets.insert(target);
    mtx.unlock();
}
void OutgoingHandler::disableConnection(RemoteUser* target) {
    mtx.lock();
    this->keepAliveTargets.erase(target);
    mtx.unlock();
}
