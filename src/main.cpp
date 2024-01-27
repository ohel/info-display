#include <Arduino.h>
#include <string.h>
#include <lcdgfx.h>
#include "network.h"

auto display = DisplaySSD1306_128x64_I2C(-1);
unsigned long next_update = 0;

void setText(String text) {
    String line1, line2, line3, line4;

    int newline1 = text.indexOf('\n');
    if (newline1 == -1) {
        line1 = text;
    } else {
        line1 = text.substring(0, newline1);
        int newline2 = text.indexOf('\n', newline1 + 1);
        if (newline2 == -1) {
            line2 = text.substring(newline1 + 1);
        } else {
            line2 = text.substring(newline1 + 1, newline2);
            int newline3 = text.indexOf('\n', newline2 + 1);
            if (newline3 == -1) {
                line3 = text.substring(newline2 + 1);
            } else {
                line3 = text.substring(newline2 + 1, newline3);
                int newline4 = text.indexOf('\n', newline3 + 1);
                line4 = (newline4 == -1) ? text.substring(newline3 + 1) :
                        text.substring(newline3 + 1, newline4);
            }
        }
    }

    display.clear();
    if (line1.length() > 0) display.printFixed(0, 0, line1.c_str());
    if (line2.length() > 0) display.printFixed(0, 16, line2.c_str());
    if (line3.length() > 0) display.printFixed(0, 32, line3.c_str());
    if (line4.length() > 0) display.printFixed(0, 48, line4.c_str());
}

void setup() {
    Serial.begin(115200);
    delay(500);
    display.setFixedFont(ssd1306xled_font8x16);
    display.begin();
    Network::setupWifi(setText);
    delay(3000);
}

void loop() {
    Network::handleClient();
    if (next_update < millis()) {
        // 15 min = 900000 ms
        next_update = millis() + 900000;
        String new_text = Network::getUpdatedText();
        if (new_text.length() > 0) setText(new_text);
    }
    delay(5000);
}
