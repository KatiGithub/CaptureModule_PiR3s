#include "ConfigManager.hpp"
#include "TimeThread.hpp"
#include "CaptureEngine.hpp"

#include <iostream>
#include <pcap.h>
#include <string>
#include <fstream>
#include <cstring>
#include <thread>
#include <chrono>
#include <netinet/in.h>
#include <netinet/if_ether.h>
#include <netinet/ip.h>
#include <stdio.h>

static const uint16_t TYPE_TCP = 0x0001;
static const uint16_t TYPE_UDP = 0x0002;
static const uint16_t TYPE_ARP = 0x0003;
static const uint16_t TYPE_OTHER = 0x0004;

// ... other includes ...

int check_link_file(const char *ifname) {
    // 1. Use the 'carrier' file, not 'operstate'
    // 'carrier' is 1 (UP) or 0 (DOWN) based on physical connection.
    char path[128];
    snprintf(path, sizeof(path), "/sys/class/net/%s/carrier", ifname);

    // 2. Open a new stream every time to avoid buffering stale data
    std::ifstream file(path);
    
    if (!file.is_open()) {
        return -1; // Interface likely doesn't exist
    }

    std::string line;
    std::cout << line << std::endl;
    if (std::getline(file, line)) {
        // 3. Simple check: "1" = Link Up, "0" = Link Down
        if (line == "1") return 1;
        return 0;
    }
    
    return 0;
}

pcap_t* openInterface(const std::string device, char* errbuf) {
    pcap_t* handle = pcap_open_live(device.c_str(), 65535, 1, 10, errbuf);
    if(handle == nullptr) {
        std::cout << "Cannot open pcap" << std::endl;
    }

    return handle;
}

uint16_t getPacketType(const u_char* packet) {
    struct ether_header* eth = (struct ether_header*) packet;

    uint16_t type = ntohs(eth->ether_type);
    if(type == ETHERTYPE_ARP) {
        return TYPE_ARP;
    }

    if(type == ETHERTYPE_IP) {
        struct ip* ip_header = (struct ip*)(packet + sizeof(struct ether_header));

        if(ip_header->ip_p == IPPROTO_TCP) return TYPE_TCP;
        if(ip_header->ip_p == IPPROTO_UDP) return TYPE_UDP;

        return TYPE_OTHER;
    }

    return TYPE_OTHER;
}

void CaptureEngine::incrementPacketCount(uint16_t packetType) {
    if(packetType == TYPE_ARP) {
        arpCount += 1;
    } else if(packetType == TYPE_TCP) {
        tcpCount += 1;
    } else if(packetType == TYPE_UDP) {
        udpCount += 1;
    } else {
        othersCount += 1;
    }
}

void CaptureEngine::engineLoop(ConfigManager* configManager, TimeThread* timeThread) {
    char errbuf[PCAP_ERRBUF_SIZE];

    CaptureEngine::rx = openInterface("eth1", errbuf); // port without sticker
    pcap_t* tx = openInterface("eth0", errbuf); // port with sticker

    // RxPortLink = (!rx);

    if(!rx || !tx) return;
    struct pcap_pkthdr* header;
    const u_char* original_packet;

    uint8_t tx_buffer[2000];
    while(true) {
        int status = check_link_file("eth1");

        if(status == 1) {
            RxPortLink = true;
        } else {
            RxPortLink = false;
        }

        int res = pcap_next_ex(rx, &header, &original_packet);
        if(res == 1) {

            // 0. Get config first
            AppConfig config;
            configManager->getConfig(&config);

            // 1. Get packet type
            uint16_t proto_type = getPacketType(original_packet);

            if((proto_type == TYPE_ARP && config.logARP == false) || (proto_type == TYPE_UDP && config.logUDP == false) || (proto_type == TYPE_TCP && config.logTCP == false) || (proto_type == TYPE_OTHER && config.logOthers == false) 
            ) {
                goto jmp;
            }
            incrementPacketCount(proto_type);

            // 2. Get current time
            int64_t current_timestamp;
            timeThread->getTime(&current_timestamp);

            // 3. Construct new packet
            int16_t current_index = 0;
            
            std::memcpy(tx_buffer, &current_timestamp, sizeof(int64_t));
            current_index = current_index + sizeof(int64_t);

            std::memcpy(tx_buffer + current_index, &proto_type, sizeof(uint16_t));
            current_index = current_index + sizeof(uint16_t);

            std::memcpy(tx_buffer + current_index, original_packet, header->len);
            current_index = current_index + header->len;

            int new_size = current_index;

            // 4. send packet
            if(pcap_sendpacket(tx, tx_buffer, new_size) != 0) {
                std::cout << "Error sending packet with buffer size: " << std::to_string(new_size) << std:: endl;
            } else {
                std::cout << "Logged packet with size: " << std::to_string(new_size+10) << std:: endl;
            }
        }
        jmp:;
    }
    pcap_close(rx);
    pcap_close(tx);
}