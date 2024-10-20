#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <pthread.h>
#include "/home/michel-dick/Arquivos/SnakeGame/SERVER/Game.h"

#define PORT 6335
#define MAX_CLIENTS 2

using namespace std;

int players_ready = 0;
Game SnakeGame;
bool Start = false;
bool gameOver = false;

pthread_mutex_t lock;

void jogo(int Socket, int id)
{
    int rval;
    Direction dir;
    char Map[HEIGHT][WIDTH];
    Player players_[MAX_CLIENTS]; 

    while ( !gameOver )
    {
        SnakeGame.RenderMap();

        // receive a direction
        rval = recv(Socket, &dir, sizeof(dir), 0);
        
        pthread_mutex_lock(&lock);
        SnakeGame.Set_Dir(dir, id);
        SnakeGame.Logic(id);
        gameOver = SnakeGame.getGameover();
        memcpy(Map, SnakeGame.MAP, sizeof(Map));
        memcpy(players_, SnakeGame.Players, sizeof(players_));
        pthread_mutex_unlock(&lock);

        // send mapa
        rval = send(Socket, &players_, sizeof(players_), 0);
        rval = send(Socket, &Map, sizeof(Map), 0);
        rval = send(Socket, &gameOver, sizeof(gameOver), 0);
        usleep(100000*2);
    }
    
    cout << "Jogo terminou!" << endl;
}

void* handle_client(void *arg)
{

    int *s = (int*)arg;
    int Socket = *s;
    int rval;
    Player newplayer;
    int id;

    // Recebe o novo jogador
    rval = recv(Socket, &newplayer, sizeof(newplayer), 0);
    if(rval > 0) 
    {
        cout << "Player: " << newplayer.Nome << endl;
    } else 
    {
        cout << "rval: " << rval;
        close(Socket);
        return NULL;
    }

    // Configura o jogador no jogo
    id = SnakeGame.SetupPlayer(newplayer);

    // Sincroniza o início do jogo: incrementa contador de jogadores prontos
    pthread_mutex_lock(&lock);
    players_ready++;
    pthread_mutex_unlock(&lock);

    // Espera todos os jogadores estarem prontos
    while(players_ready < MAX_PLAYERS) 
    {
        cout << "Aguardando jogadores!!!" << endl;
        sleep(5);  // Aguarda um tempo antes de verificar novamente
    }

    //gcc -Wall client.c -o client -lncurses -lpthread
    // 10.34.57.66

    // Todos os jogadores estão prontos, começa o jogo
    pthread_mutex_lock(&lock);
    Start = true;
    gameOver = false;
    pthread_mutex_unlock(&lock);
    
    rval = send(Socket, &Start, sizeof(Start), 0);
    rval = send(Socket, &id, sizeof(id), 0);
    jogo(Socket, id);
    

    return NULL;
}

int main()
{
    //jogo
    SnakeGame.Setup();

    //Sockets
    pthread_t threads[MAX_CLIENTS];
    int sockets[MAX_CLIENTS];
    int sock, length;                                          
    struct sockaddr_in server;                                  
    int rval, i;                                          
    char c;


    // Obter e exibir o IP do servidor
    char hostname[256];
    char *ip;
    struct hostent *host_entry;
    
    gethostname(hostname, sizeof(hostname));  // Obter o nome do host
    host_entry = gethostbyname(hostname);     // Resolver o nome do host para IP
    ip = inet_ntoa(*((struct in_addr*) host_entry->h_addr_list[0]));  // Converter para string
    
    cout << "Servidor rodando no IP: " << ip << endl;


    /* Create socket. */                  
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) 
    {                                            
        perror("opening stream socket");                 
        exit(1);                                           
    }   
                                            
    bzero(&server, sizeof(server));                       
    server.sin_family = AF_INET; 
    server.sin_addr.s_addr = INADDR_ANY; 
    server.sin_port = htons (PORT);
                    
    length = sizeof (server);

    bind(sock, (struct sockaddr *)&server, length);
    /*Aguarda conexoes do cliente */    
    listen(sock,5);

    cout << "Servidor aguardando!! "<< endl;

    i=0;
    while (i < MAX_CLIENTS) 
    {
            sockets[i] = accept(sock,(struct sockaddr *)0,0);
            cout << "Socket[" << i << "]: " << sockets[i] << endl; 
            if( pthread_create(&threads[i], NULL, handle_client, (void*)&sockets[i]) < 0)
            {
                perror("creating pthread");
            }                 
            i++;
    }

    for (int i=0; i<MAX_CLIENTS; i++)
    {
        pthread_join(threads[i], NULL);
    }
    
    return 0;

}