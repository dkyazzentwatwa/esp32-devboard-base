// Minimal SSD1306 display diagnostic
// Scans I2C bus, tries both addresses, draws text
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
bool displayEnabled = false;

void setup() {
  Wire.begin(5, 4);  // SDA=5, SCL=4 (per PCB routing)
  Serial.begin(115200);
  delay(2000);
  Serial.println("=== Display Test ===");

  // I2C bus scan
  Serial.println("I2C scan:");
  for (uint8_t addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
      Serial.printf("  Found: 0x%02X\n", addr);
    }
  }

  // Try both addresses
  bool found = false;
  uint8_t foundAddr = 0;
  if (display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    found = true; foundAddr = 0x3C;
  } else if (display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) {
    found = true; foundAddr = 0x3D;
  }

  if (!found) {
    Serial.println("ERROR: No display found");
    return;
  }
  Serial.printf("Display OK at 0x%02X\n", foundAddr);

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(2);
  display.setCursor(0, 0);
  display.println("HELLO!");

  display.setTextSize(1);
  display.setCursor(0, 20);
  display.printf("Addr: 0x%02X", foundAddr);
  display.setCursor(0, 30);
  display.println("Display working :)");

  display.display();
  Serial.println("Done.");
}

void loop() {}
