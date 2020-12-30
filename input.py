import tkinter as tk 

root = tk.Tk()

box = tk.Canvas(root, width = 400, height = 300)
box.pack()

label1 = tk.Label(root, text = 'IP ADDRESS')
box.create_window(200, 50, window = label1)

entry1 = tk.Entry(root)
box.create_window(200, 80, window = entry1)

label2 = tk.Label(root, text = 'PORT')
box.create_window(200, 130, window = label2)

entry2 = tk.Entry(root)
box.create_window(200, 150, window = entry2)

def get_input():
    host = entry1.get()
    port = entry2.get()

    label3 = tk.Label(root, text = 'Your IP address is ' + host + ' and your port is ' + port)
    box.create_window(200, 250, window = label3)

    print(host + ' ' + port)

def close_window():
    root.destroy()

button1 = tk.Button(text="OK", command = get_input, bg = 'blue', fg = 'white')
box.create_window(150, 200, window = button1)

button2 = tk.Button(text = "Close", command = close_window, bg = 'red', fg = 'white')
box.create_window(250, 200, window = button2)

root.mainloop()