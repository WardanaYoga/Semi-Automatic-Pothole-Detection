from ultralytics import YOLO
import cv2
import requests
import time

ESP32_IP = "192.168.137.192"  # GANTI sesuai ESP32
URL = f"http://{ESP32_IP}/detect"

model = YOLO("best.pt")
cap = cv2.VideoCapture("http://192.168.137.44:81/stream")

last_state = "AMAN"
last_send_time = 0
SEND_INTERVAL = 3  # detik

def send_state(state):
    global last_state, last_send_time
    now = time.time()

    if state != last_state or (state == "HOLE" and now - last_send_time > SEND_INTERVAL):
        try:
            requests.post(URL, data={"state": state}, timeout=0.2)
            print(f"[HTTP] Kirim: {state}")
            last_state = state
            last_send_time = now
        except requests.exceptions.RequestException:
            print("ESP32 tidak terhubung")

while True:
    ret, frame = cap.read()
    if not ret:
        break

    results = model(frame, conf=0.35)
    annotated = results[0].plot()

    pothole_detected = False
    if results[0].boxes:
        for box in results[0].boxes:
            cls = int(box.cls[0])
            conf = float(box.conf[0])
            if cls == 0 and conf >= 0.35:
                pothole_detected = True
                break

    send_state("HOLE" if pothole_detected else "AMAN")

    cv2.imshow("Pothole Detection", annotated)
    if cv2.waitKey(1) == 27:
        break

cap.release()
cv2.destroyAllWindows()
