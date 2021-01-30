import pygame
from pygame import font
from constants import *
from piece import Piece

class Board:
    def __init__(self):
        pygame.init()
        self.window = pygame.display.set_mode((WIDTH+20, HEIGHT+20))
        pygame.display.set_caption("Checkers")
        self.clock = pygame.time.Clock()
        self.board_2d = []
        self.start = None
        self.my_turn = False
        self.player_number = ""

    def tick(self):
        self.clock.tick(FPS)

    def draw_squares(self):
        if self.my_turn:
            if self.player_number == "1":
                self.window.fill(PEACH)
            else:
                self.window.fill(BLUE)
            pygame.draw.rect(self.window, WHITE, (10, 10, WIDTH, HEIGHT))
        else:
            self.window.fill(WHITE)
        for row in range(ROWS):
            for col in range((row+1) % 2 , COLUMNS, 2):
                if((row, col) == self.start):
                    pygame.draw.rect(self.window, LIGHTBLUE, (col * SQUARE_SIZE + 10, row * SQUARE_SIZE + 10, SQUARE_SIZE, SQUARE_SIZE))
                else:    
                    pygame.draw.rect(self.window, BLACK, (col * SQUARE_SIZE + 10, row * SQUARE_SIZE + 10, SQUARE_SIZE, SQUARE_SIZE))
        pygame.display.update()

    def create_board(self, board_2d):
        board = []
        for row in range(ROWS):
            board.append([])
            for col in range(COLUMNS):
                if board_2d[row][col] == 1:
                    board[row].append(Piece(row, col, PEACH, False))                    
                elif board_2d[row][col] == 2:
                    board[row].append(Piece(row, col, BLUE, False))
                elif board_2d[row][col] == 3:
                    board[row].append(Piece(row, col, PEACH, True))                    
                elif board_2d[row][col] == 4:
                    board[row].append(Piece(row, col, BLUE, True))
                else:
                    board[row].append(0)
        return board

    def draw(self, board_2d, my_turn, player_number):
        self.board_2d = board_2d
        self.my_turn = my_turn
        self.player_number = player_number
        board = self.create_board(board_2d)
        self.draw_squares()
        self.start = None
        for row in range(ROWS):
            for col in range(COLUMNS):
                piece = board[row][col]
                if piece != 0:
                    piece.draw(self.window)
        pygame.display.update()

    def _get_row_col_from_mouse(self, pos):
        x, y = pos
        row = int((y - 10) // SQUARE_SIZE)
        col = int((x - 10) // SQUARE_SIZE)
        return row, col

    def _wait_for_press(self):
        while(True):
            for event in pygame.event.get():
                if event.type == pygame.MOUSEBUTTONDOWN:
                    start_pos = pygame.mouse.get_pos()
                    return self._get_row_col_from_mouse(start_pos)

    def get_moves(self):
        start_tuple = self._wait_for_press()
        self.start = start_tuple
        self.draw(self.board_2d, self.my_turn, self.player_number)
        end_tuple = self._wait_for_press()
        return start_tuple, end_tuple

    def show_text(self, caption):
        font = pygame.font.SysFont("ubuntu", 70)
        text = font.render(caption, True, RED)
        self.window.blit(text, (160, 220))
        pygame.display.update()

    def show_long_text(self, caption):
        font = pygame.font.SysFont("ubuntu", 50)
        text = font.render(caption, True, RED)
        self.window.blit(text, (20, 220))
        pygame.display.update()
