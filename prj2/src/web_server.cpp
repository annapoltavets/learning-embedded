#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

#include "web_server.h"

static WebServer server(80);
static Led *_led = nullptr;

static void handleRoot()
{
  String html = "<!DOCTYPE html><html><head>";
  html += "<meta charset='UTF-8'>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<title>Керування LED</title>";
  html += "<style>";
  html += "body{text-align:center;font-family:sans-serif;margin-top:50px;background:#1a1a2e;color:#eee;}";
  html += "h2{color:#e94560;}";
  html += ".btn{display:inline-block;padding:15px 40px;font-size:22px;font-weight:bold;";
  html += "color:white;text-decoration:none;margin:10px;border-radius:8px;}";
  html += ".on{background:#27ae60;} .off{background:#e74c3c;}";
  html += ".bright{background:#2980b9;} .dim{background:#8e44ad;}";
  html += ".status{font-size:18px;margin:20px;}";
  html += "</style></head><body>";
  html += "<h2>Керування LED — ESP32-S3</h2>";
  html += "<div class='status'>Стан: <b>" + String(_led->isOn() ? "Увімкнено" : "Вимкнено") + "</b></div>";
  html += "<div class='status'>Яскравість: <b>" + String(map(_led->getBrightness(), 0, 255, 0, 100)) + "%</b></div>";
  html += "<br>";
  html += "<a href='/on'  class='btn on'>Увімкнути</a>";
  html += "<a href='/off' class='btn off'>Вимкнути</a>";
  html += "<br><br>";
  html += "<a href='/brighter' class='btn bright'>+</a>";
  html += "<a href='/dimmer'   class='btn dim'>&#8722;</a>";
  html += "</body></html>";
  server.send(200, "text/html; charset=UTF-8", html);
}

static void handleOn()
{
  _led->on();
  server.sendHeader("Location", "/");
  server.send(303);
}

static void handleOff()
{
  _led->off();
  server.sendHeader("Location", "/");
  server.send(303);
}

static void handleBrighter()
{
  _led->brighter();
  server.sendHeader("Location", "/");
  server.send(303);
}

static void handleDimmer()
{
  _led->dimmer();
  server.sendHeader("Location", "/");
  server.send(303);
}

void initWebServer(const char *ssid, const char *password, Led *led)
{
  _led = led;

  WiFi.softAP(ssid, password);
  Serial.println("Точка доступу запущена!");
  Serial.print("IP: ");
  Serial.println(WiFi.softAPIP()); // зазвичай 192.168.4.1

  server.on("/",         handleRoot);
  server.on("/on",       handleOn);
  server.on("/off",      handleOff);
  server.on("/brighter", handleBrighter);
  server.on("/dimmer",   handleDimmer);
  server.begin();
  Serial.println("HTTP-сервер запущено");
}

void webServerLoop()
{
  server.handleClient();
}
