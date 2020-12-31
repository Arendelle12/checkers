import socket

HOST = "127.0.0.1"
PORT = 1234

class Network:
    def __init__(self):
        self.client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.server = HOST
        self.port = PORT
        self.addr = (self.server, self.port)

    def __enter__(self):
        print("Network __enter__")
        self.client.connect(self.addr)
        return self

    def __exit__(self, *args):
        print("Network __exit__")
        self.client.close()

    def sendall(self, move):
        byt = bytes(move, 'utf-8')
        self.client.sendall(byt)

    def recv(self, n):
        byt = self.client.recv(n)
        return byt.decode('utf-8')