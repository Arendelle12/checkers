from constants import *
from mappers import *
from board import Board
from network import Network
from inputWindow import InputWindow
from time import sleep
from threading import Thread, Lock


class GameState:
    def __init__(self):
        self.lock = Lock()
        self.player_number = None
        self.board = None
        self.my_turn = False
        self.move = None
        self.win = False
        self.lose = False
        self.exit_game = False
        self.exit_from_other = False

    def get_player_number(self):
        self.lock.acquire()
        player_number = self.player_number
        self.lock.release()
        return player_number

    def get_board(self):
        self.lock.acquire()
        board = self.board
        self.lock.release()
        return board

    def get_turn(self):
        self.lock.acquire()
        my_turn = self.my_turn
        self.lock.release()
        return my_turn

    def get_move(self):
        self.lock.acquire()
        move = self.move
        self.lock.release()
        return move
    
    def get_win(self):
        self.lock.acquire()
        win = self.win
        self.lock.release()
        return win

    def get_lose(self):
        self.lock.acquire()
        lose = self.lose
        self.lock.release()
        return lose

    def get_exit_game(self):
        self.lock.acquire()
        exit_game = self.exit_game
        self.lock.release()
        return exit_game

    def get_exit_from_other(self):
        self.lock.acquire()
        exit_from_other = self.exit_from_other
        self.lock.release()
        return exit_from_other

    def set_player_number(self, player_number):
        self.lock.acquire()
        self.player_number = player_number
        self.lock.release()

    def set_board(self, board):
        self.lock.acquire()
        self.board = board 
        self.lock.release()

    def set_turn(self, turn):
        self.lock.acquire()
        self.my_turn = turn
        self.lock.release()

    def set_move(self, move):
        self.lock.acquire()
        self.move = move 
        self.lock.release()

    def set_win(self, win):
        self.lock.acquire()
        self.win = win 
        self.lock.release()

    def set_lose(self, lose):
        self.lock.acquire()
        self.lose = lose
        self.lock.release()

    def set_exit_game(self):
        self.lock.acquire()
        self.exit_game = True
        self.lock.release()

    def set_exit_from_other(self):
        self.lock.acquire()
        self.exit_from_other = True
        self.lock.release()

def board_manager(gameState):
    pygame_board = Board()
    pygame_board.draw_squares()
    player_number = None
    while(player_number is None):
        player_number = gameState.get_player_number()
    previous_board = ""
    while(1):
        if gameState.get_exit_from_other():
            return

        if (pygame_board.something_pressed() == "-1"):
            gameState.set_exit_game()
            return
        pygame_board.tick()
        board = gameState.get_board()
        if(board is not None):
            if(previous_board != board):
                previous_board = board
                board_2d = convert_board(board)
                my_turn = gameState.get_turn()
                pygame_board.draw(board_2d, my_turn, player_number)
        if(board is not None and gameState.get_turn()):
            print("Show your turn")
            pygame_board.show_text("Your turn") 
            sleep(1)
            pygame_board.draw(board_2d, my_turn, player_number)
            start_field = "0"
            while(start_field == "0"):
                if gameState.get_exit_from_other():
                    return
                start_field = pygame_board.something_pressed()
            
            if (start_field == "-1"):
                gameState.set_exit_game()
                return

            pygame_board.set_selected_field(start_field)

            end_field = "0"
            while(end_field == "0"):
                if gameState.get_exit_from_other():
                    return
                end_field = pygame_board.something_pressed()

            if (end_field == "-1"):
                gameState.set_exit_game()
                return
            
            print(start_field, end_field)
            move = start_field + end_field
            print(move)
            gameState.set_turn(False)
            gameState.set_move(move)
        if(gameState.get_win()):
            pygame_board.show_text("You win!")
        if(gameState.get_lose()):
            pygame_board.show_text("You lose")

def connection(gameState, network):
    player_number = network.readline()
    gameState.set_player_number(player_number)
    print(player_number)
    while(1):
        rec_str = network.readline()
        if(rec_str == "Your turn\x00"):
            print("Ustawiam ture")
            gameState.set_turn(True)
            move = None
            while(move is None):
                move = gameState.get_move()
            gameState.set_move(None)
            network.sendall(move)
            print("Ruch wyslany")
        elif(rec_str == "You win\x00"):
            gameState.set_win(True)
        elif(rec_str == "You lose\x00"):
            gameState.set_lose(True)
        elif(rec_str == "EXIT\x00"):
            gameState.set_exit_from_other()
            return
        else:
            print("Otrzymano wiadomosc: ")
            gameState.set_board(rec_str)

def send_exit(gameState, network):
    while(1):
        if gameState.get_exit_from_other():
            return

        is_exit = gameState.get_exit_game()
        if is_exit:
            network.sendall("EXIT")
            return

def run_game_with_threads(host, port):
    gameState = GameState()
    game_thread = Thread(target = board_manager, args = (gameState, ))
    game_thread.start()
    with Network(host, port) as network:
        connection_thread = Thread(target = connection, args = (gameState, network))
        connection_thread.start()

        send_exit_thread = Thread(target = send_exit, args = (gameState, network))
        send_exit_thread.start()

        game_thread.join()
        connection_thread.join()
        send_exit_thread.join()

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

input_window = InputWindow(callback=run_game_with_threads)