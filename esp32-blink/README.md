# ESP32 Blink

A bare-metal C++ blink project for the ESP32 DevKit V1, built with PlatformIO.  
Demonstrates a non-blocking superloop, ISR-based button input, and a three-mode LED state machine.

---

## Features

| Feature | Detail |
|---|---|
| Non-blocking blink | `millis()`-based, no `delay()` |
| Button modes | Blink → AlwaysOn → AlwaysOff → … |
| ISR + debounce | Interrupt flag set in ISR; debounce in `loop()` |
| Loop timing | Prints average µs/iteration over Serial |

---

## Hardware

**No external components needed.** The project uses only the built-in peripherals on the ESP32 DevKit V1.

| Pin | Function | Notes |
|---|---|---|
| GPIO 2 | Built-in blue LED | Active HIGH |
| GPIO 0 | BOOT button | Active LOW, internal pull-up |

### Board Schematic

```
                    ESP32 DevKit V1
                  ┌───────────────────┐
           3V3 ── │ 3V3           GND │ ── GND
            EN ── │ EN            IO23│
           IO34── │ IO34          IO22│
           IO35── │ IO35           TX0│
           IO32── │ IO32           RX0│
           IO33── │ IO33          IO21│
           IO25── │ IO25           IO19│
           IO26── │ IO26           IO18│
           IO27── │ IO27            IO5│
           IO14── │ IO14           IO17│
           IO12── │ IO12           IO16│
           GND ── │ GND             IO4│
           IO13── │ IO13            IO0│ ◄─── BOOT button (GPIO 0)
           IO9 ── │ IO9             IO2│ ◄─── Built-in LED (GPIO 2)
           IO10── │ IO10           IO15│
           IO11── │ IO11            SD1│
            SD3── │ SD3             SD0│
            SD2── │ SD2             CLK│
            CMD── │ CMD             SD2│
          +5V ──  │ 5V              SD3│
                  └───────────────────┘
                         │
                       [USB]
                    (power + serial)
```

### What to assemble

1. Connect the ESP32 DevKit V1 to your computer via a **Micro-USB cable**.
2. That's it — no breadboard, no external LEDs, no resistors needed.

> If you want to wire an **external LED** on a breadboard instead, connect:  
> `GPIO 2 → 220 Ω resistor → LED anode → LED cathode → GND`

---

## Build & Flash

**Prerequisites:** [PlatformIO CLI](https://platformio.org/install/cli) or the PlatformIO IDE extension for VS Code.

```bash
# Build
pio run

# Flash
pio run --target upload

# Open Serial Monitor (115200 baud)
pio device monitor
```

---

## Usage

After flashing, the built-in LED starts **blinking at 500 ms intervals**.  
Press the **BOOT button** to cycle through modes:

```
[Blink] ──► [AlwaysOn] ──► [AlwaysOff] ──► [Blink] ──► …
```

Serial output (115200 baud) logs mode transitions and loop timing:

```
[Setup] ESP32 Embedded C++ ready
  LED_PIN=2  BTN_PIN=0  BLINK=500ms  DEBOUNCE=50ms
[Mode] AlwaysOn
[Mode] AlwaysOff
[Mode] Blink
[LoopTimer] 1000 iters, avg = 3 µs/iter
```

---

## Configuration

All tuneable constants live in `Config` at the top of `src/main.cpp`:

| Constant | Default | Description |
|---|---|---|
| `LED_PIN` | `2` | GPIO pin for the LED |
| `BUTTON_PIN` | `0` | GPIO pin for the button |
| `BLINK_INTERVAL_MS` | `500` | Toggle period in milliseconds |
| `DEBOUNCE_MS` | `50` | Button debounce window |
| `LOOP_LOG_EVERY` | `1000` | Serial timing log interval (iterations) |

---

## Project Structure

```
esp32-blink/
├── src/
│   └── main.cpp        # All source code
├── platformio.ini      # Board + build config
└── README.md
```

---

## Build Flags

```ini
-std=gnu++17
-Wall
-Wextra
-Werror=return-type
```
