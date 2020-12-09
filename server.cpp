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



struct client_info
{
    int client_socket_descriptor;
    int id;
    int *position_in_clients_array;
    int *second_player_fd;
    int *connected_clients;
    pthread_mutex_t *connection_mutex;
    //TEMPORARY
    //PLANSZA - DO STRUKTURY GAME
    char *board;
};

struct game
{
    int *board;
    int turn;
    int player1;
    int player2;
    int move[4];
};

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



//TWORZENIE PLANSZY
int *createBoard()
{
    int* board = new int[ROWS * COLUMNS];
    for(int row = 0; row < ROWS; row++)
    {
        for(int col = 0; col < COLUMNS; col++)
        {
            if(row < 3)
            {
                if((row%2 == 0 && col%2 == 1) || (row%2 == 1 && col%2 == 0))
                {
                    board[row * ROWS + col] = 1;
                }
            }
            else if(row > 4)
            {
                if((row%2 == 1 && col%2 == 0) || (row%2 == 0 && col%2 == 1))
                {
                    board[row * ROWS + col] = 2;
                }
            }
            else
            {
                board[row * ROWS + col] = 0;
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

    char tab[BUF_SIZE];

    //odbieramy napis od klienta o parzystym id
    //zamieniamy napis na tablice
    //zamieniamy tablice na napis
    //wysylamy napis do klienta o nieparzystym id
    int n = read((*t_client).client_socket_descriptor, tab, sizeof(tab)-1);
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
    if(n <= 0)
    {
        pthread_mutex_lock((*t_client).connection_mutex);
        *(*t_client).position_in_clients_array = -1;
        (*t_client).connected_clients--;
        close((*t_client).client_socket_descriptor);
        free(t_client);
        pthread_mutex_unlock((*t_client).connection_mutex);
        pthread_exit(NULL);
    }
    tab[n] = 0;
    printf("%s\n", tab);
    /*for (int i = 0; i < n; i++)
    {
        printf("%c\n", tab[i]);
    }*/
    

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
    /*
    //ZAMIANA TABLICY 2D NA 1D
    int temp1[n];
    for(int i = 0; i < n; i++)
    {
        temp1[i] = temp[i/3][i%3];
        //printf("Element na pozycji temp1[%d] wynosi %d\n", i, temp1[i]);
    }


    */

    //char buf[BUF_SIZE]; 
    //n = sprintf(buf, "%s", "Hello client");
    //printf("%d\n", n);

    //TEMPORARY
    //TABLICA JAKO PLANSZA - DODAC DO STRUKTURY GAME
    char* buf;
    buf = (*t_client).board;
    //TEMPORARY
    cout<<"THREAD BOARD"<<endl;
    printf("%s\n", buf);
    
    //printf("%s\n", (*t_client).board);

    while(*(*t_client).second_player_fd == -1){
        sleep(1);
    }
    //ROZMIAR TABLICY BUF - DO ZMIANY?
    int size = ROWS * COLUMNS;
    //write(*(*t_client).second_player_fd, buf, n);
    write(*(*t_client).second_player_fd, buf, size);
    
    //write(*(*t_client).second_player_fd, tab, n);

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

    pthread_mutex_t connection_mutex = PTHREAD_MUTEX_INITIALIZER;

    //inicjalizacja tablicy clients wartoscia -1
    for(int i = 0; i < MAX_NUM_OF_CLIENTS; i++)
    {
        clients[i] = -1;
    }
    //Wypisywanie tablic - DO USUNIECIA
    cout<<"BOARD"<<endl;
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
/*
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
*/
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
/*
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
    cout<<"board is of type: "<<typeid(*board).name()<<endl;
    cout<<"boardc is of type: "<<typeid(*boardc).name()<<endl;
    //cout<<"board2 is of type: "<<typeid(*board2).name()<<endl;
    //cout<<"boardc2 is of type: "<<typeid(*boardc2).name()<<endl;

    
    //free(board2);



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
                    clients[i] = connection_socket_descriptor;
                    client_id = i;
                    connected_clients++;
                    //printf("Liczba polaczonych klientow: %d, id klienta: %d\n", connected_clients, client_id);
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

        pthread_mutex_unlock(&connection_mutex);


        struct client_info *client = (client_info *)malloc(sizeof(struct client_info));
        (*client).client_socket_descriptor = connection_socket_descriptor;
        (*client).id = client_id;        
        (*client).position_in_clients_array = &clients[client_id];
        (*client).second_player_fd = &clients[player_two_id];
        (*client).connected_clients = &connected_clients;
        (*client).connection_mutex = &connection_mutex;
        //TEMPORARY
        //Przypisanie tablicy do struktury klienta - DO ZMIANY NA STRUKTURE GRY
        (*client).board = boardc;

        //Wypisywanie tablic - DO USUNIECIA
        cout<<"BOARD C MAIN"<<endl;

        for(int i = 0; i < ROWS; i++)
        {
            for(int j = 0; j < COLUMNS; j++)
            {
                cout<<boardc[i * ROWS + j]<<" ";
            }
            cout<<endl;
        }

        cout<<"CLIENT BOARD MAIN"<<endl;

        /*for(int i = 0; i < ROWS; i++)
        {
            for(int j = 0; j < COLUMNS; j++)
            {
                cout<<(*client).board[i * ROWS + j]<<" ";
            }
            cout<<endl;
        }*/
        //cout<<(*client).board<<endl;
        printf("%s\n", (*client).board);
        
        handleConnection(client);
        
            




    }
    close(server_socket_descriptor);

    //ZWALNIANIE PAMIECI PO TABLICACH - DO ZMIANY
    free(board);
    free(boardc);

    return 0;
}