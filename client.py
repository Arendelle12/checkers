import socket
import time

HOST = "127.0.0.1"
PORT = 1234

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))

    print('Napisz cos')
    string = input()
    byt = bytes(string, 'utf-8')

    #s.sendall(b'Hello server')
    s.sendall(byt)
    data = s.recv(1024)

print('RECEIVED', data.decode())

time.sleep(10)