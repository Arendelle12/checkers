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
        for row in range(ROWS):
            self.board.append([])
            for col in range(COLUMNS):
                if board_2d[row][col] == 1:
                    self.board[row].append(Piece(row, col, PEACH))                    
                elif board_2d[row][col] == 2:
                    self.board[row].append(Piece(row, col, BLUE))
                else:
                    self.board[row].append(0)

    def draw(self, window, board_2d):
        self.draw_squares(window)
        for row in range(ROWS):
            for col in range(COLUMNS):
                piece = self.board[row][col]
                if piece != 0:
                    piece.draw(window)

                """
                if board_2d[row][col] == 1:
                    piece.draw(window)
                elif board_2d[row][col] == 2:
                    piece.draw(window)
                """
                