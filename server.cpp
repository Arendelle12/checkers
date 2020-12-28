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
#define MAX_NUM_OF_CLIENTS 100
#define ROWS 8
#define COLUMNS 8
//ROZMIAR TABLICY DO PLANSZY
#define SIZE 64
#define MAX_NUM_OF_GAMES 50

//TODO
/*
- czekanie na 2 gracza
- adresy nie wbite na stale
- wykrywanie konca gry
- damki
- fragmentacja
- uporzadkowanie kodu : po angielsku, osobny plik na gre (?)
*/

struct game
{
    char *board;
    int turn;
    //int player1;
    //int player2;
    int move[4];
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
    //TEMPORARY
    //DO STRUKTURY GAME (?)
    //char *board;
    //int turn;
    //char *move; //?
    game *checkers;
};

//LOGIKA GRY - DO PRZENIESIENIA DO INNEGO PLIKU
//PSEUDOKOD
int *mozliweRuchy(char plansza[], int pozycjaStartowa, int tura)
{
    //ZWRACAMY MOZLIWE KONCOWE POZYCJE W TABLICY JAKO INT
    //0 - przesuniecie w lewo
    //1 - przesuniecie w prawo
    //2 - bicie w lewo
    //3 - bicie w prawo
    //4 - bylo bicie
    int* buf = new int [5];
    for(int i = 0; i < 5; i++)
    {
        buf[i] = -1;
    }

    if(tura == 0)
    {            
        //PRZESUNIECIE PIONKA BEZ BICIA
        if((pozycjaStartowa % 16 != 8) && (plansza[pozycjaStartowa + 7] == '0'))
        {
            //RUCH POPRAWNY w lewo
            buf[0] = pozycjaStartowa + 7;
        }
        if((pozycjaStartowa % 16 != 7) && (plansza[pozycjaStartowa + 9] == '0'))
        {
            //RUCH POPRAWNY w prawo
            buf[1] = pozycjaStartowa + 9;
        }
        //BICIE
        if((pozycjaStartowa % 16 != 1) && (pozycjaStartowa % 16 != 8))
        {
            if((plansza[pozycjaStartowa + 7] == '2') && (plansza[pozycjaStartowa + 14] == '0'))
            {
                //JEST BICIE w lewo
                buf[2] = pozycjaStartowa + 14;
                buf[4] = 1;
            }
        }
        if((pozycjaStartowa % 16 != 7) && (pozycjaStartowa % 16 != 14))
        {
            if((plansza[pozycjaStartowa + 9] == '2') && (plansza[pozycjaStartowa + 18] == '0'))
            {
                //JEST BICIE w prawo
                buf[3] = pozycjaStartowa + 18;
                buf[4] = 1;
            }
        }    
    }
    else if(tura == 1)
    {
        //PRZESUNIECIE PIONKA BEZ BICIA
        if((pozycjaStartowa % 16 != 8) && (plansza[pozycjaStartowa - 9] == '0'))
        {
            //RUCH POPRAWNY w lewo
            buf[0] = pozycjaStartowa - 9;
        }
        if((pozycjaStartowa % 16 != 7) && (plansza[pozycjaStartowa - 7] == '0'))
        {
            //RUCH POPRAWNY w prawo
            buf[1] = pozycjaStartowa - 7;
        }
        //BICIE
        if((pozycjaStartowa % 16 != 1) && (pozycjaStartowa % 16 != 8))
        {
            if((plansza[pozycjaStartowa - 9] == '1') && (plansza[pozycjaStartowa - 18] == '0'))
            {
                //JEST BICIE w lewo
                buf[2] = pozycjaStartowa - 18;
                buf[4] = 1;
            }
        }
        if((pozycjaStartowa % 16 != 7) && (pozycjaStartowa % 16 != 14))
        {
            if((plansza[pozycjaStartowa - 7] == '1') && (plansza[pozycjaStartowa - 14] == '0'))
            {
                //JEST BICIE w prawo
                buf[3] = pozycjaStartowa - 14;
                buf[4] = 1;
            }
        }  
    }
    /*
    printf("Wyznaczone ruchy w funkcji\n");
    for(int i = 0; i < 2; i++)
    {
        printf("%d, ", buf[i]);
    }
    printf("\n\n");
    */
    return buf;
}

