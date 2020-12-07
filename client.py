import socket
import time
from itertools import chain, islice

HOST = "127.0.0.1"
PORT = 1234

#ZAMIANA STRINGA POSTACI 1234 NA LISTE
def str_to_list(test_str):
#test_str = "1234"
    res = []

    for i in test_str:
        res.append(int(i))

    return res

#print(res)

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))

    print('Napisz 6 cyfr')
    string = input()
    #string = "Hello"
    #byt = bytes(string, 'utf-8')

    #s.sendall(b'Hello server')



    byt = bytes(string, 'utf-8')

    s.sendall(byt)
    data = s.recv(1024)

    rec_str = data.decode()

    res = str_to_list(rec_str)

    print('RECEIVED', res)
#print('RECEIVED', data.decode())
#print('RECEIVED', list(data))

time.sleep(10)

#ZAMIANA 2D LIST NA 1D
"""
my_list = [[1,2,3,4],[5,6,7],[8,9,0]]
one_d = list(chain.from_iterable(my_list))
print(one_d)
"""

#ZAMIANA 1D LIST NA 2D
"""
def convert_1d_to_2d(lst, len_lst):
    it = iter(lst)
    return [list(islice(it, i)) for i in len_lst]

lst = [1,2,3,4,5,6,7,8]
len_lst = [4, 4]

res = convert_1d_to_2d(lst, len_lst)
print(res)
print(type(res))
"""



#ZAMIANA LISTY NA STRING POSTACI 1234
"""
my_list = [1, 2, 3, 4]
#zamiana kazdego inta na string
my_string_list = [str(int) for int in my_list]
#polaczenie w jeden string
my_string = "".join(my_string_list)
print(my_string)
"""