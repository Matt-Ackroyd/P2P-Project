#include "OutgoingHandler.h"
#include "FileHandler.h"

OutgoingHandler::OutgoingHandler() {
    this->OutgoingHandlerThread = std::thread(&OutgoingHandler::OutgoingLoop, this);
};


void OutgoingHandler::OutgoingLoop() {
    PrimaryClient* client = PrimaryClient::getInstance();
    SOCKTYPE socketfd = client->socketfd;

    sockaddr_in relay = client->getPreferedRelay();

    char keepAlive[1] = {1};

    auto nextCheck = std::chrono::system_clock::now() + std::chrono::milliseconds(this->keepAliveInterval);
    while (true) {

        for (auto& userid: client->onlineConnections) {
            RemoteUser* recipient = client->getUser(userid);

            // For each server you share with this user, send a sync request for that server
            if (recipient->connection.synced == false) {
                for (auto& [serverid, server]: client->allServers) {
                    if (server->knownUsers.contains(*recipient->getID())) {
                        recipient->connection.sendSyncRequest(server);
                    }
                }
                recipient->connection.synced = true;
            }


            // Send Any Outgoing packets
            
            recipient->connection.mtx.lock(); // Mtx lock because packets would be deleted at any time otherwise
            int i = recipient->connection.posOfFirstOutgoingPacket;
            while (recipient->connection.outgoingBuffer[i] != nullptr) {
                Packet* packet = recipient->connection.outgoingBuffer[i];
                // Add timers for each packet
                if (packet->timeToSend <= std::chrono::system_clock::now()) {
                    recipient->connection.sendPacket(packet);
                        packet->timesResent += 1;
                    packet->timeToSend = std::chrono::system_clock::now() + std::chrono::milliseconds(500);
                }

                i = (i+1) % recipient->connection.windowSize;
            }
            recipient->connection.mtx.unlock();
        }


        // File Manager
        client->fileHandler->manageFiles();

        

        // Send Keep Alive To Our Relay
        if (std::chrono::system_clock::now() > nextCheck) {
            // Stay Connected to your prefered relay in order to be informed of incoming connections
            int a = sendto(socketfd, keepAlive, 0, 0, (struct sockaddr*)&relay, sizeof(relay));
            // Send KeepAlive packets to other targets ( Maybe replace this list with known connections)  
            nextCheck = std::chrono::system_clock::now() + std::chrono::milliseconds(this->keepAliveInterval);
        }

        
        // Send Keep Alive Packets to online users we haven't contacted in a while
        for (auto& userid: client->onlineConnections) {
            RemoteUser* target = client->getUser(userid);
            auto currentTime = std::chrono::system_clock::now();

            // Prevent Spamming the target with keep alive 
            if (currentTime > target->connection.sendCooldown) {
                target->connection.sendCooldown = currentTime + std::chrono::milliseconds(this->keepAliveInterval);

                // If its been a while sinse we had contact send a keep alive packet
                if (currentTime > target->connection.lastHeardFrom + std::chrono::milliseconds(this->keepAliveInterval)) {
                    target->connection.sendKeepAlive();
                }
                
                // Disconnet User if its been A very long time
                if (currentTime > target->connection.lastHeardFrom + std::chrono::milliseconds(this->connectionTimeout)) {
                    PrimaryClient::disableConnection(target);
                    break;
                }
            }
        }
        //std::this_thread::sleep_for(std::chrono::milliseconds(10));
        
        
    }
}
