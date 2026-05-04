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
Arduino/C++ port of the Pico Gamer project for the ESP32 devkit and Waveshare ESP32-S3-Touch-AMOLED-1.8.

**Features:**
- Launcher for 52 games, from Pico ports to new micro arcade, puzzle, shooter, board, racing, and reflex games
- Same sketch supports SSD1306 devkit and native 368x448 AMOLED profiles
- Modular game engine with Adafruit_GFX display helpers, debounced button/touch input, and game lifecycle helpers
- Devkit controls: Left/Right move or navigate, Select starts/actions, Select long press exits to launcher
- AMOLED controls: left/right touch zones move or navigate, center tap selects/actions, BOOT long press exits to launcher
- Silent gameplay: no PWM audio, no buzzer pin, no tone output
- RGB LED feedback for menu movement, collisions, scores, crashes, and landings

**Devkit Controls:**
- GPIO 34 — Left
- GPIO 36 — Right
- GPIO 39 — Select

**AMOLED Controls:**
- Left screen zone — Left
- Right screen zone — Right
- Center screen zone — Select/action
- GPIO 0 BOOT — long press exits a running game

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
Adafruit XCA9554
FastLED
GFX Library for Arduino
XPowersLib
```

For the AMOLED touch profile, install Waveshare's offline `Arduino_DriveBus` package from the ESP32-S3-Touch-AMOLED-1.8 Arduino sample bundle. In `arduino-cli lib list` it may appear as `Driver Bus Library Based on Arduino`.

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

For the Pico Gamer port on the SSD1306 devkit:

```bash
arduino-cli compile --fqbn esp32:esp32:esp32 esp32_pico_gamer
arduino-cli upload -p /dev/ttyUSB0 --fqbn esp32:esp32:esp32 esp32_pico_gamer
```

For the Pico Gamer port on the Waveshare ESP32-S3-Touch-AMOLED-1.8:

```bash
arduino-cli compile \
  --fqbn 'esp32:esp32:esp32s3:FlashSize=16M,PSRAM=opi,USBMode=default,CDCOnBoot=cdc,PartitionScheme=app3M_fat9M_16MB' \
  --build-property build.extra_flags='-DESP32 -DGAMER_BOARD_PROFILE=GAMER_BOARD_WAVESHARE_AMOLED_18' \
  esp32_pico_gamer
```

The AMOLED board uses native USB. For upload, touch the current runtime port at 1200 baud, wait for it to re-enumerate, then upload to the new bootloader port:

```bash
python3 -c "import serial,time; s=serial.Serial('/dev/cu.usbmodemXXXX',1200); time.sleep(0.1); s.close()"
sleep 2
arduino-cli board list
arduino-cli upload \
  -p /dev/cu.usbmodemYYYY \
  --fqbn 'esp32:esp32:esp32s3:FlashSize=16M,PSRAM=opi,USBMode=default,CDCOnBoot=cdc,PartitionScheme=app3M_fat9M_16MB' \
  --build-property build.extra_flags='-DESP32 -DGAMER_BOARD_PROFILE=GAMER_BOARD_WAVESHARE_AMOLED_18' \
  esp32_pico_gamer
arduino-cli monitor -p /dev/cu.usbmodemYYYY -c baudrate=115200
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

### Waveshare ESP32-S3-Touch-AMOLED-1.8

| Pin | Function |
|-----|----------|
| 4/5/6/7 | SH8601 QSPI data |
| 11 | SH8601 QSPI SCLK |
| 12 | SH8601 QSPI CS |
| 14 | FT3168 / XCA9554 I2C SCL |
| 15 | FT3168 / XCA9554 I2C SDA |
| 21 | FT3168 touch interrupt |
| 0 | BOOT exit button |

## License

MIT
