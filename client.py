import socket
import time

HOST = "127.0.0.1"
PORT = 1234

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))
    s.sendall(b'Hello server')
    data = s.recv(1024)

print('RECEIVED', data.decode())

time.sleep(10)
