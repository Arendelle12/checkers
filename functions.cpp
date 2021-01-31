#include "functions.h"

#define ROWS 8
#define COLUMNS 8
#define SIZE 64

bool validStart(int turn, char startValue)
{
    if((turn == 0) && (startValue == '1' || startValue == '3')){
        return true;
    }
    else if((turn == 1) && (startValue == '2' || startValue == '4')){
        return true;
    }
    else{
        return false;
    }
}

bool isEdgeLeftColumn(int position)
{
    return position % 16 == 8;
}

bool isEdgeRightColumn(int position)
{
    return position % 16 == 7;
}

pieceMove isValidPieceMove(char board[], int start_position, int end_position, int previous_jump_end, int turn)
{
    //Zwracamy czy ruch jest poprawny i pozycje zbitego pionka 
    pieceMove result;
    result.isValidMove = false;
    result.deletePiece = -1;

    if(!validStart(turn, board[start_position]))
    {
        return result;
    }

    if (start_position == end_position)
    {
        return result;
    }

    if (board[end_position] != '0')
    {
        return result;
    }

    if((turn == 0) && (board[start_position] == '1'))
    {
        //PRZESUNIECIE PIONKA BEZ BICIA
        if((previous_jump_end == -1) && !isEdgeLeftColumn(start_position) && (end_position == start_position + 7))
        {
            //RUCH POPRAWNY w lewo
            result.isValidMove = true;
        }
        if((previous_jump_end == -1) && !isEdgeRightColumn(start_position) && (end_position == start_position + 9))
        {
            //RUCH POPRAWNY w prawo
            result.isValidMove = true;
        }

        if ((previous_jump_end == -1) || (previous_jump_end == start_position))
        {
            //BICIE
            if (!isEdgeLeftColumn(start_position + 7))
            {
                if((board[start_position + 7] == '2' || board[start_position + 7] == '4') && (end_position == start_position + 14))
                {
                    //JEST BICIE w lewo
                    result.deletePiece = start_position + 7;
                    result.isValidMove = true;
                }
            }
            if (!isEdgeRightColumn(start_position + 9))
            {
                if((board[start_position + 9] == '2' || board[start_position + 9] == '4') && (end_position == start_position + 18))
                {
                    //JEST BICIE w prawo
                    result.deletePiece = start_position + 9;
                    result.isValidMove = true;
                }
            }
        }
    }
    else if((turn == 0) && (board[start_position] == '3'))
    {
        int jump_value = 0;
        if((end_position - start_position) % 7 == 0)
        {
            jump_value = 7;
        }
        else if((end_position - start_position) % 9 == 0)
        {
            jump_value = 9;
        }
        else
        {
            return result;
        }

        if (start_position > end_position)
        {
            jump_value = -1 * jump_value;
        }
        bool oponent_piece = false;
        int delete_position = -1;
        int position_to_check = start_position + jump_value;
        while (position_to_check != end_position)
        {
            if((board[position_to_check] == '0') && ((isEdgeLeftColumn(position_to_check)) || (isEdgeRightColumn(position_to_check))))
            {
                //printf("Koniec planszy\n");
                return result;
            }
            if((board[position_to_check] == '1') || (board[position_to_check] == '3'))
            {
                //printf("Moj pionek\n");
                return result;
            }
            if((board[position_to_check] == '2') || (board[position_to_check] == '4'))
            {
                if(oponent_piece)
                {
                    return result;
                }
                else if(isEdgeLeftColumn(position_to_check) || isEdgeRightColumn(position_to_check))
                {
                    return result;
                }
                else
                {
                    oponent_piece = true;
                    delete_position = position_to_check;
                }
            }
            position_to_check += jump_value;
        }
        result.deletePiece = delete_position;
        result.isValidMove = true;
    }
    else if((turn == 1) && (board[start_position] == '2'))
    {
        //PRZESUNIECIE PIONKA BEZ BICIA
        if((previous_jump_end == -1) && !isEdgeLeftColumn(start_position) && (end_position == start_position - 9))
        {
            //RUCH POPRAWNY w lewo
            result.isValidMove = true;
        }
        if((previous_jump_end == -1) && !isEdgeRightColumn(start_position) && (end_position == start_position - 7))
        {
            //RUCH POPRAWNY w prawo
            result.isValidMove = true;
        }

        if ((previous_jump_end == -1) || (previous_jump_end == start_position))
        {
            //BICIE
            if (!isEdgeLeftColumn(start_position - 9))
            {
                if((board[start_position - 9] == '1' || board[start_position - 9] == '3') && (end_position == start_position - 18))
                {
                    //JEST BICIE w lewo
                    result.deletePiece = start_position - 9;
                    result.isValidMove = true;
                }
            }
            if (!isEdgeRightColumn(start_position - 7))
            {
                if((board[start_position - 7] == '1' || board[start_position - 7] == '3') && (end_position == start_position - 14))
                {
                    //JEST BICIE w prawo
                    result.deletePiece = start_position - 7;
                    result.isValidMove = true;
                }
            }
        }
    }
    else if((turn == 1) && (board[start_position] == '4'))
    {
        int jump_value = 0;
        if((end_position - start_position) % 7 == 0)
        {
            jump_value = 7;
        }
        else if((end_position - start_position) % 9 == 0)
        {
            jump_value = 9;
        }
        else
        {
            return result;
        }

        if (start_position > end_position)
        {
            jump_value = -1 * jump_value;
        }
        bool oponent_piece = false;
        int delete_position = -1;
        int position_to_check = start_position + jump_value;
        while (position_to_check != end_position)
        {
            if((board[position_to_check] == '0') && ((isEdgeLeftColumn(position_to_check)) || (isEdgeRightColumn(position_to_check))))
            {
                //printf("Koniec planszy\n");
                return result;
            }
            if((board[position_to_check] == '2') || (board[position_to_check] == '4'))
            {
                return result;
            }
            if((board[position_to_check] == '1') || (board[position_to_check] == '3'))
            {
                if(oponent_piece)
                {
                    return result;
                }
                else if(isEdgeLeftColumn(position_to_check) || isEdgeRightColumn(position_to_check))
                {
                    return result;
                }
                else
                {
                    oponent_piece = true;
                    delete_position = position_to_check;
                } 
            }
            position_to_check += jump_value;
        }
        result.deletePiece = delete_position;
        result.isValidMove = true;
    }
    return result;
}

