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

  const int _pin;
  const int _pwmChannel;
  const int _pwmFreq;
  const int _pwmResolution;
  const int _brightnessStep;

  volatile bool _on;
  volatile int  _brightness;
};
