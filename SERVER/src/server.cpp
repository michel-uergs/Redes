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
#include <pthread.h>
#include "/home/michel/Downloads/Redes-main/SERVER/Game.h"

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
        if (rval <= 0) {
            cerr << "Erro ao receber a direção do cliente!" << endl;
            break;
        }

        pthread_mutex_lock(&lock);
        SnakeGame.Set_Dir(dir, id);
        SnakeGame.Logic(id);
        gameOver = SnakeGame.getGameover();
        memcpy(Map, SnakeGame.MAP, sizeof(Map));
        memcpy(players_, SnakeGame.Players, sizeof(players_));
        pthread_mutex_unlock(&lock);

        // send game state to the client
        rval = send(Socket, &players_, sizeof(players_), 0);
        rval = send(Socket, &Map, sizeof(Map), 0);
        rval = send(Socket, &gameOver, sizeof(gameOver), 0);
        if (rval <= 0) 
        {
            cerr << "Erro ao enviar estado do jogo para o cliente!" << endl;
            break;
        }

        usleep(100000);  // Pausa para limitar a taxa de atualização
    }
    
    cout << "Jogo terminou para o cliente!" << endl;
    close(Socket);
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
    if (rval <= 0) 
    {
        cerr << "Erro ao receber jogador!" << endl;
        close(Socket);
        return NULL;
    }
    cout << "Player: " << newplayer.Nome << endl;

    // Configura o jogador no jogo
    id = SnakeGame.SetupPlayer(newplayer);

    // Sincroniza o início do jogo
    pthread_mutex_lock(&lock);
    players_ready++;
    pthread_mutex_unlock(&lock);

    // Espera todos os jogadores estarem prontos
    while(players_ready < MAX_PLAYERS) 
    {
        cout << "Aguardando jogadores!" << endl;
        sleep(1);
    }

    // Iniciar o jogo
    pthread_mutex_lock(&lock);
    Start = true;
    gameOver = false;
    pthread_mutex_unlock(&lock);

    rval = send(Socket, &Start, sizeof(Start), 0);
    rval = send(Socket, &id, sizeof(id), 0);
    if (rval <= 0) 
    {
        cerr << "Erro ao enviar sinal de início ao cliente!" << endl;
        close(Socket);
        return NULL;
    }

    // Executa o jogo
    jogo(Socket, id);

    return NULL;
}

int main()
{
    SnakeGame.Setup();

    pthread_t threads[MAX_CLIENTS];
    int sockets[MAX_CLIENTS];
    int sock, length;
    struct sockaddr_in server;
    int rval, i;

    char hostname[256];
    char *ip;
    struct hostent *host_entry;

    gethostname(hostname, sizeof(hostname));
    host_entry = gethostbyname(hostname);
    ip = inet_ntoa(*((struct in_addr*) host_entry->h_addr_list[0]));
    cout << "Servidor rodando no IP: " << ip << endl;

    // Criar socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) 
    {
        perror("Erro ao criar socket");
        exit(1);
    }

    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    length = sizeof(server);
    if (bind(sock, (struct sockaddr *)&server, length) < 0) 
    {
        perror("Erro ao fazer bind");
        exit(1);
    }

    listen(sock, 5);
    cout << "Servidor aguardando conexões!" << endl;

    for (i = 0; i < MAX_CLIENTS; i++) 
    {
        sockets[i] = accept(sock, NULL, NULL);
        if (sockets[i] < 0)
        {
            perror("Erro ao aceitar conexão");
        }
        cout << "Cliente conectado no socket [" << i << "]: " << sockets[i] << endl;
        if (pthread_create(&threads[i], NULL, handle_client, (void*)&sockets[i]) < 0)
        {
            perror("Erro ao criar thread");
        }
    }

    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        pthread_join(threads[i], NULL);
    }

    close(sock);
    return 0;
}
