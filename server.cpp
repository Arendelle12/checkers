#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <cstdlib>

#define BUF_SIZE 1024
#define MAX_NUM_OF_CLIENTS 100



struct client_info
{
    int client_socket_descriptor;
    int id;
    int *position_in_clients_array;
    int *second_player_fd;
    pthread_mutex_t *connection_mutex;
};

void *ThreadBehavior(void *client)
{
    //dzieki temu nie trzeba robic join
    pthread_detach(pthread_self());
    struct client_info *t_client = (struct client_info *)client;

    int my_id = (*t_client).id;
    printf("My id: %d\n", my_id);

    char tab[BUF_SIZE];
    int n = read((*t_client).client_socket_descriptor, tab, sizeof(tab));
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
    }
    tab[n] = 0;
    printf("%s\n", tab);



    //char buf[BUF_SIZE]; 
    //n = sprintf(buf, "%s", "Hello client");
    //printf("%d\n", n);
    while(*(*t_client).second_player_fd == -1){
        sleep(1);
    }
    write(*(*t_client).second_player_fd, tab, n);

    sleep(5);

    pthread_exit(NULL);

}

void handleConnection(int connection_socket_descriptor, int client_id, int *position_in_clients_array, int *second_player_fd, pthread_mutex_t *connection_mutex)
{
    int create_result = 0;
    pthread_t thread1;

    //struct client_info *client = malloc(sizeof(struct client_info));
    struct client_info *client = (client_info *)malloc(sizeof(struct client_info));
    (*client).client_socket_descriptor = connection_socket_descriptor;
    (*client).id = client_id;
    //nie wiem, czy to przypisanie jest poprawne
    (*client).position_in_clients_array = position_in_clients_array;
    (*client).second_player_fd = second_player_fd;
    (*client).connection_mutex = connection_mutex;

    //printf("player2 id??? : %p\n", second_player_fd);
    printf("player2 fd : %d\n", *second_player_fd);

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

    //inicjalizacja gniazda serwera
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(1234);
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);

    //deskryptor gniazda sockfd
    server_socket_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    if(server_socket_descriptor < 0)
    {
        printf("Socket creation failed\n");
        exit(1);
    }

    //setsockopt
    //int nFoo = 1;
    socklen_t len = sizeof(option_value);  
    setsockopt_result = setsockopt(server_socket_descriptor, SOL_SOCKET, SO_REUSEADDR, &option_value, len);
    //setsockopt_result = setsockopt(server_socket_descriptor, SOL_SOCKET, SO_REUSEADDR, (char*)&option_value, sizeof(option_value));
    if(setsockopt_result != 0)
    {
        printf("Setsockopt failed\n");
        exit(1);
    }
    
    
    bind_result = bind(server_socket_descriptor, (const struct sockaddr *)&server_address, sizeof(server_address));
    if(bind_result < 0)
    {
        printf("Socket bind failed\n");
        exit(1);
    }

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
        printf("Deskryptor klienta: %d\n", connection_socket_descriptor);

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
            if(client_id % 2 == 0)
            {
                player_two_id = client_id + 1;
            }
            else
            {
                player_two_id = client_id - 1;
            }
            printf("pl2 id po wyliczeniu : %d\n", player_two_id);
        }
        else
        {
            printf("Nie mozna polaczyc klienta\n");
            close(connection_socket_descriptor);
        }

        pthread_mutex_unlock(&connection_mutex);

        //chcialabym miec dostep do wartosci, ktora znajduje sie na pozycji player_two_id w tablicy clients
        //ale nie mam kompletnie pojecia, czy to tak dziala XD

        
        handleConnection(connection_socket_descriptor, client_id, &clients[client_id], &clients[player_two_id], &connection_mutex);
        
            




    }
    close(server_socket_descriptor);

    return 0;
}