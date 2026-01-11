#pragma once

#include "ConfigManager.hpp"
#include "TimeThread.hpp"
#include "CaptureEngine.hpp"

void startServer(ConfigManager* config, TimeThread* timeThread, CaptureEngine* captureEngine);