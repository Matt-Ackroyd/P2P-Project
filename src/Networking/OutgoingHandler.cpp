#include "OutgoingHandler.h"

std::mutex OutgoingHandler::mtx;

OutgoingHandler::OutgoingHandler() {
    this->OutgoingHandlerThread = std::thread(&OutgoingHandler::OutgoingLoop, this);
};


void OutgoingHandler::OutgoingLoop() {
    PrimaryClient* client = PrimaryClient::getInstance();
    SOCKTYPE socketfd = client->socketfd;

    sockaddr_in relay = client->getPreferedRelay();

    char keepAlive[0] = {};
    while (true) {
        // TODO Replace with online list instead
        for (auto [id, recipient]: client->knownConnections) {
            std::deque<Packet*>* packetsToBeSend = recipient->connection.getOutgoingBuffer(); 
            for (auto packet: *packetsToBeSend) {

                // Add timers for each packet
                recipient->connection.sendPacket(packet);
            }
        }

        // Stay Connected to your prefered relay in order to be informed of incoming connections
        int a = sendto(socketfd, keepAlive, 0, 0, (struct sockaddr*)&relay, sizeof(relay));

        // Send KeepAlive packets to other targets ( Maybe replace this list with known connections)
        for (auto target: this->keepAliveTargets) {
            target->connection.sendKeepAlive();
        }

        // Change to comaring time and add section for sending outgoing packets 
        std::this_thread::sleep_for(std::chrono::milliseconds(this->keepAliveInterval));
        std::chrono::system_clock::now();
    }
}

void OutgoingHandler::enableConnection(RemoteUser* target) {
    std::lock_guard<std::mutex> lock(mtx);
    this->keepAliveTargets.emplace(target);
}
void OutgoingHandler::disableConnection(RemoteUser* target) {
    std::lock_guard<std::mutex> lock(mtx);
    this->keepAliveTargets.erase(target);
}
