#pragma once

class Led {
public:
  Led(int pin, int pwmChannel, int pwmFreq, int pwmResolution, int brightnessStep);

  void begin();
  void on();
  void off();
  void brighter();
  void dimmer();
  void cycle(); // збільшує яскравість по колу; вмикає LED якщо вимкнено

  bool isOn()         const;
  int  getBrightness() const;

private:
  void apply();

  volatile int _pin;
  volatile int _pwmChannel;
  volatile int _pwmFreq;
  volatile int _pwmResolution;
  volatile int _brightnessStep;
  volatile bool _on;
  volatile int  _brightness;
};
