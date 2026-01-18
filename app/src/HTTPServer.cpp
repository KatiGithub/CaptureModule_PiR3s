#include "HTTPServer.hpp"
#include "ConfigManager.hpp"
#include "CaptureEngine.hpp"
#include "TimeThread.hpp"
#include "InjectionModule.hpp"
#include "crow/crow_all.h"

#include <vector>

void startServer(ConfigManager* config, TimeThread* timeThread, CaptureEngine* captureEngine) {
    crow::SimpleApp app;

    CROW_ROUTE(app, "/api/status").methods(crow::HTTPMethod::GET)
    ([&config, &captureEngine]() {
        crow::json::wvalue x;
        
        x["tcpPacketsCount"] = captureEngine->tcpCount;
        x["arpPacketsCount"] = captureEngine->arpCount;
        x["udpPacketsCount"] = captureEngine->udpCount;
        x["otherPacketsCount"] = captureEngine->othersCount;
        x["loggingPortStatus"] = captureEngine->RxPortLink; 

        return x;
    });

    CROW_ROUTE(app, "/api/setTime").methods(crow::HTTPMethod::PUT)
    ([&config, &timeThread](const crow::request& req) {

        if(req.body.empty()) {
            return crow::response(304, "Wrong time format");
        }

        try {
            int64_t new_time = std::stoll(req.body);
            timeThread->setTime(&new_time);
        } catch(const std::invalid_argument& e) {
            return crow::response(304, "Invalid number");
        } catch(const std::out_of_range& e) {
            return crow::response(304, "Too large");
        }

        return crow::response(200);
    });

    CROW_ROUTE(app, "/api/injectPacket").methods(crow::HTTPMethod::POST)
    ([&config, &timeThread, &captureEngine](const crow::request& req) {

        uint32_t length = req.body.size();

        if(length > 0) {
            injectPacket(
                captureEngine,
                timeThread,
                (char*) req.body.data(),
                length
            );
        }


        return crow::response(200);
    });

    CROW_ROUTE(app, "/api/loadConfig").methods(crow::HTTPMethod::GET)
    ([&config]() {
        AppConfig cfg_curr;
        config->getConfig(&cfg_curr);

        crow::json::wvalue x;

        x["logTCP"] = cfg_curr.logTCP;
        x["logARP"] = cfg_curr.logARP;
        x["logUDP"] = cfg_curr.logUDP;
        x["logOthers"] = cfg_curr.logOthers;

        return x;
    });

    CROW_ROUTE(app, "/api/updateConfig").methods(crow::HTTPMethod::PUT)
    ([&config](const crow::request& req) {
        crow::json::rvalue x = crow::json::load(req.body);

        if(!x.has("logTCP") || !x.has("logARP") || !x.has("logUDP") || !x.has("logOthers")) {
            return crow::response(304, "Missing fields");
        }

        AppConfig cfg_buffer;
        cfg_buffer.logTCP = x["logTCP"].b();
        cfg_buffer.logARP = x["logARP"].b();
        cfg_buffer.logUDP = x["logUDP"].b();
        cfg_buffer.logOthers = x["logOthers"].b();
        
        config->setConfig(&cfg_buffer);
        return crow::response(200);
    });

    app.loglevel(crow::LogLevel::WARNING);
    app.port(8080).run();
}