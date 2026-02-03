#include <DFRobotDFPlayerMini.h>

DFRobotDFPlayerMini df;

void setup() {
  Serial.begin(9600); // sama dengan DFPlayer

  if (!df.begin(Serial)) {
    // jika gagal, cek wiring
    while (true);
  }

  df.volume(20);
  df.play(1);
}

void loop() {}
