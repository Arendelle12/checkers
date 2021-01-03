import pygame

FPS = 60

WIDTH = HEIGHT = 560
ROWS = COLUMNS = 8
SQUARE_SIZE = WIDTH / COLUMNS

#rgb
WHITE = (255, 255, 255)
RED = (255, 0, 0)
BLACK = (0, 0, 0)
PEACH = (255, 218, 185)
BLUE = (123, 104, 238)
LIGHTBLUE = (0, 153, 255)

CROWN = pygame.transform.scale(pygame.image.load('crown.png'), (40, 30))