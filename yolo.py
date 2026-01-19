from ultralytics import YOLO
import cv2

# Load YOLO model
model = YOLO("yolo11n.pt")

# ESP32-CAM stream URL (SUDAH TERBUKTI OK)
url = "http://192.168.137.134:81/stream"

# Gunakan backend FFMPEG (WAJIB di Windows)
cap = cv2.VideoCapture(url, cv2.CAP_FFMPEG)

if not cap.isOpened():
    print("ERROR: Tidak bisa membuka stream ESP32-CAM")
    exit()

print("Stream ESP32-CAM berhasil dibuka")

while True:
    ret, frame = cap.read()
    if not ret:
        print("Frame tidak terbaca")
        break

    # OPTIONAL: resize agar YOLO lebih ringan
    frame = cv2.resize(frame, (640, 480))

    # YOLO inference
    results = model.predict(frame, conf=0.4, verbose=False)

    # Gambar bounding box
    if results[0].boxes is not None:
        for box in results[0].boxes.xyxy.cpu().numpy():
            x1, y1, x2, y2 = map(int, box)
            cv2.rectangle(frame, (x1, y1), (x2, y2), (0, 0, 255), 2)

    cv2.imshow("ESP32-CAM YOLO Detection", frame)

    # Tekan Q untuk keluar
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