//wyznaczenie czy jest kolejne bicie
bool isNextJump(char board[], int start_position, int turn)
{
    if((turn == 0) && (board[start_position] == '1'))
    {
        if(!isEdgeLeftColumn(start_position) && !isEdgeLeftColumn(start_position + 7))
        {
            if(((board[start_position + 7] == '2') || (board[start_position + 7] == '4')) && (board[start_position + 14] == '0'))
            {
                return true;
            }
        }
        if(!isEdgeRightColumn(start_position) && !isEdgeRightColumn(start_position + 9))
        {
            if(((board[start_position + 9] == '2') || (board[start_position + 9] == '4')) && (board[start_position + 18] == '0'))
            {
                return true;
            }
        }
    }
    else if((turn == 0) && (board[start_position] == '3'))
    {
        int jumps [4] = {7,-7,9,-9};
        for (int i = 0; i < 4; i++)
        {
            int jump_step = jumps[i];
            int position = start_position + jump_step;
            while((position >= 0) && (position <= 63))
            {
                if((board[position]  == '1') || (board[position]  == '3'))
                {
                    break;  
                }
                if((board[position]  == '2') || (board[position]  == '4'))
                {
                    if(isEdgeLeftColumn(position) || isEdgeRightColumn(position))
                    {
                        break;
                    }
                    position += jump_step;
                    if((position >= 0) && (position <= 63))
                    {
                        if(board[position] == '0')
                        {
                            return true;
                        }
                    }
                    break;
                }
                position += jump_step;
            }
        }
    }
    else if((turn == 1) && (board[start_position] == '2'))
    {
        if(!isEdgeLeftColumn(start_position) && !isEdgeLeftColumn(start_position - 9))
        {
            if(((board[start_position - 9] == '1') || (board[start_position - 9] == '3')) && (board[start_position - 18] == '0'))
            {
                return true;
            }
        }
        if(!isEdgeRightColumn(start_position) && !isEdgeRightColumn(start_position - 7))
        {
            if(((board[start_position - 7] == '1') || (board[start_position - 7] == '3')) && (board[start_position - 14] == '0'))
            {
                return true;
            }
        }
    }
    else if((turn == 1) && (board[start_position] == '4'))
    {
        int jumps [4] = {7,-7,9,-9};
        for (int i = 0; i < 4; i++)
        {
            int jump_step = jumps[i];
            int position = start_position + jump_step;
            while((position >= 0) && (position <= 63))
            {
                if((board[position]  == '2') || (board[position]  == '4'))
                {
                    break;  
                }
                if((board[position]  == '1') || (board[position]  == '3'))
                {
                    if(isEdgeLeftColumn(position) || isEdgeRightColumn(position))
                    {
                        break;
                    }
                    position += jump_step;
                    if((position >= 0) && (position <= 63))
                    {
                        if(board[position] == '0')
                        {
                            return true;
                        }
                    }
                    break;
                }
                position += jump_step;
            }
        }
    }
    return false;
}

bool checkIfKing(char board[], int position)
{
    if((board[position] == '1') && (position / 8 == 7))
    {
        return true;
    }
    else if((board[position] == '2') && (position / 8 == 0))
    {
        return true;
    }
    return false;
}

bool allEnemiesRemoved(char board[], int turn)
{
    int pieces = 0;
    for(int i = 0; i < SIZE; i++)
    {
        if (turn == 0)
        {
            if(board[i] == '2' || board[i] == '4')
            {
                pieces++;
            }
        }
        if(turn == 1)
        {
            if(board[i] == '1' || board[i] == '3')
            {
                pieces++;
            }
        }
    }
    return pieces == 0;
}

int changeTurn(int turn)
{
    if(turn == 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int getPosition(char row, char col)
{
    int r = row - '0';
    int c = col - '0';
    int position = r * ROWS + c;
    return position;
}

//TWORZENIE PLANSZY
char *createBoard()
{

    char* board = new char[ROWS * COLUMNS];
    for(int row = 0; row < ROWS; row++)
    {
        for(int col = 0; col < COLUMNS; col++)
        {
            board[row * ROWS + col] = '0';
            if(row < 3)
            {
                if((row%2 == 0 && col%2 == 1) || (row%2 == 1 && col%2 == 0))
                {
                    board[row * ROWS + col] = '1';
                }
            }
            else if(row > 4)
            {
                if((row%2 == 1 && col%2 == 0) || (row%2 == 0 && col%2 == 1))
                {
                    board[row * ROWS + col] = '2';
                }
            }
        }
    }
    return board;
}