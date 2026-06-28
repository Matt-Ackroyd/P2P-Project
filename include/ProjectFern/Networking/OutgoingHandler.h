#pragma once
#include <thread>
#include <chrono>
#include "PrimaryClient.h"

class OutgoingHandler {
private:
    std::thread OutgoingHandlerThread;
    std::mutex mtx;

    // Time Between KeepAlive Packets in milli
    int keepAliveInterval = 10000;
    std::unordered_set<RemoteUser*> keepAliveTargets;

    void OutgoingLoop();
public:
    OutgoingHandler();

    void enableConnection(RemoteUser* target);
    void disableConnection(RemoteUser* target);
};