#include <WiFi.h>
#include <WebServer.h>
#include <DFRobotDFPlayerMini.h>

const char* ssid = "AI-CENTER";
const char* password = "aicenter";

WebServer server(80);
DFRobotDFPlayerMini dfplayer;

// UART2 pin (SUDAH TERBUKTI)
#define DF_RX 25
#define DF_TX 26

bool hole_active = false;
unsigned long last_play = 0;
const unsigned long PLAY_INTERVAL = 3000; // 3 detik

void handleDetect() {
  if (!server.hasArg("state")) {
    server.send(400, "text/plain", "Missing state");
    return;
  }

  String state = server.arg("state");
  hole_active = (state == "HOLE");

  server.send(200, "text/plain", "OK");
}

void setup() {
  Serial.begin(115200);

  // DFPlayer
  Serial2.begin(9600, SERIAL_8N1, DF_RX, DF_TX);
  if (!dfplayer.begin(Serial2)) {
    Serial.println("DFPlayer gagal");
    while (1);
  }
  dfplayer.volume(25);

  // WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");
  Serial.println(WiFi.localIP());

  // HTTP endpoint
  server.on("/detect", HTTP_POST, handleDetect);
  server.begin();
}

void loop() {
  server.handleClient();

  if (hole_active && millis() - last_play > PLAY_INTERVAL) {
    dfplayer.play(1);   // /mp3/0001.mp3
    last_play = millis();
  }
}
