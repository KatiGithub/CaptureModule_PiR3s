#include <iostream>
#include <thread>

#include "TimeThread.hpp"
#include "HTTPServer.hpp"
#include "ConfigManager.hpp"
#include "CaptureEngine.hpp"


int main() {
    ConfigManager config;
    TimeThread timeThread;
    CaptureEngine captureEngine;
    std::cout << "Starting Capture Module" << std::endl;
    
    std::thread webThread(startServer, &config, &timeThread, &captureEngine);
    std::cout << "----------------------------------------------" << std::endl;
    std::cout << "Web Server started successfully" << std::endl;
    std::cout << "Hosting on: 0.0.0.0" << std::endl;
    std::cout << "Port: 8080" << std::endl;
    std::cout << "----------------------------------------------" << std::endl;
    
    std::thread captureThread(&CaptureEngine::engineLoop, &captureEngine, &config, &timeThread);
    std::cout << "----------------------------------------------" << std::endl;
    std::cout << "Capture Thread started successfully" << std::endl;
    std::cout << "Receiving from eth1" << std::endl;
    std::cout << "Logging to eth0" << std::endl;
    std::cout << "----------------------------------------------" << std::endl;

    std::thread timerThread(&TimeThread::timeLoop, &timeThread);


    if(webThread.joinable()) webThread.join();
    if(captureThread.joinable()) captureThread.join();
    if(timerThread.joinable()) timerThread.join();
    return 0;
}
