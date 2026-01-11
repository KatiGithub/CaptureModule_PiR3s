#pragma once
#include <mutex>
#include <string>
#include <atomic>

struct AppConfig {
    bool logTCP = true;
    bool logARP = true;
    bool logUDP = true;
    bool logOthers = true;
};

class ConfigManager {
    private:
        AppConfig config;
        std::mutex mtx;

    public:
        void setConfig(AppConfig* new_cfg);
        void getConfig(AppConfig* cfg_buffer);
};