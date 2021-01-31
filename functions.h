#ifndef FUNCTIONS_H
#define FUNCTIONS_H

struct pieceMove
{
    bool isValidMove;
    int deletePiece;
};

// czy start jest poprawny
bool validStart(int, char);
// czy lewa kolumna
bool isEdgeLeftColumn(int);
// czy prawa kolumna
bool isEdgeRightColumn(int);
// czy poprawny ruch
pieceMove isValidPieceMove(char[], int, int, int, int);
// czy jest kolejne bicie
bool isNextJump(char [], int, int);
// sprawdzenie czy pionek jest damka
bool checkIfKing(char [], int);
// czy wszystkie pionki przeciwnika usuniete z planszy
bool allEnemiesRemoved(char [], int);
// zmiana tury
int changeTurn(int);
// pobranie pozycji
int getPosition(char, char);
// utworzenie planszy
char *createBoard();

#endif 