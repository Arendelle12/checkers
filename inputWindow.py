import tkinter as tk
import re

class InputWindow:
    def __init__(self, callback):
        self.callback = callback
        self.root = tk.Tk()
        
        self.box = tk.Canvas(self.root, width = 400, height = 350)
        self.box.pack()
        self.entry1 = tk.Entry(self.root)
        self.entry1.insert(0, "127.0.0.1")
        self.entry2 = tk.Entry(self.root)
        self.entry2.insert(0, "1234")
        self.validationError = tk.StringVar()
        self.window()

    def get_input(self):
        pattern = re.compile("^\d+\.\d+\.\d+\.\d+$")
        host = self.entry1.get()
        port = self.entry2.get()

        if not bool(pattern.match(host)):
            self.validationError.set(f'Host is not IPv4 {host}')
        elif not port.isdigit():
            self.validationError.set(f'Port is not an int {port}')
        else:
            self.root.destroy()
            self.callback(host, int(port))

    def window(self):
        label1 = tk.Label(self.root, text = 'IP ADDRESS')
        self.box.create_window(200, 50, window = label1)
        
        self.box.create_window(200, 100, window = self.entry1)

        label2 = tk.Label(self.root, text = 'PORT')
        self.box.create_window(200, 150, window = label2)

        self.box.create_window(200, 190, window = self.entry2)

        button1 = tk.Button(text="OK", command = self.get_input, bg = 'blue', fg = 'white')
        self.box.create_window(200, 250, window = button1)

        label3 = tk.Label(self.root, textvariable = self.validationError)
        self.box.create_window(200, 290, window = label3)

        self.root.mainloop()
