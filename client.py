from constants import *
from mappers import *
from board import Board
from network import Network
from inputWindow import InputWindow
from time import sleep
import signal



def run_game(host, port):
    my_turn = False

    #game window
    board_2d = []
    with Network(host, port) as network:
        def signal_handler(signum, frame):
            print("Signal", signum)
            pygame.quit()
            if my_turn:
                print("quit")
                network.sendall("quit")
            else:
                print("quit2")
                network.sendall("quit2")
            exit()
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

            if(rec_str == "Your turn"):
                print("Moja tura")
                my_turn = True
            elif(rec_str == "Opponent disconnected"):
                print(rec_str)
                pygame_board.show_long_text("Opponent disconnected")
                print("quit2")
                network.sendall("quit2")
                sleep(5)
                pygame.quit()
                exit()
            elif(rec_str == "You win"):
                pygame_board.show_text("You win!")
                network.sendall("quit2")
                sleep(5)
                pygame.quit()
                exit()
            elif(rec_str == "You lose"):
                pygame_board.show_text("You lose")
                network.sendall("quit2")
                sleep(5)
                pygame.quit()
                exit()
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
            
            signal.signal(signal.SIGINT, signal_handler)

input_window = InputWindow(callback=run_game)