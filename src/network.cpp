#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include "network.h"
#include "network_cfg.h"
#include "index_html.h"
#include "favicon.h"

namespace Network {
    void (*_setText)(String);
    ESP8266WebServer _server(_server_port);

    HTTPClient _http_client;
    bool _update_override = false;

    void handleClient() {
        _server.handleClient();
    }

    void setupWifi(void (*setText)(String)) {
        _setText = setText;

        IPAddress subnet(255, 255, 255, 0);
        bool is_ap =
            _wifi_ip.toString() != INADDR_NONE.toString() &&
            _wifi_gateway.toString() == INADDR_NONE.toString();

        // Note: here WiFi setup only as AP or STA, not both (WIFI_AP_STA).

        Serial.println("");
        if (is_ap) {
            WiFi.mode(WIFI_AP);
            Serial.println("Setting up AP:");
            Serial.print("    SSID: ");
            Serial.println(_wifi_ssid);
            Serial.print("    Password: ");
            if (strlen(_wifi_password) < 8) {
                Serial.println("");
                WiFi.softAP(_wifi_ssid);
            } else {
                Serial.println(_wifi_password);
                WiFi.softAP(_wifi_ssid, _wifi_password);
            }
            Serial.print("    IP/Gateway: ");
            Serial.println(_wifi_ip);
            WiFi.softAPConfig(_wifi_ip, _wifi_ip, subnet);
            _setText("----WiFi AP----\nSSID: " + String(_wifi_ssid) + "\nPwd: " + String(_wifi_password) + "\nGw: " + _wifi_ip.toString());
        } else {
            WiFi.mode(WIFI_STA);
            if (_wifi_ip.toString() == INADDR_NONE.toString()) {
                Serial.println("Using dynamic IP address.");
            } else {
                WiFi.config(_wifi_ip, _wifi_gateway, subnet);
            }
        }

        WiFi.begin(_wifi_ssid, _wifi_password);

        if (!is_ap) {
            int timeout = 0;
            Serial.print("Connecting to: ");
            Serial.println(_wifi_ssid);
            _setText("Connecting to:\n" + String(_wifi_ssid));
            while (WiFi.status() != WL_CONNECTED && timeout < 20) {
                delay(1000);
                Serial.print(".");
                timeout++;
            }
            Serial.println("");
            if (WiFi.status() == WL_CONNECTED) {
                Serial.print("WiFi connected using IP address: ");
                Serial.println(WiFi.localIP());
            } else {
                Serial.println("Failed to connect to WiFi.");
            }
        }

        _setText("IP:\n" + WiFi.localIP().toString() + "\nHTTP PORT:\n" + String(_server_port));

        _server.on("/", HTTP_GET, [](){
            Serial.println("GET: /");
            _server.send(200, "text/html", index_html);
        });

        _server.on("/favicon.ico", HTTP_GET, [](){
            Serial.println("GET: /favicon.ico");
            _server.send_P(200, "image/x-icon", favicon, sizeof(favicon_bytes));
        });

        _server.on("/", HTTP_POST, [](){
            if (!_server.hasArg("plain")) {
                Serial.println("POST with no body.");
                _server.send(400, "text/html", "NOT OK");
                return;
            }
            _server.send(200, "text/html", "OK");
            Serial.println("POST with body:");
            String body = _server.arg("plain");
            Serial.println(body);

            bool only_newlines = true;
            for (unsigned int i = 0; i < body.length(); i++) {
                Serial.println(body[i]);
                if (body[i] != '\n') {
                    only_newlines = false;
                    break;
                }
            }

            _update_override = !only_newlines;
            _setText(body);
        });

        _server.begin();
        Serial.println("Server is running.");
    }

    String getUpdatedText() {
        if (_update_override) {
            Serial.println("Override: not fetching updated text.");
            return "";
        }

        String retval = "";

        Serial.println("Fetching updated text.");
        _http_client.begin(_text_url);
        _http_client.setTimeout(10000);
        int responseCode = _http_client.GET();
        if (responseCode > 0) {
            Serial.println("Fetched text:");
            retval = _http_client.getString();
            Serial.println(retval);
        } else {
            Serial.println("Error fetching text: " + String(responseCode) + " " + _http_client.errorToString(responseCode));
            retval = "Not updated. IP:\n" + WiFi.localIP().toString() + "\nHTTP PORT:\n" + String(_server_port);
        }
        _http_client.end();

        return retval;
    }
};
