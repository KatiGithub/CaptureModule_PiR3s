#pragma once

#include "CaptureEngine.hpp"
#include "TimeThread.hpp"

#include <pcap.h>

void injectPacket(CaptureEngine* captureEngine, TimeThread* TimeThread, char* membuf, uint32_t length);