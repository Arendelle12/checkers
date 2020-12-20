import socket
import time
from itertools import chain, islice

HOST = "127.0.0.1"
PORT = 1234

len_lst = [8, 8, 8, 8, 8, 8, 8, 8]

#ZAMIANA STRINGA POSTACI 1234 NA LISTE
def str_to_list(test_str):
#test_str = "1234"
    res = []

    for i in test_str:
        res.append(int(i))

    return res

#print(res)

#ZAMIANA 1D LIST NA 2D

def convert_1d_to_2d(lst, len_lst):
    it = iter(lst)
    return [list(islice(it, i)) for i in len_lst]

#lst = [1,2,3,4,5,6,7,8]


#res = convert_1d_to_2d(lst, len_lst)
#print(res)
#print(type(res))


with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))

    #while(True):
        #odebranie informacji 
    player = s.recv(1)
    players = player.decode()
    print(players)

    data = s.recv(64)
    
    
    rec_str = data.decode()
    print("Otrzymane na poczatku")
    print(rec_str)

    if(rec_str == "Twoj ruch\x00"):
        print("TRUE: ", rec_str)

        #niby wyslanie ruchu
        print('Napisz 4 cyfry - ruch')
        string = input()
        #zamiana string na bytes
        byt = bytes(string, 'utf-8')
        s.sendall(byt)
        


    else:
        print("Ruch przeciwnika")


#PIERWSZE ODEBRANIE PLANSZY
        

    """
    res = str_to_list(rec_str)
    res = convert_1d_to_2d(res, len_lst)
    print('RECEIVED')
    for i in range(8):
        print(res[i])
    


    #WYSLANIE NIBY RUCHU

    
    #string = "Hello"
    #byt = bytes(string, 'utf-8')

    #s.sendall(b'Hello server')



   

    #DRUGIE ODEBRANIE PLANSZY
    data = s.recv(1024)

    rec_str = data.decode()

    res = str_to_list(rec_str)
    res = convert_1d_to_2d(res, len_lst)
    print('RECEIVED')
    for i in range(8):
        print(res[i])
        """

    
#print('RECEIVED', data.decode())
#print('RECEIVED', list(data))

time.sleep(10)

#ZAMIANA 2D LIST NA 1D
"""
my_list = [[1,2,3,4],[5,6,7],[8,9,0]]
one_d = list(chain.from_iterable(my_list))
print(one_d)
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