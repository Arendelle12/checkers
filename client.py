from constants import *
from mappers import *
from board import Board
from network import Network
from time import sleep

my_turn = False

#FPS = 60

#game window
board_2d = []
with Network() as network:
    #narysowanie okna z plansza
    pygame_board = Board()
    pygame_board.draw_squares()

    #clock
    #clock = pygame.time.Clock()
    #run
    #run = True

    #while(True):
        #odebranie informacji 
    player_number = network.recv(1)
    print(player_number)
    if(player_number == "1"):
        my_turn = True

    while(1):
        #clock.tick(FPS)
        rec_str = network.recv(64)

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
            pygame_board.show_text()
            sleep(1)
            pygame_board.draw(board_2d)
            #wyslanie ruchu
            start_field, end_field = pygame_board.get_moves()
            move = move_to_string(start_field, end_field)
            print(move)
            network.sendall(move)
            my_turn = False
