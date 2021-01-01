import socket

class Network:
    def __init__(self, server, port):
        self.client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.server = server
        self.port = port
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