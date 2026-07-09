#pragma once
#include <thread>
#include <chrono>
#include "PrimaryClient.h"

class OutgoingHandler {
private:
    std::thread OutgoingHandlerThread;
    static std::mutex mtx;

    // Time Between KeepAlive Packets in milli
    int keepAliveInterval = 10000;
    int connectionTimeout = 60000;

    void OutgoingLoop();
public:
    OutgoingHandler();
};