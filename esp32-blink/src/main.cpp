#include <Arduino.h>

// ─── Configuration ────────────────────────────────────────────────────────────
// All "magic numbers" live here — never scatter literals through the code.
struct Config {
    static constexpr uint8_t  LED_PIN            = 17;    // built-in LED on ESP32 DevKit
    static constexpr uint8_t  BUTTON_PIN         = 0;    // BOOT button on ESP32 DevKit
    static constexpr uint32_t BLINK_INTERVAL_MS  = 500;
    static constexpr uint8_t  BLINKS_SHORT_PRESS = 3;    // acknowledgement flashes (reserved)
    static constexpr uint32_t DEBOUNCE_MS        = 50;
    static constexpr uint32_t LOOP_LOG_EVERY     = 1000; // print timing every N iterations
};

// ─── LED abstraction ──────────────────────────────────────────────────────────
enum class LedState : uint8_t { Off, On };
enum class LedMode  : uint8_t { Blink, AlwaysOn, AlwaysOff };

class Led {
public:
    void init() {
        pinMode(Config::LED_PIN, OUTPUT);
        set(LedState::On);  // start with LED on for visual confirmation of boot
    }

    void set(LedState state) {
        _state = state;
        digitalWrite(Config::LED_PIN, state == LedState::On ? HIGH : LOW);
    }

    void toggle() { set(_state == LedState::On ? LedState::Off : LedState::On); }

    LedState state() const { return _state; }

private:
    LedState _state = LedState::Off;
};

// ─── Non-blocking blinker (superloop-safe) ────────────────────────────────────
class Blinker {
public:
    // Call every loop() iteration; toggles LED when interval elapses.
    void update(Led& led) {
        const uint32_t now = millis();
        if (now - _lastToggleMs >= Config::BLINK_INTERVAL_MS) {
            _lastToggleMs = now;
            led.toggle();
        }
    }

    void reset(Led& led) {
        _lastToggleMs = millis();
        led.set(LedState::Off);
    }

private:
    uint32_t _lastToggleMs = 0;
};

// ─── Button with interrupt + software debounce ────────────────────────────────
class Button {
public:
    void init() {
        pinMode(Config::BUTTON_PIN, INPUT_PULLUP);
        attachInterrupt(digitalPinToInterrupt(Config::BUTTON_PIN), isrHandler, FALLING);
    }

    // Returns true once per debounced press. Safe to call every loop() iteration.
    bool consumePress() {
        if (!_pressed) return false;

        const uint32_t now = millis();
        if (now - _lastPressMs < Config::DEBOUNCE_MS) {
            _pressed = false;   // discard — noise within debounce window
            return false;
        }

        _pressed     = false;
        _lastPressMs = now;
        Serial.println("[Button] Press detected");
        return true;
    }

    // ISR: declared here, defined out-of-line below to avoid ESP32-S3 l32r relocation error.
    static void IRAM_ATTR isrHandler();

private:
    static volatile bool _pressed;
    uint32_t             _lastPressMs = 0;
};

volatile bool Button::_pressed = false;
void IRAM_ATTR Button::isrHandler() { _pressed = true; }


// ─── File-scope state (no namespace pollution, accessible to setup/loop) ──────
static Led       led;
static Button    button;
static Blinker   blinker;
static LedMode   mode = LedMode::Blink;

// ─── Mode transition: Blink → AlwaysOn → AlwaysOff → Blink ──────────────────
static void cycleMode() {
    switch (mode) {
case LedMode::Blink:
            mode = LedMode::AlwaysOn;
            led.set(LedState::On);
            Serial.println("[Mode] AlwaysOn");
            break;
        case LedMode::AlwaysOn:
            mode = LedMode::AlwaysOff;
            led.set(LedState::Off);
            Serial.println("[Mode] AlwaysOff");
            break;
        case LedMode::AlwaysOff:
            mode = LedMode::Blink;
            blinker.reset(led);
            Serial.println("[Mode] Blink");
            break;
    }
}

// ─── Arduino entry points ─────────────────────────────────────────────────────
void setup() {
    Serial.begin(115200);
    led.init();
    button.init();

    Serial.println("[Setup] ESP32 Embedded C++ ready");
    Serial.printf("  LED_PIN=%u  BTN_PIN=%u  BLINK=%lums  DEBOUNCE=%lums\n",
                  Config::LED_PIN, Config::BUTTON_PIN,
                  static_cast<unsigned long>(Config::BLINK_INTERVAL_MS),
                  static_cast<unsigned long>(Config::DEBOUNCE_MS));
}

void loop() {
    if (button.consumePress()) {
        cycleMode();            // button logic handled here, NOT inside ISR
    }

    if (mode == LedMode::Blink) {
        blinker.update(led);    // non-blocking — no delay()
    }
}
