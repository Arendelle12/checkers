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
            
            if(pygame_board.something_pressed() == "-1"):
                network.sendall("EXIT")
                return

            rec_str = network.readline()

            """
            if(run == True):
                for event in pygame.event.get():
                    if event.type == pygame.QUIT:
                        run = False

            if(run == False):
                pygame.quit() 
            """      

            if(rec_str == ""):
                # print("Socket timeout")
                continue
            elif(rec_str == "EXIT\x00"):
                network.sendall("EXIT")
                return
            elif(rec_str == "Your turn\x00"):
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

                start_field = "0"
                while(start_field == "0"):
                    start_field = pygame_board.something_pressed()
                
                if (start_field == "-1"):
                    network.sendall("EXIT")
                    return

                pygame_board.set_selected_field(start_field)

                end_field = "0"
                while(end_field == "0"):
                    end_field = pygame_board.something_pressed()

                if (end_field == "-1"):
                    network.sendall("EXIT")
                    return
            
                print(start_field, end_field)
                move = start_field + end_field
                network.sendall(move)
                my_turn = False

input_window = InputWindow(callback=run_game)