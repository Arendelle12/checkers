from constants import *
from mappers import *
from board import Board
from network import Network
from inputWindow import InputWindow
from time import sleep


def run_game(host, port):
    my_turn = False

    #game window
    board_2d = []
    with Network(host, port) as network:
        #narysowanie okna z plansza
        pygame_board = Board()
        pygame_board.draw_squares()

        player_number = network.readline()
        print(player_number)
        if(player_number == "1"):
            my_turn = True

        while(1):
            pygame_board.tick()
            
            rec_str = network.readline()

            """
            if(run == True):
                for event in pygame.event.get():
                    if event.type == pygame.QUIT:
                        run = False

            if(run == False):
                pygame.quit() 
            """      

            if(rec_str == "Your turn\x00"):
                my_turn = True
            elif(rec_str == "You win\x00"):
                pygame_board.show_text("You win!")
                sleep(5)
            elif(rec_str == "You lose\x00"):
                pygame_board.show_text("You lose")
                sleep(5)
            else:
                print("Otrzymano wiadomosc: ")
                board_2d = convert_board(rec_str)
                pygame_board.draw(board_2d, my_turn, player_number)

            if(my_turn == True):
                pygame_board.show_text("Your turn")
                sleep(1)
                pygame_board.draw(board_2d, my_turn, player_number)
                #wyslanie ruchu
                start_field, end_field = pygame_board.get_moves()
                move = move_to_string(start_field, end_field)
                print(move)
                network.sendall(move)
                my_turn = False

input_window = InputWindow(callback=run_game)