//tak zwane bicie xD
//usuniecie pionka z wyznaczonej pozycji
char *jump(char board[], int start_position, int end_position)
{
    int delete_position = start_position + (end_position - start_position) / 2;
    board[delete_position] = '0';
    return board;
}

//wyznaczenie kolejnego bicia
//do zmiany jak dam rade xD
int *nextJump(char plansza[], int pozycjaStartowa, int tura)
{
    //pierwsze dwa pola sie nie zmieniaja - potrzebne do dzialania kodu
    //5 pozycja nas informuje, czy bylo bicie: -1 nie bylo, 1 bylo
    printf("Debug funkcji kolejneBicie\n");
    int* buf = new int [5];
    for(int i = 0; i < 5; i++)
    {
        buf[i] = -1;
    }
    for(int i = 0; i < 5; i++)
    {
        printf("Buf[%d] = %d\n", i, buf[i]);
    }
    printf("Tura w funkcji: %d\n", tura);

    printf("\n plansza w funkcji: \n");
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){ 
            printf("%d ",plansza[i*8+j]-'0');
        }
        printf("\n");
    }

    printf("Pozycja startowa w funkcji: %d\n", pozycjaStartowa);

    if(tura == 0)
    {   
        //BICIE
        if((pozycjaStartowa % 16 != 1) && (pozycjaStartowa % 16 != 8))
        {
            if((plansza[pozycjaStartowa + 7] == '2') && (plansza[pozycjaStartowa + 14] == '0'))
            {
                //JEST BICIE
                buf[2] = pozycjaStartowa + 14;
                buf[4] = 1;
            }
        }
        if((pozycjaStartowa % 16 != 7) && (pozycjaStartowa % 16 != 14))
        {
            if((plansza[pozycjaStartowa + 9] == '2') && (plansza[pozycjaStartowa + 18] == '0'))
            {
                //JEST BICIE
                buf[3] = pozycjaStartowa + 18;
                buf[4] = 1;
            }
        }    
    }
    else if(tura == 1)
    {
        //BICIE
        if((pozycjaStartowa % 16 != 1) && (pozycjaStartowa % 16 != 8))
        {
            if((plansza[pozycjaStartowa - 9] == '1') && (plansza[pozycjaStartowa - 18] == '0'))
            {
                //JEST BICIE
                buf[2] = pozycjaStartowa - 18;
                buf[4] = 1;
            }
        }
        if((pozycjaStartowa % 16 != 7) && (pozycjaStartowa % 16 != 14))
        {
            if((plansza[pozycjaStartowa - 7] == '1') && (plansza[pozycjaStartowa - 14] == '0'))
            {
                //JEST BICIE
                buf[3] = pozycjaStartowa - 14;
                buf[4] = 1;
            }
        }  
    }
        
    printf("Wyznaczone bicia w funkcji\n");
    for(int i = 0; i < 5; i++)
    {
        printf("%d, ", buf[i]);
    }
    printf("\n\n");
    
    return buf;
}

bool validStart(int turn, char startValue)
{
    if((turn == 0) && (startValue == '1')){
        return true;
    }
    else if((turn == 1) && (startValue == '2')){
        return true;
    }
    else{
        return false;
    }
    
}

bool validEnd(int moves[], int endPosition)
{
    for(int i = 0; i < 4; i++)
    {
        if(endPosition == moves[i])
        {
            return true;
            break;
        }
    }
    return false;
}

//ZAMIANA TABLICY CHAR NA TABLICE INT
int *charArrayToInt(char array[])
{
    
    int* buf = new int [ROWS * COLUMNS];
    for(int i = 0; i < ROWS * COLUMNS; i++)
    {
        buf[i] = array[i] - '0';
        //printf("%d\n", buf[i]);
    }
    return buf;
}

//ZAMIANA TABLICY INT NA TABLICE CHAR
char *intArrayToChar(int array[])
{
    char* buf = new char[ROWS * COLUMNS];
    for(int i = 0; i < ROWS * COLUMNS; i++)
    {
        buf[i] = array[i] + '0';
        //printf("%c\n", temp2[i]);
    }
    return buf;
}
/*
//ZAMIANA TABLICY 2D NA 1D
int temp1[n];
for(int i = 0; i < n; i++)
{
    temp1[i] = temp[i/3][i%3];
    //printf("Element na pozycji temp1[%d] wynosi %d\n", i, temp1[i]);
}


*/

