#pragma once

#include "led.h"

void initWebServer(const char *ssid, const char *password, Led *led);
void webServerLoop();
