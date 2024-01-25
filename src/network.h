#pragma once

#include <Arduino.h>
#include <string.h>

namespace Network {
    void setupWifi(void (*setText)(String));
    String getIP();
    void handleClient();
    String getUpdatedText();
}
