#include <Arduino.h>
#include "led.h"


Led::Led(int pin, int pwmChannel, int pwmFreq, int pwmResolution, int brightnessStep)
    : _pin(pin),
      _pwmChannel(pwmChannel),
      _pwmFreq(pwmFreq),
      _pwmResolution(pwmResolution),
      _brightnessStep(brightnessStep),
      _on(false),
      _brightness(128)
{
}

void Led::begin()
{
  ledcSetup(_pwmChannel, _pwmFreq, _pwmResolution);
  ledcAttachPin(_pin, _pwmChannel);
  apply();
}

void Led::on()      { _on = true;  apply(); }
void Led::off()     { _on = false; apply(); }

void Led::brighter()
{
  _brightness += _brightnessStep;
  apply();
}

void Led::dimmer()
{
  _brightness -= _brightnessStep;
  apply();
}

void Led::cycle()
{
  _brightness += _brightnessStep;
  if (_brightness > 255){
    _brightness = 0;
    _on = false;
  }

  if (!_on)
    _on = true;
  apply();
}

bool Led::isOn()          const { return _on; }
int  Led::getBrightness() const { return _brightness; }

void Led::apply()
{
  _brightness = constrain(_brightness, 0, 255);
  ledcWrite(_pwmChannel, _on ? _brightness : 0);
}
