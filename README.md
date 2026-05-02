# ESP32 Devboard Base

A modular ESP32 development board firmware suite with OLED display, button navigation, and sensor/actuator support.

## Hardware

- **ESP32 DevKit** (or compatible)
- **SSD1306** 0.96" OLED display (I2C: SDA=5, SCL=4)
- **3 tactile buttons** (GPIO 34, 36, 39 — input-only pins)
- **WS2812 RGB LED** (GPIO 27)

## Firmware Sketches

### `esp32-devboard-base.ino`
Base firmware for general ESP32 dev board testing.

**Features:**
- Menu-driven UI (Up/Down to navigate, Select to select)
- GPIO testing — toggle 17 safe output pins on/off with state tracking
- LED color test — Red, Green, Blue, White, Rainbow, Off
- Board info — displays chip model, flash size, MAC address
- Non-blocking button edge detection with debounce
- Serial debug at 115200 baud

**Test Pins (H1):** IO23, IO19, IO18, IO25, IO26  
**Test Pins (H2):** IO13, IO12, IO14, IO15, IO21, IO22, IO17, IO16  
**Test Pins (H3):** IO33, IO32, IO2, IO0

### `esp32_pico_gamer/`
Arduino/C++ port of the Pico Gamer project for this ESP32 devboard.

**Features:**
- Launcher for 52 128x64 OLED games, from Pico ports to new micro arcade, puzzle, shooter, board, racing, and reflex games
- Modular game engine with display helpers, debounced button input, and game lifecycle helpers
- Three-button smart controls: Left/Right move or navigate, Select starts/actions, Select long press exits to launcher
- Silent gameplay: no PWM audio, no buzzer pin, no tone output
- RGB LED feedback for menu movement, collisions, scores, crashes, and landings

**Controls:**
- GPIO 34 — Left
- GPIO 36 — Right
- GPIO 39 — Select

**Game Catalog:**
- Pico ports: Pong, Snake, Full Speed, Lunar Module
- Arcade/action: Breakout, Flappy Pico, Dino Runner, Jetpack, Dodge Rain, Catch Star, Basket Catch, Balloon Pop, Cave Flyer, Tunnel Run, Wall Bounce, Gravity Flip, Platform Hop, Brick Drop
- Motion/racing: Lane Racer, Traffic Dodge, Ski Slalom, Boat Slalom, Rail Runner, Road Drift
- Shooters: Asteroids, Invaders, Missile Cmd, Turret Def, UFO Defender, Meteor Blast
- Puzzles: Lights Out, Minefield, Sokoban, Sliding, Memory, Simon, Mastermind, Number Guess, Flood Fill, Box Push, Laser Mirror
- Board/reflex: Tic Tac Toe, Connect Four, Nim, Dots Boxes, Reaction, Quick Draw, Stop Bar, Stack Tower, Lock Pick, Pixel Whack, Pulse Match

### `cypher_flock_esp32_devkit/`
WiFi device detector firmware for the custom devkit board.

**Features:**
- Promiscuous WiFi sniffer with channel hopping
- OUI-based device detection (30+ Flock camera prefixes)
- SSID keyword matching (configurable "flock" detection)
- Probe request wildcard detection
- RSSI threshold filtering
- Dual alert system — buzzer + LED with distinct sounds
- New device chirp (ascending 2-tone) vs. heartbeat beep (monotone)
- SPIFFS persistence — saves detection session to JSON
- 4-page OLED UI with menu toggle

**Channel Modes:**
- `CHANNEL_MODE_FULL_HOP` — cycles all 1-11
- `CHANNEL_MODE_CUSTOM` — hops 1, 6, 11 (default)
- `CHANNEL_MODE_SINGLE` — locks to one channel

**Controls:**
- Up/Down — page/navigate
- Select short press — toggle menu
- Select long press (800ms) — pause/resume scanning

### `display_test/`
Minimal I2C scanner and SSD1306 display diagnostic.

## Dependencies

Install via Arduino IDE Library Manager:

```
Adafruit GFX
Adafruit SSD1306
FastLED
```

## Compile & Flash

```bash
# Detect serial port
arduino-cli board list

# Compile (devkit profile)
arduino-cli compile \
  --fqbn esp32:esp32:esp32 \
  --build-property build.extra_flags='-DESP32 -DBOARD_PROFILE=CYPHER_FLOCK_DEVKIT'

# Flash
arduino-cli upload -p /dev/ttyUSB0 \
  --fqbn esp32:esp32:esp32 \
  --build-property build.extra_flags='-DESP32 -DBOARD_PROFILE=CYPHER_FLOCK_DEVKIT'

# Monitor
arduino-cli monitor -p /dev/ttyUSB0 -b 115200
```

For `esp32-devboard-base.ino`, omit `--build-property` and flash directly:

```bash
arduino-cli compile -p /dev/ttyUSB0 --fqbn esp32:esp32:esp32
arduino-cli upload -p /dev/ttyUSB0 --fqbn esp32:esp32:esp32
```

For the Pico Gamer port:

```bash
arduino-cli compile --fqbn esp32:esp32:esp32 esp32_pico_gamer
arduino-cli upload -p /dev/ttyUSB0 --fqbn esp32:esp32:esp32 esp32_pico_gamer
```

## Pin Reference

| Pin | Function |
|-----|----------|
| 4 | OLED SCL |
| 5 | OLED SDA |
| 27 | WS2812 RGB LED |
| 34 | Button Left / Up |
| 36 | Button Center / Down |
| 39 | Button Right / Select |

## License

MIT
