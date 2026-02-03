from ultralytics import YOLO
import cv2
import requests
import time

ESP32_IP = "192.168.137.148"   # GANTI sesuai Serial Monitor
URL = f"http://{ESP32_IP}/detect"

model = YOLO("best.pt")
cap = cv2.VideoCapture("http://192.168.137.38:81/stream")

last_state = "AMAN"
last_send = 0
SEND_INTERVAL = 1.0  # detik

while True:
    ret, frame = cap.read()
    if not ret:
        break

    results = model(frame, conf=0.35)
    annotated = results[0].plot()

    # ===== DETEKSI =====
    detected = len(results[0].boxes) > 0
    state = "HOLE" if detected else "AMAN"

    now = time.time()
    if state != last_state or (now - last_send > SEND_INTERVAL):
        try:
            requests.post(URL, data={"state": state}, timeout=0.3)
            last_send = now
            last_state = state
        except:
            pass

    cv2.imshow("Pothole Detection", annotated)
    if cv2.waitKey(1) == 27:
        break

cap.release()
cv2.destroyAllWindows()
