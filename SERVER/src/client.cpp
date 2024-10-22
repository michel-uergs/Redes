#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include "/home/michel/Downloads/Redes-main/SERVER/Game.h"

using namespace std;

#define PORT 6335
#define IP_SERVER "127.0.0.1"

Direction dir = STOP;
char Map[HEIGHT][WIDTH];
Player players_[MAX_PLAYERS];

int kbhit(void);
void Input();
void Draw();

int main() 
{
    Player newplayer;
    int id;
    bool gameOver = false;
    bool Start = false;

    int s0;
    struct sockaddr_in server;
    int rval;

    s0 = socket(AF_INET, SOCK_STREAM, 0);
    if (s0 < 0)
    {
        perror("Erro ao abrir socket");
        exit(1);
    }

    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr(IP_SERVER);

    if (connect(s0, (struct sockaddr *)&server, sizeof(server)) < 0) 
    {
        perror("Erro ao conectar com o servidor");
        exit(1);
    }

    cout << "Digite seu nome: ";
    cin >> newplayer.Nome;

    rval = send(s0, &newplayer, sizeof(newplayer), 0);
    if (rval < 0) 
    {
        perror("Erro ao enviar dados do jogador");
        close(s0);
        exit(1);
    }

    rval = recv(s0, &Start, sizeof(Start), 0);
    rval = recv(s0, &id, sizeof(id), 0);
    if (!Start) 
    {
        cout << "Erro ao iniciar o jogo!" << endl;
        close(s0);
        return 1;
    }

    while (!gameOver) 
    {
        Input();
        rval = send(s0, &dir, sizeof(dir), 0);
        rval = recv(s0, &players_, sizeof(players_), 0);
        rval = recv(s0, &Map, sizeof(Map), 0);
        rval = recv(s0, &gameOver, sizeof(gameOver), 0);

        if (rval <= 0) 
        {
            cerr << "Erro na comunicação com o servidor!" << endl;
            break;
        }

        Draw();
        usleep(100000);  // Limite de FPS
    }

    cout << "Jogo finalizado!" << endl;
    close(s0);
    return 0;
}

void Draw() 
{
    bool print;
    system("clear");
    cout << endl;

    for (int i = 0; i < HEIGHT; i++) 
    {
        for (int j = 0; j < WIDTH; j++) 
        {
            cout << Map[i][j];
        }
        cout << endl;
    }
}


void Input() 
{
    if (kbhit()) 
    {
        switch (getchar()) 
        {
            case 'a':
                dir = LEFT;
                break;
            case 'd':
                dir = RIGHT;
                break;
            case 'w':
                dir = UP;
                break;
            case 's':
                dir = DOWN;
                break;
        }
    }
}

int kbhit(void)
{
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if(ch != EOF) 
    {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}