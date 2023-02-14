import socket
import json

if __name__ == "__main__":
  ip = "localhost"
  port = 40001

  sender = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
  message = {"1": 3.0}
  sender.sendto(bytes(json.dumps(message), "utf-8"), (ip, port))

  print(f"[INFO] Sent message {message} to {ip}:{port}")