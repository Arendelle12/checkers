import socket

class Network:
    def __init__(self, server, port):
        self.client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.server = server
        self.port = port
        self.addr = (self.server, self.port)

    def __enter__(self):
        self.client.connect(self.addr)
        return self

    def __exit__(self, *args):
        self.client.close()

    def sendall(self, move):
        byt = bytes(move+"\n", 'utf-8')
        self.client.sendall(byt)

    def recv(self, n):
        byt = self.client.recv(n)
        return byt.decode('utf-8')

    def readline(self):
        text = ""
        while(True):
            data = self.client.recv(1)
            value = data.decode()
            # print(value)
            if value == "\n":
                print('NOWA LINIA')
                return text
            else:
                text = text + value
