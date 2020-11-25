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
};

void *ThreadBehavior(void *client)
{
    //dzieki temu nie trzeba robic join
    pthread_detach(pthread_self());
    struct client_info *t_client = (struct client_info *)client;

    char tab[BUF_SIZE];
    int n = read((*t_client).client_socket_descriptor, tab, sizeof(tab));
    if (n == -1){
        printf("Read error occures\n");
        close((*t_client).client_socket_descriptor);
        free(t_client);
    }
    if (n == 0){
        printf("Client disconnected\n");
        close((*t_client).client_socket_descriptor);
        free(t_client);
    }
    tab[n] = 0;
    printf("%s\n", tab);

    char buf[BUF_SIZE]; 
    n = sprintf(buf, "%s", "Hello client");
    printf("%d\n", n);
    write((*t_client).client_socket_descriptor, buf, n);

    sleep(5);

    pthread_exit(NULL);

}

void handleConnection(int connection_socket_descriptor, int client_id)
{
    int create_result = 0;
    pthread_t thread1;

    //struct client_info *client = malloc(sizeof(struct client_info));
    struct client_info *client = (client_info *)malloc(sizeof(struct client_info));
    (*client).client_socket_descriptor = connection_socket_descriptor;
    (*client).id = client_id;

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
    pthread_mutex_t connection_mutex = PTHREAD_MUTEX_INITIALIZER;
    int connected_clients = 0;
    int client_id;

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
                    printf("Liczba polaczonych klientow: %d, id klienta: %d\n", connected_clients, client_id);
                    break;
                }
            }
        }
        else
        {
            printf("Nie mozna polaczyc klienta\n");
            close(connection_socket_descriptor);
        }

        pthread_mutex_unlock(&connection_mutex);

        handleConnection(connection_socket_descriptor, client_id);




    }
    close(server_socket_descriptor);

    return 0;
}