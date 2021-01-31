#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <unistd.h>
#include <pthread.h>
#include <cstdlib>
#include <typeinfo>
#include <signal.h>
#include "functions.h"

using namespace std;

#define BUF_SIZE 1024
#define MAX_NUM_OF_CLIENTS 100
//ROZMIAR TABLICY DO PLANSZY
#define SIZE 64
#define MAX_NUM_OF_GAMES 50

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

bool sendMsgWithNewLine(int file_descriptor, const char text[], int sizeOfArray)
{
    //trzeba wyslac sizeOfArray znakow
    int n = 0;
    int w = 0;
    while(n < sizeOfArray)
    {
        char tab[sizeOfArray - n];
        for(int i = 0; i < sizeOfArray-n; i++)
        {
            tab[i] = text[n+i];
        }
        w = write(file_descriptor, tab, sizeOfArray-n);
        if(w < 0 && errno == EPIPE) 
        {
            printf("Write = -1\n");
            return false;
        }
        n += w;
    }
    w = write(file_descriptor, "\n", 1);
    if(w < 0 && errno == EPIPE)
    {
        printf("Write = -1\n");
        return false;
    }
    n += w;
    return true;
}

string readLine(int file_descriptor)
{
    string s = "";
    int n;
    while (true)
    {
        char tab[2];
        n = read(file_descriptor, tab, sizeof(tab)-1);
        if (n <= 0)
        {
            s = "";
            return s;
        }
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

void disconnect_myself(struct client_info * client)
{
    pthread_mutex_lock((*client).connection_mutex);
    printf("Deskryptor w disconnect : %d\n", (*client).client_socket_descriptor);
    *(*client).position_in_clients_array = -1;
    *(*client).connected_clients = *(*client).connected_clients - 1;
    printf("Klienci :%d\n", *(*client).connected_clients);
    (*client).checkers->turn = changeTurn((*client).checkers->turn);
    pthread_mutex_unlock((*client).connection_mutex);
    close((*client).client_socket_descriptor);
    free(client);
    printf("Pamiec zwolniona\n");
    pthread_exit(NULL);
}

void *ThreadBehavior(void *client)
{
    //dzieki temu nie trzeba robic join
    pthread_detach(pthread_self());
    struct client_info *t_client = (struct client_info *)client;

    //wiadomosci
    const char *yourTurn = "Your turn";
    const char *win = "You win";
    const char *lose = "You lose";
    const char *disconnect = "Opponent disconnected";
    
    int player = (*t_client).id %2;
    bool message_sent = false;

    if(player == 1)
    {
        (*t_client).checkers->board = createBoard(); 
        (*t_client).checkers->turn = 0;


        message_sent = sendMsgWithNewLine(*(*t_client).second_player_fd, (char*)"1", 1);
        if(!message_sent)
        {
            //printf("Wyslanie 1 : %d\n" , message_sent );
            sendMsgWithNewLine((*t_client).client_socket_descriptor, disconnect, strlen(disconnect));
            disconnect_myself(t_client);
        }

        sendMsgWithNewLine((*t_client).client_socket_descriptor, (char*)"2", 1);
        
        sendMsgWithNewLine((*t_client).client_socket_descriptor, (*t_client).checkers->board, SIZE);
        
        message_sent = sendMsgWithNewLine(*(*t_client).second_player_fd, (*t_client).checkers->board, SIZE);
        if(!message_sent)
        {
            //printf( "Wyslanie planszy 2 : %d\n", message_sent );
            sendMsgWithNewLine((*t_client).client_socket_descriptor, disconnect, strlen(disconnect));
            disconnect_myself(t_client);
        }
    }
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
                cout << "Otrzymana wiadomosc : " << readc << "\n";
                if(readc == "")
                {
                    printf("if pusty string\n");
                    sendMsgWithNewLine(*(*t_client).second_player_fd, disconnect, strlen(disconnect));
                    disconnect_myself(t_client);
                }
                if(readc == "quit")
                {
                    printf("If quit\n");
                    sendMsgWithNewLine(*(*t_client).second_player_fd, disconnect, strlen(disconnect));
                    disconnect_myself(t_client);
                }
                if(readc == "quit2")
                {
                    printf("If quit2\n");
                    disconnect_myself(t_client);
                }
                
                for (unsigned int i = 0; i < sizeof(tab); i++)
                {
                    tab[i] = readc[i];
                }

                //KLIENT WYSYLA WIADOMOSC 'ROW1COL1ROW2COL2', TAB[0] = ROW1 I TAB[1] = COL1, TAB[2] = ROW2, TAB[3] = COL2
                start_position = getPosition(tab[0], tab[1]);
                end_position = getPosition(tab[2], tab[3]);

                pieceMove = isValidPieceMove((*t_client).checkers->board, start_position, end_position, previous_jump_end, (*t_client).checkers->turn);

                //SPRAWDZENIE, CZY POPRAWNY RUCH
                if(pieceMove.isValidMove){
                    //POZYCJA KONCOWA PRAWIDLOWA
                    previous_jump_end = -1;
                    break;
                }
                else
                {
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
                (*t_client).checkers->board[pieceMove.deletePiece] = '0';

                sendMsgWithNewLine((*t_client).client_socket_descriptor, (*t_client).checkers->board, SIZE);
                
                message_sent = sendMsgWithNewLine(*(*t_client).second_player_fd, (*t_client).checkers->board, SIZE);
                if(!message_sent)
                {
                    sendMsgWithNewLine((*t_client).client_socket_descriptor, disconnect, strlen(disconnect));
                    disconnect_myself(t_client);
                }

                if(allEnemiesRemoved((*t_client).checkers->board, (*t_client).checkers->turn))
                {
                    sendMsgWithNewLine((*t_client).client_socket_descriptor, win, strlen(win));
                                        
                    message_sent = sendMsgWithNewLine(*(*t_client).second_player_fd, lose, strlen(lose));
                    if(!message_sent)
                    {
                        sendMsgWithNewLine((*t_client).client_socket_descriptor, disconnect, strlen(disconnect));
                        disconnect_myself(t_client);
                    }
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
                    message_sent = sendMsgWithNewLine(*(*t_client).second_player_fd, yourTurn, strlen(yourTurn));
                    if(!message_sent)
                    {
                        sendMsgWithNewLine((*t_client).client_socket_descriptor, disconnect, strlen(disconnect));
                        disconnect_myself(t_client);
                    }
                    
                }
                
            }
            else
            {
                //wysylamy plansze do obu klientow
                sendMsgWithNewLine((*t_client).client_socket_descriptor, (*t_client).checkers->board, SIZE);
                
                message_sent = sendMsgWithNewLine(*(*t_client).second_player_fd, (*t_client).checkers->board, SIZE);
                if(!message_sent)
                {
                    sendMsgWithNewLine((*t_client).client_socket_descriptor, disconnect, strlen(disconnect));
                    disconnect_myself(t_client);
                }
                //zmieniamy ture
                pthread_mutex_lock((*t_client).game_mutex);
                (*t_client).checkers->turn = changeTurn((*t_client).checkers->turn);
                pthread_mutex_unlock((*t_client).game_mutex);

                //wysylamy do przeciwnika wiadomosc TWOJ RUCH
                message_sent = sendMsgWithNewLine(*(*t_client).second_player_fd, yourTurn, strlen(yourTurn));
                if(!message_sent)
                {
                    sendMsgWithNewLine((*t_client).client_socket_descriptor, disconnect, strlen(disconnect));
                    disconnect_myself(t_client);
                }
            }
        }
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

    //ignorowanie sygnalu w programie
    struct sigaction act;
    memset(&act, '\0', sizeof(act));
    act.sa_handler = SIG_IGN;
    if(sigaction(SIGPIPE, &act, NULL))
    {
        perror("sigaction");
        exit(1);
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

            struct client_info *client = (client_info *)malloc(sizeof(struct client_info));
            (*client).client_socket_descriptor = connection_socket_descriptor;
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
    }
    close(server_socket_descriptor);

    return 0;
}