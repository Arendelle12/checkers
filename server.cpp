#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <unistd.h>
#include <pthread.h>
#include <cstdlib>
#include <typeinfo>

using namespace std;

#define BUF_SIZE 1024
#define MAX_NUM_OF_CLIENTS 2
#define ROWS 8
#define COLUMNS 8
//ROZMIAR TABLICY DO PLANSZY
#define SIZE 64
#define MAX_NUM_OF_GAMES 1

struct game
{
    char *board;
    int turn;
};

struct client_info
{
    int client_socket_descriptor;
    int id;
    int *position_in_clients_array;
    int *second_player_fd;
    int *connected_clients;
    pthread_mutex_t *connection_mutex;
    pthread_mutex_t *game_mutex;
    game *checkers;
};

struct pieceMove
{
    bool isValidMove;
    int deletePiece;
};

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

//LOGIKA GRY - DO PRZENIESIENIA DO INNEGO PLIKU
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
                printf("Koniec planszy\n");
                return result;
            }
            if((board[position_to_check] == '1') || (board[position_to_check] == '3'))
            {
                printf("Moj pionek\n");
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
                printf("Koniec planszy\n");
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

void sendMsgWithNewLine(int file_descriptor, const char text[], int sizeOfArray)
{
    write(file_descriptor, text, sizeOfArray);
    write(file_descriptor, "\n", 1);
}

string readLine(int file_descriptor)
{
    string s = "";
    int n;
    while (true)
    {
        char tab[2];
        n = read(file_descriptor, tab, sizeof(tab)-1);
        cout << tab;
        cout << "\n";
        tab[n] = 0;
        if (tab[0] == '\n')
        {
            return s;
        }
        else
        {
            s += tab[0];
        }
    }
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

void *ThreadBehavior(void *client)
{
    //dzieki temu nie trzeba robic join
    pthread_detach(pthread_self());
    struct client_info *t_client = (struct client_info *)client;

    int my_id = (*t_client).id;
    printf("My id: %d\n", my_id);

    //wiadomosci
    const char *yourTurn = "Your turn";
    const char *win = "You win";
    const char *lose = "You lose";
    
    int player = (*t_client).id %2;

    //TWORZENIE PLANSZY W STRUKTURZE GAME - DLA PIERWSZEGO GRACZA Z PARY
    //ORAZ NADANIE TURY
    //I wyslanie numeru gracza
    if(player == 0)
    {
        (*t_client).checkers->board = createBoard();
        (*t_client).checkers->turn = 0;
        sendMsgWithNewLine((*t_client).client_socket_descriptor, (char*)"1", 1);
    }
    else
    {
        sendMsgWithNewLine((*t_client).client_socket_descriptor, (char*)"2", 1);
    }

    printf("\nTURA: %d\n", (*t_client).checkers->turn);

    
    
    //ODBIERAMY RUCH A WYSYLAMY PLANSZE
    char tab[BUF_SIZE];

    //odbieramy napis od klienta o parzystym id
    //zamieniamy napis na tablice
    //zamieniamy tablice na napis
    //wysylamy napis do klienta o nieparzystym id
    string readc = "";
    int start_position;
    int end_position;

    int previous_jump_end = -1;

    bool print_turn = false;


    //CZEKAMY NA WEJSCIE DRUGIEGO GRACZA
    while(*(*t_client).second_player_fd == -1)
    {
        
    }

    printf("WATEK - CO ZAWIERA TABLICA KLIENTA\n%s\n", (*t_client).checkers->board);
    
    //WYSLANIE PLANSZY DO KLIENTA
    sendMsgWithNewLine((*t_client).client_socket_descriptor, (*t_client).checkers->board, SIZE);

    while(1)
    {    
        //JESLI GRACZ MA RUCH
        if((*t_client).checkers->turn == player)
        {  
            pieceMove pieceMove;
            //DOPOKI RUCH NIE JEST PRAWIDLOWY
            while(true)
            {
                //ODCZYTUJEMY RUCH
                readc = readLine((*t_client).client_socket_descriptor);
                
                // tab[readc.length()]; 
                for (unsigned int i = 0; i < sizeof(tab); i++)
                {
                    tab[i] = readc[i];
                }
                printf("ODEBRANA WIADOMOSC: %s\n", tab);

                //KLIENT WYSYLA WIADOMOSC 'ROW1COL1ROW2COL2', TAB[0] = ROW1 I TAB[1] = COL1, TAB[2] = ROW2, TAB[3] = COL2
                start_position = getPosition(tab[0], tab[1]);
                end_position = getPosition(tab[2], tab[3]);
                printf("ID GRACZA: %d;;; start: %d, end: %d\n", (*t_client).id, start_position, end_position);
 
                printf("ID GRACZA: %d;;; Wybrane pole koncowe: %d\n", (*t_client).id, end_position);

                pieceMove = isValidPieceMove((*t_client).checkers->board, start_position, end_position, previous_jump_end, (*t_client).checkers->turn);
                printf("PIECE MOVE STRUCTURE: %s, %d\n", pieceMove.isValidMove ? "true" : "false", pieceMove.deletePiece);

                //SPRAWDZENIE, CZY POPRAWNY RUCH
                if(pieceMove.isValidMove){
                    //POZYCJA KONCOWA PRAWIDLOWA
                    previous_jump_end = -1;
                    printf("ID GRACZA: %d;;; Prawidlowa pozycja koncowa\n", (*t_client).id);
                    break;
                }
                else
                {
                    printf("ID GRACZA: %d;;; KONIEC NIEPOPRAWNY\n", (*t_client).id);
                    sendMsgWithNewLine((*t_client).client_socket_descriptor, yourTurn, strlen(yourTurn));
                } 

            }

            //ruch jest prawidlowy

            //przestawiamy pionek ze start_position na end_position
            (*t_client).checkers->board[end_position] = (*t_client).checkers->board[start_position];
            (*t_client).checkers->board[start_position] = '0';

            //jesli pionek stal sie damka - zrob damke
            bool isKing = checkIfKing((*t_client).checkers->board, end_position);
            if(isKing)
            {
                if((*t_client).checkers->turn == 0)
                {
                    (*t_client).checkers->board[end_position] = '3';
                }
                else
                {
                    (*t_client).checkers->board[end_position] = '4';
                }
            }

            //jesli bylo bicie, wykonaj bicie - zmiana pionka na 0
            //wysylamy plansze do obu graczy
            //jesli nie ma pionkow przeciwnika - wygrana
            //nastepnie sprawdzamy, czy jest mozliwe kolejne bicie
            //jesli tak, wyslij wiadomosc yourTurn
            if(pieceMove.deletePiece != -1)
            {
                printf("Wszedlem w if od bicia\n");
                (*t_client).checkers->board[pieceMove.deletePiece] = '0';

                sendMsgWithNewLine((*t_client).client_socket_descriptor, (*t_client).checkers->board, SIZE);
                sendMsgWithNewLine(*(*t_client).second_player_fd, (*t_client).checkers->board, SIZE);


                if(allEnemiesRemoved((*t_client).checkers->board, (*t_client).checkers->turn))
                {
                    sendMsgWithNewLine((*t_client).client_socket_descriptor, win, strlen(win));
                    sendMsgWithNewLine(*(*t_client).second_player_fd, lose, strlen(lose));
                    printf("\n!!!Wygral pierwszy gracz!!!\n");
                    break;
                }

                //jesli nie zostal damka
                if(!isKing && isNextJump((*t_client).checkers->board, end_position, (*t_client).checkers->turn))
                {
                    previous_jump_end = end_position;
                    sendMsgWithNewLine((*t_client).client_socket_descriptor, yourTurn, strlen(yourTurn));
                }
                else
                {
                    //zmieniamy ture
                    pthread_mutex_lock((*t_client).game_mutex);
                    (*t_client).checkers->turn = changeTurn((*t_client).checkers->turn);
                    pthread_mutex_unlock((*t_client).game_mutex);

                    //wysylamy do przeciwnika wiadomosc TWOJ RUCH
                    sendMsgWithNewLine(*(*t_client).second_player_fd, yourTurn, strlen(yourTurn));
                }
                
            }
            else
            {
                //wysylamy plansze do obu klientow
                sendMsgWithNewLine((*t_client).client_socket_descriptor, (*t_client).checkers->board, SIZE);
                sendMsgWithNewLine(*(*t_client).second_player_fd, (*t_client).checkers->board, SIZE);

                //printf do usuniecia
                if(print_turn == false){
                    printf("\nTURA PRZED ZMIANA W GRACZU %d: %d\n", (*t_client).id, (*t_client).checkers->turn);
                }
                //zmieniamy ture
                pthread_mutex_lock((*t_client).game_mutex);
                (*t_client).checkers->turn = changeTurn((*t_client).checkers->turn);
                pthread_mutex_unlock((*t_client).game_mutex);

                //sprawdzenie, czy tura sie zmienia 
                if(print_turn == false){
                    printf("\nTURA PO ZMIANIE W GRACZU %d: %d\n", (*t_client).id, (*t_client).checkers->turn);
                }
                print_turn = true;
                //wysylamy do przeciwnika wiadomosc TWOJ RUCH
                sendMsgWithNewLine(*(*t_client).second_player_fd, yourTurn, strlen(yourTurn));
            }
        }

        print_turn = false;
    }

    pthread_exit(NULL);

}

void handleConnection(struct client_info *client)
{
    int create_result = 0;
    pthread_t thread1;

    create_result = pthread_create(&thread1, NULL, ThreadBehavior, (void *)client);
    if(create_result)
    {
        printf("Thread creation failed, error code: %d\n", create_result);
        exit(-1);
    }
}

int main(int argc, char* argv[]){

    if(argc != 2)
    {
        printf("Sposob uzycia: make run_server port=<numer_portu>\n");
        printf("LUB\n");
        printf("Sposob uzycia: ./server <numer_portu>\n");
        exit(1);
    }
    int server_port = atoi(argv[1]);

    struct sockaddr_in server_address;
    int clients[MAX_NUM_OF_CLIENTS];
    int option_value = 1;
    int setsockopt_result;
    int server_socket_descriptor;
    int bind_result;
    int listen_result;
    int connection_socket_descriptor;
    
    int connected_clients = 0;
    int client_id;
    int player_two_id;
    int game_id;

    pthread_mutex_t connection_mutex = PTHREAD_MUTEX_INITIALIZER;

    //TABLICA STRUKTUR
    game games[MAX_NUM_OF_GAMES];
    //TABLICA MUTEXOW (DO TURN)
    pthread_mutex_t game_mutex[MAX_NUM_OF_GAMES];

    //inicjalizacja tablicy clients wartoscia -1
    for(int i = 0; i < MAX_NUM_OF_CLIENTS; i++)
    {
        clients[i] = -1;
    }

    //inicjalizacja tablicy mutexow
    
    for(int i = 0 ; i < MAX_NUM_OF_GAMES; i++)
    {
        game_mutex[i] = PTHREAD_MUTEX_INITIALIZER;
    }

    //inicjalizacja gniazda serwera
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(server_port);
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);

    //deskryptor gniazda - server_socket_descriptor
    server_socket_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    if(server_socket_descriptor < 0)
    {
        printf("Socket creation failed\n");
        exit(1);
    }

    //setsockopt - opcje socketu
    socklen_t len = sizeof(option_value);  
    setsockopt_result = setsockopt(server_socket_descriptor, SOL_SOCKET, SO_REUSEADDR, &option_value, len);
    if(setsockopt_result != 0)
    {
        printf("Setsockopt failed\n");
        exit(1);
    }

    //bind - dowiazanie nazwy do socketu
    bind_result = bind(server_socket_descriptor, (const struct sockaddr *)&server_address, sizeof(server_address));
    if(bind_result < 0)
    {
        printf("Socket bind failed\n");
        exit(1);
    }

    //listen - nasluchiwanie na polaczenia na sockecie
    listen_result = listen(server_socket_descriptor, MAX_NUM_OF_CLIENTS);
    if(listen_result < 0)
    {
        printf("Listen failed\n");
        exit(1);
    }

    printf("Waiting for clients\n");

    while (1)
    {
        //connection_socket_descriptor - deskryptor polaczonego klienta
        connection_socket_descriptor = accept(server_socket_descriptor, NULL, NULL);
        if(connection_socket_descriptor < 0)
        {
            printf("Server accept failed\n");
            exit(1);
        }

        //Dodajemy klienta do tablicy clients -> wpisujemy connection_socket_descriptor tam, gdzie jest -1 
        pthread_mutex_lock(&connection_mutex);
        printf("Mutex LOCKED\n");

        if(connected_clients < MAX_NUM_OF_CLIENTS)
        {
            for(int i = 0; i < MAX_NUM_OF_CLIENTS; i++)
            {
                if(clients[i] == -1)
                {
                    printf("Connection socket desc: %d\n", connection_socket_descriptor);
                    clients[i] = connection_socket_descriptor;
                    client_id = i;
                    connected_clients++;
                    game_id = client_id/2;
                    printf("Liczba polaczonych klientow: %d, id klienta: %d\n", connected_clients, client_id);
                    break;
                }
            }
            //wyliczanie id drugiego gracza
            if(client_id % 2 == 0)
            {
                player_two_id = client_id + 1;
            }
            else
            {
                player_two_id = client_id - 1;
            }
            //printf("pl2 id po wyliczeniu : %d\n", player_two_id);

            struct client_info *client = (client_info *)malloc(sizeof(struct client_info));
            (*client).client_socket_descriptor = connection_socket_descriptor;
            printf("Id klienta: %d\n", client_id);
            (*client).id = client_id;        
            (*client).position_in_clients_array = &clients[client_id];
            (*client).second_player_fd = &clients[player_two_id];
            (*client).connected_clients = &connected_clients;
            (*client).connection_mutex = &connection_mutex;
            (*client).checkers = &games[game_id];
            (*client).game_mutex = &game_mutex[game_id];

            handleConnection(client);
        }
        else
        {
            printf("Nie mozna polaczyc klienta\n");
            close(connection_socket_descriptor);
        }

        

        pthread_mutex_unlock(&connection_mutex);
        printf("Mutex UNLOCKED\n");

       
        
        
    }
    close(server_socket_descriptor);

    return 0;
}