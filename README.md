# Info display

Software to show some info messages on a GM009605 (SSD1306 128x64) OLED display.

Copy the contents of `src/network_cfg.json.template` to `src/network_cfg.json` and configure accordingly. The "ap" boolean value defines whether the device works as an Access Point or a normal WiFi client. In the latter case, defining an "ip" of "0.0.0.0" results in a dynamic IP address, otherwise the property defines a static IP address.

If given an URL in the network config, the program will try to update text from that URL at an interval. For some reason the HTTPClient didn't work with a newer platform version, so espressif8266@1.8 is used. Thus also secure connections (HTTPS) using BearSSL::WiFiClientSecure didn't work so it's just HTTP for now.
