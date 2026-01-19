#include "esp_camera.h"

// ===================
// PIN KAMERA AI THINKER
// ===================
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

// ===================
#define DARK_THRESHOLD 90
#define VAR_THRESHOLD  25

int hole_counter = 0;

// ===================
void setup() {
  Serial.begin(115200);      // debug
  Serial1.begin(115200);     // UART ke ESP32
  Serial.println("ESP32-CAM START");

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer   = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_GRAYSCALE;
  config.frame_size = FRAMESIZE_QQVGA;
  config.jpeg_quality = 12;
  config.fb_count = 1;

  if (esp_camera_init(&config) != ESP_OK) {
    Serial.println("Camera init FAILED");
    while (true);
  }

  Serial.println("Camera init OK");
}

// ===================
void loop() {
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) return;

  uint8_t *img = fb->buf;
  int w = fb->width;
  int h = fb->height;

  int y_start = h * 0.7;
  long sum = 0, sum_sq = 0;
  int count = 0;

  for (int y = y_start; y < h; y += 2) {
    for (int x = 0; x < w; x += 2) {
      int p = img[y * w + x];
      sum += p;
      count++;
    }
  }

  int avg = sum / count;

  for (int y = y_start; y < h; y += 2) {
    for (int x = 0; x < w; x += 2) {
      int p = img[y * w + x];
      sum_sq += (p - avg) * (p - avg);
    }
  }

  int var = sum_sq / count;

  bool hole = false;
  if (avg < DARK_THRESHOLD && var > VAR_THRESHOLD) {
    hole_counter++;
  } else {
    hole_counter = 0;
  }

  if (hole_counter >= 3) hole = true;

  if (hole) {
    Serial1.println("HOLE:1");
    Serial.println("HOLE DETECTED");
  } else {
    Serial1.println("HOLE:0");
  }

  esp_camera_fb_return(fb);
  delay(150);
}
