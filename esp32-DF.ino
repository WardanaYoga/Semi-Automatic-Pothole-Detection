#include <WiFi.h>
#include <WebServer.h>
#include <DFRobotDFPlayerMini.h>

const char* ssid = "AI-CENTER";
const char* password = "aicenter";

WebServer server(80);

HardwareSerial dfSerial(2);
DFRobotDFPlayerMini dfplayer;

void handleDetect() {
  if (!server.hasArg("state")) {
    server.send(400, "text/plain", "Missing state");
    return;
  }

  String state = server.arg("state");

  if (state == "HOLE") {
    dfplayer.play(1);
    Serial.println("HOLE diterima");
  }

  server.send(200, "text/plain", "OK");
}

void setup() {
  Serial.begin(115200);

  // DFPlayer
  dfSerial.begin(9600, SERIAL_8N1, 16, 17);
  dfplayer.begin(dfSerial);
  dfplayer.volume(25);

  // WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");
  Serial.println(WiFi.localIP());

  server.on("/detect", HTTP_POST, handleDetect);
  server.begin();
}

void loop() {
  server.handleClient();
}
