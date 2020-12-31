import tkinter as tk 

# root = tk.Tk()

# box = tk.Canvas(root, width = 400, height = 300)
# box.pack()

# label1 = tk.Label(root, text = 'IP ADDRESS')
# box.create_window(200, 50, window = label1)

# entry1 = tk.Entry(root)
# box.create_window(200, 80, window = entry1)

# label2 = tk.Label(root, text = 'PORT')
# box.create_window(200, 130, window = label2)

# entry2 = tk.Entry(root)
# box.create_window(200, 150, window = entry2)

# def get_input():
#     host = entry1.get()
#     port = entry2.get()

#     label3 = tk.Label(root, text = 'Your IP address is ' + host + ' and your port is ' + port)
#     box.create_window(200, 250, window = label3)

#     print(host + ' ' + port)

# def close_window():
#     root.destroy()

# button1 = tk.Button(text="OK", command = get_input, bg = 'blue', fg = 'white')
# box.create_window(150, 200, window = button1)

# button2 = tk.Button(text = "Close", command = close_window, bg = 'red', fg = 'white')
# box.create_window(250, 200, window = button2)

# root.mainloop()

class Input:
    def __init__(self):
        self.root = tk.Tk()
        self.box = tk.Canvas(self.root, width = 400, height = 300)
        self.box.pack()
        self.entry1 = tk.Entry(self.root)
        self.entry2 = tk.Entry(self.root)
        self.window()
        self.host
        self.port

    def get_input(self):
        self.host = self.entry1.get()
        self.port = self.entry2.get()

        label3 = tk.Label(self.root, text = 'Your IP address is ' + self.host + ' and your port is ' + self.port)
        self.box.create_window(200, 250, window = label3)

        print(self.host + ' ' + self.port)

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

        button2 = tk.Button(text = "Close", command = self.close_window, bg = 'red', fg = 'white')
        self.box.create_window(250, 200, window = button2)

        self.root.mainloop()

    def get_values(self):
        return self.host, self.port

i = Input()
a, b = i.get_values()
print(a)
print(b)