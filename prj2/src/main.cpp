#include <Arduino.h>
#include <WiFi.h>

#include "led.h"
#include "web_server.h"

const char *SSID     = "ESP32_LED_Control";
const char *PASSWORD = "password123";

const int BUTTON_PIN = 0; // вбудована кнопка BOOT

const int DEBOUNCE_DELAY = 50; // мс

Led led(
  /* pin          */ 16,
  /* pwmChannel   */ 0,
  /* pwmFreq      */ 5000,
  /* pwmResolution*/ 8,
  /* brightnessStep*/ 25
);

volatile bool buttonFlag        = false;
unsigned long lastInterruptTime = 0;

void IRAM_ATTR handleButton()
{
  unsigned long now = millis();
  if (now - lastInterruptTime > DEBOUNCE_DELAY)
  {
    lastInterruptTime = now;
    buttonFlag = true;
  }
}

void setup()
{
  Serial.begin(115200);

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), handleButton, FALLING);

  led.begin();
  initWebServer(SSID, PASSWORD, &led);
}

void loop()
{
  webServerLoop();

  if (buttonFlag)
  {
    buttonFlag = false;
    led.cycle();
    Serial.print("Кнопка: яскравість = ");
    Serial.println(map(led.getBrightness(), 0, 255, 0, 100));
  }

  delay(10); // невелика затримка для стабільності FreeRTOS
}