/*
//ZAMIANA TABLICY 1D NA 2D
int temp[2][3];
for(int i = 0; i < 2; i++)
{
    for(int j = 0; j < 3; j++)
    {
        temp[i][j] = buf[i*3+j];
        //printf("Element na pozycji temp[%d][%d] wynosi %d\n", i, j, temp[i][j]);
    }
}
*/

int changeTurn(int turn)
{
    if(turn == 0)
    {
        turn = 1;
    }
    else
    {
        turn = 0;    
    }

    return turn;    
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
            if(row < 3)
            {
                if((row%2 == 0 && col%2 == 1) || (row%2 == 1 && col%2 == 0))
                {
                    board[row * ROWS + col] = '1';
                }
                else
                {
                    board[row * ROWS + col] = '0';
                }
                
            }
            else if(row > 4)
            {
                if((row%2 == 1 && col%2 == 0) || (row%2 == 0 && col%2 == 1))
                {
                    board[row * ROWS + col] = '2';
                }
                else
                {
                    board[row * ROWS + col] = '0';
                }
            }
            else
            {
                board[row * ROWS + col] = '0';
            }
            
        }
    }
    /*printf("Inside function\n");
    for(int i = 0; i < ROWS; i ++){
        for(int j = 0; j < COLUMNS; j++)
        {
            printf("%d, ", board[i * ROWS + j]);
        }
        printf("\n");
    }*/
    return board;
}

