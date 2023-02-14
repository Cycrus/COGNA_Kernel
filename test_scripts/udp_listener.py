import socket
import json

if __name__ == "__main__":
  ip = "localhost"
  port = 40002

  receiver = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
  receiver.bind((ip, port))

  print(f"[INFO] Starting to listen at {ip}:{port}.")
  try:
    while True:
      data, addr = receiver.recvfrom(1024)
      print(f"[INFO] received message from {addr}.")
      print(json.loads(data))
  except KeyboardInterrupt:
    print("[INFO] User exit.")