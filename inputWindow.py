import tkinter as tk
import re

class InputWindow:
    def __init__(self, callback):
        self.callback = callback
        self.root = tk.Tk()
        self.box = tk.Canvas(self.root, width = 400, height = 300)
        self.box.pack()
        self.entry1 = tk.Entry(self.root)
        self.entry2 = tk.Entry(self.root)
        self.window()
        self.host = None
        self.port = None

    def get_input(self):
        pattern = re.compile("^\d+\.\d+\.\d+\.\d+$")
        self.host = self.entry1.get()
        self.port = self.entry2.get()

        h = self.entry1.get()
        p = self.entry2.get()


        if not bool(pattern.match(self.host)):
            label3 = tk.Label(self.root, text = f'Host is not IPv4 {self.host}')
            self.box.create_window(200, 250, window = label3)
        elif not self.port.isdigit():
            label3 = tk.Label(self.root, text = f'Port is not an int {self.port}')
            self.box.create_window(200, 250, window = label3)
        else:
            self.port = int(self.port)
            print(f"wartosci {h}, {p}")
            self.callback(h, int(p), lambda: self.root.quit())

    def close_window(self):
        self.root.destroy()

    def window(self):
        label1 = tk.Label(self.root, text = 'IP ADDRESS')
        self.box.create_window(200, 50, window = label1)
        
        self.box.create_window(200, 80, window = self.entry1)

        label2 = tk.Label(self.root, text = 'PORT')
        self.box.create_window(200, 130, window = label2)

        self.entry2 = tk.Entry(self.root)
        self.box.create_window(200, 150, window = self.entry2)

        button1 = tk.Button(text="OK", command = self.get_input, bg = 'blue', fg = 'white')
        self.box.create_window(150, 200, window = button1)

        self.root.mainloop()

    def get_values(self):
        return self.host, self.port


# i = InputWindow()

# for i in range(100):
#     print(i)