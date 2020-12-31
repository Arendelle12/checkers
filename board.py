import pygame
from constants import BLACK, WHITE, ROWS, COLUMNS, SQUARE_SIZE, PEACH, BLUE
from piece import Piece

class Board:
    def __init__(self):
        self.board = []
        #self.create_board()

    def draw_squares(self, window):
        window.fill(BLACK)
        for row in range(ROWS):
            for col in range(row % 2, COLUMNS, 2):
                pygame.draw.rect(window, WHITE, (row * SQUARE_SIZE, col * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE))

    def create_board(self, board_2d):
        self.board = []
        for row in range(ROWS):
            self.board.append([])
            for col in range(COLUMNS):
                if board_2d[row][col] == 1:
                    self.board[row].append(Piece(row, col, PEACH))                    
                elif board_2d[row][col] == 2:
                    self.board[row].append(Piece(row, col, BLUE))
                else:
                    self.board[row].append(0)

    def draw(self, window):
        self.draw_squares(window)
        for row in range(ROWS):
            for col in range(COLUMNS):
                piece = self.board[row][col]
                if piece != 0:
                    piece.draw(window)

    def update(self, window):
        self.board.draw(window)
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
        end_tuple = self._wait_for_press()
        return start_tuple, end_tuple
