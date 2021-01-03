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
        self.number = 0
        self.player_number = None
        self.board = None
        self.my_turn = False
        self.move = None
        self.win = False
        self.lose = False

    def increase(self):
        self.lock.acquire()
        self.number = self.number + 1
        # print(self.number)
        self.lock.release()

    def get_player_number(self):
        self.lock.acquire()
        number = self.player_number
        # print(self.number)
        self.lock.release()
        return number

    def get_board(self):
        self.lock.acquire()
        board = self.board
        # print(self.number)
        self.lock.release()
        return board

    def get_turn(self):
        self.lock.acquire()
        my_turn = self.my_turn
        # print(self.number)
        self.lock.release()
        return my_turn

    def get_move(self):
        self.lock.acquire()
        move = self.move
        # print(self.number)
        self.lock.release()
        return move
    
    def get_win(self):
        self.lock.acquire()
        win = self.win
        # print(self.number)
        self.lock.release()
        return win

    def get_lose(self):
        self.lock.acquire()
        lose = self.lose
        # print(self.number)
        self.lock.release()
        return lose

    def set_player_number(self, player_number):
        self.lock.acquire()
        self.player_number = player_number
        # print(self.number)
        self.lock.release()

    def set_board(self, board):
        self.lock.acquire()
        self.board = board 
        # print(self.number)
        self.lock.release()

    def set_turn(self, turn):
        self.lock.acquire()
        self.my_turn = turn
        # print(self.number)
        self.lock.release()

    def set_move(self, move):
        self.lock.acquire()
        self.move = move 
        # print(self.number)
        self.lock.release()

    def set_win(self, win):
        self.lock.acquire()
        self.win = win 
        # print(self.number)
        self.lock.release()

    def set_lose(self, lose):
        self.lock.acquire()
        self.lose = lose
        # print(self.number)
        self.lock.release()

def board_manager(gameState):
    pygame_board = Board()
    pygame_board.draw_squares()
    player_number = None
    while(player_number is None):
        player_number = gameState.get_player_number()
    while(1):
        pygame_board.tick()
        board = gameState.get_board()
        print("Board: ", board)
        if(board is not None):
            print("Board is not none")
            board_2d = convert_board(board)
            my_turn = gameState.get_turn()
            pygame_board.draw(board_2d, my_turn, player_number)
        if(board is not None and gameState.get_turn()):
            print("Show your turn")
            pygame_board.show_text("Your turn") 
            sleep(1) 
            start_field, end_field = pygame_board.get_moves()
            print(start_field, end_field)
            move = move_to_string(start_field, end_field)
            print(move)
            gameState.set_move(move)
            gameState.set_turn(False)
        if(gameState.get_win()):
            pygame_board.show_text("You win!")
        if(gameState.get_lose()):
            pygame_board.show_text("You lose")


    # pobierz ruch
    # ustaw ruch w gameState
    # wyswietl twoja tura ale tylko jak sie zmienilo na true
    # wyswietl koniec gry
    # quit

def connection(gameState, host, port):
    with Network(host, port) as network:
        player_number = network.readline()
        gameState.set_player_number(player_number)
        print(player_number)
        if(player_number == "1"):
            gameState.set_turn(True)
        while(1):
            rec_str = network.readline()
            if(rec_str == "Your turn\x00"):
                print("Ustawiam ture")
                gameState.set_turn(True)
            elif(rec_str == "You win\x00"):
                gameState.set_win(True)
            elif(rec_str == "You lose\x00"):
                gameState.set_lose(True)
            elif(rec_str != gameState.get_board()):
                print("Otrzymano wiadomosc: ")
                gameState.set_board(rec_str)
            move = gameState.get_move()
            if(gameState.get_move() != move):
                ("Ruch jest inny")
                move = gameState.get_move()
                network.sendall(move)
                print("Ruch wyslany")


    # pobierz move z gameState (jesli sie zmienil to wyslij)
    # pobierz zmiane tury ustaw gameState
    # pobierz koniec gry ustaw gameState

def run_game_with_threads(host, port):
    gameState = GameState()
    game_thread = Thread(target = board_manager, args = (gameState, ))
    game_thread.start()
    connection_thread = Thread(target = connection, args = (gameState, host, port ))
    connection_thread.start()

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