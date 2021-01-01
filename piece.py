import pygame
from constants import SQUARE_SIZE, CROWN

class Piece:
    PADDING = 8
    def __init__(self, row, col, color, king):
        self.row = row
        self.col = col
        self.color = color
        self.king = king
        self.x = 0
        self.y = 0
        self.calc_pos()

    #liczymy x i y na podstawie miejsca na planszy kolumna i rzad
    def calc_pos(self):
        self.x = SQUARE_SIZE * self.col + SQUARE_SIZE // 2
        self.y = SQUARE_SIZE * self.row + SQUARE_SIZE // 2

    def draw(self, window):
        radius = SQUARE_SIZE // 2 - self.PADDING
        pygame.draw.circle(window, self.color, (self.x, self.y), radius)
        if self.king:
            window.blit(CROWN, (self.x - CROWN.get_width() // 2, self.y - CROWN.get_height() // 2))