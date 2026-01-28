import cv2

cap = cv2.VideoCapture("http://192.168.137.197:81/stream")

while True:
    ret, frame = cap.read()
    if not ret:
        print("Frame gagal")
        break

    cv2.imshow("ESP32", frame)
    if cv2.waitKey(1) == 27:
        break

cap.release()
cv2.destroyAllWindows()
