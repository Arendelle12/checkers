from itertools import islice
from constants import *
from board import Board
from network import Network

len_lst = [8, 8, 8, 8, 8, 8, 8, 8]

my_turn = False

#FPS = 60

#game window


#ZAMIANA STRINGA POSTACI 1234 NA LISTE
def str_to_list(test_str):
    return [int(i) for i in test_str]

#ZAMIANA 1D LIST NA 2D

def convert_1d_to_2d(lst, len_lst):
    it = iter(lst)
    return [list(islice(it, i)) for i in len_lst]

#return 2 dimensional list
def convert_board(rec_board):
    res = str_to_list(rec_board)
    res2d = convert_1d_to_2d(res, len_lst)
    for i in range(8):
        print(res2d[i])
    return res2d

def move_to_string(start, end):
    move = start + end
    #lista string
    data = [str(x) for x in move] 
    send_str = ''.join(data)
    return send_str


with Network() as s:
    #narysowanie okna z plansza
    pygame_board = Board()
    pygame_board.draw_squares()

    #clock
    #clock = pygame.time.Clock()
    #run
    #run = True

    #while(True):
        #odebranie informacji 
    player_number = s.recv(1)
    print(player_number)
    if(player_number == "1"):
        my_turn = True


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
            board_2d = convert_board(rec_str)
            pygame_board.draw(board_2d)


        if(my_turn == True):

            #niby wyslanie ruchu
            print('Napisz 4 cyfry - ruch')
            start_tuple, end_tuple = pygame_board.get_moves()
            string = move_to_string(start_tuple, end_tuple)
            print(string)
            s.sendall(string)
            my_turn = False
        