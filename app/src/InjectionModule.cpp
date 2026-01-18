#include "InjectionModule.hpp"
#include "CaptureEngine.hpp"
#include "TimeThread.hpp"

#include <iostream>
#include <pcap.h>
#include <string>
#include <cstring>
#include <thread>
#include <chrono>
#include <netinet/in.h>
#include <netinet/if_ether.h>
#include <netinet/ip.h>
#include <vector>

void injectPacket(CaptureEngine* captureEngine, TimeThread* timeThread, char* membuf, uint32_t length) {
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t* injectPort = captureEngine->rx;

    if(injectPort != NULL) {
        std::vector<uint8_t> tx_buffer(8 + length);
        int64_t current_time;
        timeThread->getTime(&current_time);

        std::memcpy(tx_buffer.data(), &current_time, sizeof(uint64_t));
        std::memcpy(tx_buffer.data() + 8, membuf, length);

        if(pcap_sendpacket(injectPort, tx_buffer.data(), tx_buffer.size()) != 0) {
            std::cerr << "Error sending packet: " << pcap_geterr(injectPort) << std::endl;
        } else {
            std::cout << "Injected packet of size: " << std::to_string(length) << std::endl;
        }

    }
}