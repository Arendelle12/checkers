import socket
import time
import pygame
from itertools import chain, islice
from constants import *
from board import Board
from network import Network

HOST = "127.0.0.1"
PORT = 1234

len_lst = [8, 8, 8, 8, 8, 8, 8, 8]

my_turn = False

#FPS = 60
pygame.init()
#game window
WINDOW = pygame.display.set_mode((WIDTH, HEIGHT))
pygame.display.set_caption("Checkers")

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

#return 2 dimensional list
def show_board(rec_board):
    #board = s.recv(64)
    #rec_board = board.decode('utf-8')
    res = str_to_list(rec_board)
    res2d = convert_1d_to_2d(res, len_lst)
    for i in range(8):
        print(res2d[i])
    return res2d

#pos is a tuple
def get_row_col_from_mouse(pos):
    x, y = pos
    row = int(y // SQUARE_SIZE)
    col = int(x // SQUARE_SIZE)
    return row, col

def move_to_string(start, end):
    move = start + end
    #lista string
    data = [str(x) for x in move] 
    send_str = ''.join(data)
    return send_str

#res = convert_1d_to_2d(lst, len_lst)
#print(res)
#print(type(res))

with Network() as s:
    #narysowanie okna z plansza
    pygame_board = Board()
    pygame_board.draw_squares(WINDOW)
    pygame.display.update()

    #clock
    #clock = pygame.time.Clock()
    #run
    #run = True

    #while(True):
        #odebranie informacji 
    player_number = s.recv(1)
    print(player_number)

    rec_board = s.recv(64)
    print("Otrzymana plansza")
    #print(rec_board)
    board_2d = show_board(rec_board)

    #narysowanie pionkow
    pygame_board.create_board(board_2d)

    pygame_board.draw(WINDOW)
    pygame.display.update()
    #print(board_2d)



    if(player_number == "1"):
        print("Wszedlem w if numer gracza")
        #niby wyslanie ruchu
        print("Nacisnij na plansze")

        start_tuple, end_tuple = pygame_board.get_moves()

        print(start_tuple, end_tuple)

        print('Napisz 4 cyfry - ruch')

        #string = input()

        #string z pozycji na planszy
        string = move_to_string(start_tuple, end_tuple)
        print(string)
        #zamiana string na bytes
        s.sendall(string)



    while(1):
        #clock.tick(FPS)
        rec_str = s.recv(64)

        """
        if(run == True):
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    run = False

        if(run == False):
            pygame.quit() 
        """      

        if(rec_str == "Twoj ruch\x00"):
            my_turn = True
            print("TRUE: ", rec_str)

        else:
            print("Otrzymano wiadomosc: ")
            show_board(rec_str)

        if(my_turn == True):

            #niby wyslanie ruchu
            print('Napisz 4 cyfry - ruch')
            string = input()
            #zamiana string na bytes
            s.sendall(string)
            my_turn = False
        


    #else:
    #    print("Ruch przeciwnika")


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