import pygame
from constants import *
from piece import Piece

class Board:
    def __init__(self):
        pygame.init()
        self.window = pygame.display.set_mode((WIDTH, HEIGHT))
        pygame.display.set_caption("Checkers")

    def draw_squares(self):
        self.window.fill(BLACK)
        for row in range(ROWS):
            for col in range(row % 2, COLUMNS, 2):
                pygame.draw.rect(self.window, WHITE, (row * SQUARE_SIZE, col * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE))
        pygame.display.update()

    def create_board(self, board_2d):
        board = []
        for row in range(ROWS):
            board.append([])
            for col in range(COLUMNS):
                if board_2d[row][col] == 1:
                    board[row].append(Piece(row, col, PEACH))                    
                elif board_2d[row][col] == 2:
                    board[row].append(Piece(row, col, BLUE))
                else:
                    board[row].append(0)
        return board

    def draw(self, board_2d):
        board = self.create_board(board_2d)
        self.draw_squares()
        for row in range(ROWS):
            for col in range(COLUMNS):
                piece = board[row][col]
                if piece != 0:
                    piece.draw(self.window)
        pygame.display.update()

    def _get_row_col_from_mouse(self, pos):
        x, y = pos
        row = int(y // SQUARE_SIZE)
        col = int(x // SQUARE_SIZE)
        return row, col

    def _wait_for_press(self):
        while(True):
            for event in pygame.event.get():
                if event.type == pygame.MOUSEBUTTONDOWN:
                    start_pos = pygame.mouse.get_pos()
                    return self._get_row_col_from_mouse(start_pos)

    def get_moves(self):
        start_tuple = self._wait_for_press()
        print(start_tuple)
        end_tuple = self._wait_for_press()
        print(end_tuple)
        return start_tuple, end_tuple
