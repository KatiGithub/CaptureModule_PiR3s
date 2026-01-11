#pragma once

#include "ConfigManager.hpp"
#include "TimeThread.hpp"


class CaptureEngine {
    private:
        void incrementPacketCount(uint16_t packetType);

    public:
        int32_t tcpCount{0};
        int32_t arpCount{0};
        int32_t udpCount{0};
        int32_t othersCount{0};
        bool RxPortLink{false};
        
        void engineLoop(ConfigManager* configManager, TimeThread* timeThread);
};


