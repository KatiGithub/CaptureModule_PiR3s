#include "ConfigManager.hpp"
#include <cstring>
#include <iostream>


void ConfigManager::setConfig(AppConfig* new_cfg) {
    std::memcpy(&config, new_cfg, sizeof(AppConfig));

    std::cout << "ARP Logging: " << new_cfg->logARP << std::endl;
    std::cout << "UDP Logging: " << new_cfg->logUDP << std::endl;
    std::cout << "TCP Logging: " << new_cfg->logTCP << std::endl;
    std::cout << "Others Logging: " << new_cfg->logOthers << std::endl;

}

void ConfigManager::getConfig(AppConfig* cfg_buf) {
    std::memcpy(cfg_buf, &config, sizeof(AppConfig));
}