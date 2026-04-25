import serial
import time

try:
    ser = serial.Serial('/dev/cu.usbserial-110', 115200, timeout=1)
    time.sleep(3) # Wait for ESP to start and scan
    lines_read = 0
    print("Reading serial output...")
    while lines_read < 10:
        line = ser.readline().decode('utf-8', errors='ignore').strip()
        if line:
            print(line)
            lines_read += 1
    ser.close()
except Exception as e:
    print("Error:", e)
