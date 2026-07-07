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

    auto nextCheck = std::chrono::system_clock::now() + std::chrono::milliseconds(this->keepAliveInterval);
    while (true) {
        // TODO Replace with online list instead
        for (auto [id, recipient]: client->knownConnections) {
            std::deque<Packet*>* packetsToBeSend = recipient->connection.getOutgoingBuffer(); 
            for (auto& packet: *packetsToBeSend) {

                // Add timers for each packet
                if (packet->timeToSend <= std::chrono::system_clock::now()) {
                    recipient->connection.sendPacket(packet);
                    packet->timeToSend = std::chrono::system_clock::now() + std::chrono::milliseconds(1000);
                }
            }
        }

        // Stay Connected to your prefered relay in order to be informed of incoming connections
        int a = sendto(socketfd, keepAlive, 0, 0, (struct sockaddr*)&relay, sizeof(relay));

        // Send Keep Alive every specified intervil
        if (std::chrono::system_clock::now() > nextCheck) {
            // Send KeepAlive packets to other targets ( Maybe replace this list with known connections)
            for (auto& target: this->keepAliveTargets) {
                target->connection.sendKeepAlive();
                nextCheck = std::chrono::system_clock::now() + std::chrono::milliseconds(this->keepAliveInterval);
            }
        }

        // 
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        
        
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