void *ThreadBehavior(void *client)
{
    //dzieki temu nie trzeba robic join
    pthread_detach(pthread_self());
    struct client_info *t_client = (struct client_info *)client;

    int my_id = (*t_client).id;
    printf("My id: %d\n", my_id);

    char yourTurn[10] = "Twoj ruch";
    
    int player = (*t_client).id %2;

    //TWORZENIE PLANSZY W STRUKTURZE GAME - DLA PIERWSZEGO GRACZA Z PARY
    //ORAZ NADANIE TURY
    //I wyslanie numeru gracza
    if(player == 0)
    {
        (*t_client).checkers->board = createBoard();
        //printf("\nUtworzono plansze\n");
        //printf("\nTURA: %d\n", *((*t_client).checkers->turn));
        
        (*t_client).checkers->turn = 0;
        //printf("\nNadano ture\n");
        write((*t_client).client_socket_descriptor, "1", 1);
    }
    else
    {
        write((*t_client).client_socket_descriptor, "2", 1);
    }

    printf("\nTURA: %d\n", (*t_client).checkers->turn);

    
    
    //ODBIERAMY RUCH A WYSYLAMY PLANSZE
    char tab[BUF_SIZE];

    //odbieramy napis od klienta o parzystym id
    //zamieniamy napis na tablice
    //zamieniamy tablice na napis
    //wysylamy napis do klienta o nieparzystym id
    int readc = 0;
    int start_position;
    int end_position;
    //char selected_start;
    //char selected_end;
    bool valid_move = false;
    int *ruchy;
    bool isJump = false;
    bool rightStart = false;
    bool rightEnd = false;


    bool print_turn = false;


    //CZEKAMY NA WEJSCIE DRUGIEGO GRACZA
    while(*(*t_client).second_player_fd == -1)
    {
        
    }

    printf("WATEK - CO ZAWIERA TABLICA KLIENTA\n%s\n", (*t_client).checkers->board);
    
    //WYSLANIE PLANSZY DO KLIENTA
    write((*t_client).client_socket_descriptor, (*t_client).checkers->board, SIZE);

   // printf("Przed while true\n");
    while(1)
    {    
        //printf("Wszedlem w while true\n");
        //JESLI GRACZ MA RUCH
        if((*t_client).checkers->turn == player)
        {  
            //printf("Wszedlem w if temp_tura = player\n");
            //DOPOKI RUCH NIE JEST PRAWIDLOWY
            while(valid_move == false)
            {
                //ODCZYTUJEMY RUCH
                readc = read((*t_client).client_socket_descriptor, tab, sizeof(tab)-1);
                /*if(readc <= 0)
                {
                    pthread_mutex_lock((*t_client).connection_mutex);
                    *(*t_client).position_in_clients_array = -1;
                    (*t_client).connected_clients--;
                    *(*t_client).checkers->turn = 0;
                    close((*t_client).client_socket_descriptor);
                    free(t_client);
                    pthread_mutex_unlock((*t_client).connection_mutex);
                    pthread_exit(NULL);
                }*/
                tab[readc] = 0;
                printf("ODEBRANA WIADOMOSC: %s\n", tab);

                //ZAKLADAMY, ZE WYSYLA WIADOMOSC 'ROW1COL1ROW2COL2', TAB[0] = ROW1 I TAB[1] = COL1, TAB[2] = ROW2, TAB[3] = COL2
                start_position = getPosition(tab[0], tab[1]);
                end_position = getPosition(tab[2], tab[3]);
                printf("ID GRACZA: %d;;; start: %d, end: %d\n", (*t_client).id, start_position, end_position);

                //sprawdzenie, czy gracz nacisnal wlasciwy pionek
                rightStart = validStart((*t_client).checkers->turn, (*t_client).checkers->board[start_position]);

                if(rightStart == true){
                    valid_move = true;
                    printf("ID GRACZA: %d;;; poprawny start\n", (*t_client).id);
                }
                else{
                    printf("ID GRACZA: %d;;; START NIEPOPRAWNY\n", (*t_client).id);
                    write((*t_client).client_socket_descriptor, yourTurn, 10);
                    //WRACAMY DO POCZATKU PETLI - ZEBY ODCZYTAC WIADOMOSC
                    continue;
                }
                /*
                if( ((*t_client).checkers->turn == 0) && ((*t_client).checkers->board[start_position] == '1') )
                {
                    valid_move = true;
                    printf("ID GRACZA: %d;;; poprawny start\n", (*t_client).id);
                }
                else if( ((*t_client).checkers->turn == 1) && ((*t_client).checkers->board[start_position] == '2') )
                {
                    valid_move = true;
                    printf("ID GRACZA: %d;;; poprawny start\n", (*t_client).id);
                }
                else
                {
                    printf("ID GRACZA: %d;;; START NIEPOPRAWNY\n", (*t_client).id);
                    write((*t_client).client_socket_descriptor, yourTurn, 10);
                    //WRACAMY DO POCZATKU PETLI - ZEBY ODCZYTAC WIADOMOSC
                    continue;
                }
                */
                
                //WYZNACZANIE MOZLIWYCH RUCHOW DLA WYBRANEJ POZYCJI STARTOWEJ
                //jesli bylo bicie - wyznaczamy kolejne bicie
                //jesli nie - wszystkie mozliwe ruchy
                if(isJump == true){
                    ruchy = nextJump((*t_client).checkers->board, start_position, (*t_client).checkers->turn);
                }
                else{
                    ruchy = mozliweRuchy((*t_client).checkers->board, start_position, (*t_client).checkers->turn);
                }
                //isJump zmieniamy na false
                isJump = false;
                printf("ID GRACZA: %d;;; Wyznaczone mozliwe pola koncowe\n", (*t_client).id);
                for(int i = 0; i < 4; i++)
                {
                    printf("%d, ", ruchy[i]);
                }
                printf("\n");
                printf("ID GRACZA: %d;;; Wybrane pole koncowe: %d\n", (*t_client).id, end_position);

                //PRZYPISUJEMY false, ZEBY TERAZ SPRAWDZIC CZY POZYCJA KONCOWA JEST PRAWIDLOWA
                valid_move = false;

                //SPRAWDZENIE, CZY POZYCJA END_POSITION POKRYWA SIE Z KTORAS Z WYZNACZONYCH
                rightEnd = validEnd(ruchy, end_position);
                if(rightEnd == true){
                    //POZYCJA KONCOWA PRAWIDLOWA
                    valid_move = true;
                    printf("ID GRACZA: %d;;; Prawidlowa pozycja koncowa\n", (*t_client).id);
                    break;
                }
                /*
                for(int i = 0; i < 4; i++)
                {
                    if(end_position == ruchy[i])
                    {
                        //POZYCJA KONCOWA PRAWIDLOWA
                        valid_move = true;
                        printf("ID GRACZA: %d;;; Prawidlowa pozycja koncowa\n", (*t_client).id);
                        break;
                    }
                }*/

                if(valid_move == false)
                {
                    printf("ID GRACZA: %d;;; KONIEC NIEPOPRAWNY\n", (*t_client).id);
                    write((*t_client).client_socket_descriptor, yourTurn, 10);
                    //WRACAMY DO POCZATKU PETLI - ZEBY ODCZYTAC WIADOMOSC
                    continue;
                } 

            }

            //zakladamy, ze ruch jest juz prawidlowy
            //selected_start = (*t_client).checkers->board[start_position];
            //selected_end = (*t_client).checkers->board[end_position];

            //przestawiamy pionek ze start_position na end_position
            (*t_client).checkers->board[end_position] = (*t_client).checkers->board[start_position];
            (*t_client).checkers->board[start_position] = '0';

            //jesli bylo bicie, wykonaj bicie - zmiana pionka na 0
            //wysylamy plansze do obu graczy
            //nastepnie sprawdzamy, czy jest mozliwe kolejne bicie
            //jesli tak, wyslij wiadomosc yourTurn
            if((end_position == ruchy[2]) || (end_position == ruchy[3]))
            {
                printf("Wszedlem w if od bicia\n");
                (*t_client).checkers->board = jump((*t_client).checkers->board, start_position, end_position);
                //jest bicie
                isJump = true;

                write((*t_client).client_socket_descriptor, (*t_client).checkers->board, SIZE);
                write(*(*t_client).second_player_fd, (*t_client).checkers->board, SIZE);

                printf("ID klienta: %d;;; tablica klienta PO BICIU\n", (*t_client).id);
                for(int i = 0; i < 8; i++){
                    for(int j = 0; j < 8; j++){ 
                        printf("%d ",(*t_client).checkers->board[i*8+j]-'0');
                    }
                    printf("\n");
                }
                
                ruchy = nextJump((*t_client).checkers->board, end_position, (*t_client).checkers->turn);

                //printf do usuniecia
                printf("\nID GRACZA: %d;;; Wyznaczone kolejne bicia: \n", (*t_client).id);
                for(int i = 0; i < 5; i++)
                {
                    printf("%d, ", ruchy[i]);
                }
                printf("\n");


                if(ruchy[4] == 1)
                {
                    write((*t_client).client_socket_descriptor, yourTurn, 10);
                }
                else
                {
                    //zmieniamy ture
                    pthread_mutex_lock((*t_client).game_mutex);
                    (*t_client).checkers->turn = changeTurn((*t_client).checkers->turn);
                    pthread_mutex_unlock((*t_client).game_mutex);

                    //wysylamy do przeciwnika wiadomosc TWOJ RUCH
                    write(*(*t_client).second_player_fd, yourTurn, 10);
                }
                
            }
            else
            {
                //wysylamy plansze do obu klientow
                write((*t_client).client_socket_descriptor, (*t_client).checkers->board, SIZE);
                write(*(*t_client).second_player_fd, (*t_client).checkers->board, SIZE);

                //printf do usuniecia
                if((valid_move == true) & (print_turn == false)){
                    printf("\nTURA PRZED ZMIANA W GRACZU %d: %d\n", (*t_client).id, (*t_client).checkers->turn);
                }
                //zmieniamy ture
                pthread_mutex_lock((*t_client).game_mutex);
                (*t_client).checkers->turn = changeTurn((*t_client).checkers->turn);
                pthread_mutex_unlock((*t_client).game_mutex);

                //sprawdzenie, czy tura sie zmienia 
                if((valid_move == true) & (print_turn == false)){
                    printf("\nTURA PO ZMIANIE W GRACZU %d: %d\n", (*t_client).id, (*t_client).checkers->turn);
                }
                print_turn = true;
                //wysylamy do przeciwnika wiadomosc TWOJ RUCH
                write(*(*t_client).second_player_fd, yourTurn, 10);
            }


            

            valid_move = false;

            

        
            //read = read((*t_client).client_socket_descriptor, tab, sizeof(tab)-1);
            /*if (n == -1){
                printf("Read error occures\n");
                close((*t_client).client_socket_descriptor);
                free(t_client);
            }
            if (n == 0){
                printf("Client disconnected\n");
                close((*t_client).client_socket_descriptor);
                free(t_client);
            }*/


        }

        print_turn = false;

    }
    sleep(5);

    pthread_exit(NULL);

}

