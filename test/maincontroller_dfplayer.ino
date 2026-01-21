#include <HardwareSerial.h>
#include <DFRobotDFPlayerMini.h>

HardwareSerial camSerial(2);   // UART2
HardwareSerial mp3Serial(1);   // UART1

DFRobotDFPlayerMini dfplayer;

unsigned long lastPlay = 0;
const unsigned long cooldown = 3000; // 3 detik

void setup() {
  Serial.begin(115200);

  // UART dari ESP32-CAM
  camSerial.begin(115200, SERIAL_8N1, 16, 17); // RX, TX

  // UART ke DFPlayer
  mp3Serial.begin(9600, SERIAL_8N1, 27, 26); // RX, TX

  if (!dfplayer.begin(mp3Serial)) {
    Serial.println("DFPlayer ERROR");
    while (true);
  }

  dfplayer.volume(25); // 0–30
  Serial.println("SYSTEM READY");
}

void loop() {
  if (camSerial.available()) {
    String data = camSerial.readStringUntil('\n');
    data.trim();

    if (data == "POTHOLE") {
      unsigned long now = millis();
      if (now - lastPlay > cooldown) {
        dfplayer.play(1); // 0001.mp3
        lastPlay = now;
        Serial.println("WARNING PLAYED");
      }
    }
  }
}