void handleConnection(struct client_info *client)
{
    int create_result = 0;
    pthread_t thread1;

    //int second_player_id = *(*client).second_player_fd;
    //printf("player2 fd : %d\n", second_player_id);

    create_result = pthread_create(&thread1, NULL, ThreadBehavior, (void *)client);
    if(create_result)
    {
        printf("Thread creation failed, error code: %d\n", create_result);
        exit(-1);
    }

}

int main(){
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
    server_address.sin_port = htons(1234);
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
        //printf("Deskryptor klienta: %d\n", connection_socket_descriptor);

        //Dodajemy klienta do tablicy clients -> wpisujemy connection_socket_descriptor tam, gdzie jest -1 
        pthread_mutex_lock(&connection_mutex);

        if(connected_clients < MAX_NUM_OF_CLIENTS)
        {
            for(int i = 0; i < MAX_NUM_OF_CLIENTS; i++)
            {
                if(clients[i] == -1)
                {
                    printf("Connection socket desc: %d\n", connection_socket_descriptor);
                    printf("1111 Clients pozycja: %d, wartosc: %d\n", i, clients[i]);
                    clients[i] = connection_socket_descriptor;
                    printf("2222 Clients pozycja: %d, wartosc: %d\n", i, clients[i]);
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
        }
        else
        {
            printf("Nie mozna polaczyc klienta\n");
            close(connection_socket_descriptor);
        }

        


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

        pthread_mutex_unlock(&connection_mutex);
       
        //TEMPORARY
        //Przypisanie tablicy do struktury klienta - DO ZMIANY NA STRUKTURE GRY
        //(*client).board = boardc;

        //Wypisywanie tablic - DO USUNIECIA
        /*cout<<"BOARD C MAIN"<<endl;

        for(int i = 0; i < ROWS; i++)
        {
            for(int j = 0; j < COLUMNS; j++)
            {
                cout<<boardc[i * ROWS + j]<<" ";
            }
            cout<<endl;
        }

        cout<<"CLIENT BOARD MAIN"<<endl;

        for(int i = 0; i < ROWS; i++)
        {
            for(int j = 0; j < COLUMNS; j++)
            {
                cout<<(*client).board[i * ROWS + j]<<" ";
            }
            cout<<endl;
        }
        //cout<<(*client).board<<endl;
        printf("%s\n", (*client).board);*/
        
        handleConnection(client);
        
            




    }
    close(server_socket_descriptor);

    //ZWALNIANIE PAMIECI PO TABLICACH - DO ZMIANY
    //free(board);
    //free(boardc);

    return 0;
}

    /*cout<<"BOARD"<<endl;
    char* board = createBoard();
    for(int i = 0; i < ROWS; i++)
    {
        for(int j = 0; j < COLUMNS; j++)
        {
            //printf("%d, ", board[i * ROWS + j]);
            cout<<board[i * ROWS + j]<<" ";
        }
        printf("\n");
    }*/


    //Wypisywanie tablic - DO USUNIECIA
    /*cout<<"BOARD"<<endl;
    int* board = createBoard();
    for(int i = 0; i < ROWS; i++)
    {
        for(int j = 0; j < COLUMNS; j++)
        {
            //printf("%d, ", board[i * ROWS + j]);
            cout<<board[i * ROWS + j]<<" ";
        }
        printf("\n");
    }

    char* boardc = intArrayToChar(board);

    int* board2 = createBoard();
    for(int i = 0; i < ROWS; i++)
    {
        for(int j = 0; j < COLUMNS; j++)
        {
            board2[i * ROWS + j] += 1;
            //printf("%d, ", board2[i * ROWS + j]);
            cout<<board2[i * ROWS + j]<<" ";
        }
        printf("\n");
    }

    cout<<"BOARD C"<<endl;
    for(int i = 0; i < ROWS; i++)
    {
        for(int j = 0; j < COLUMNS; j++)
        {
            //printf("%c, ", boardc[i * ROWS + j]);
            cout<<boardc[i * ROWS + j]<<" ";
        }
        printf("\n");
    }

    char* boardc2 = intArrayToChar(board2);
    for(int i = 0; i < ROWS; i++)
    {
        for(int j = 0; j < COLUMNS; j++)
        {
            //printf("%c, ", boardc2[i * ROWS + j]);
            cout<<boardc2[i * ROWS + j]<<" ";
        }
        printf("\n");
    }*/
    //cout<<"board is of type: "<<typeid(*board).name()<<endl;
    //cout<<"boardc is of type: "<<typeid(*boardc).name()<<endl;
    //cout<<"board2 is of type: "<<typeid(*board2).name()<<endl;
    //cout<<"boardc2 is of type: "<<typeid(*boardc2).name()<<endl;

    
    //free(board2);