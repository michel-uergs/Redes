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
#include "/home/michel-dick/Arquivos/SnakeGame/SERVER/Game.h"

using namespace std;

#define PORT 6335
#define IP_SERVER "127.0.0.1"

int valor;
Direction dir=STOP;
char Map[HEIGHT][WIDTH];
Player players_[MAX_PLAYERS];

int kbhit(void);
void Input();
void Draw(int id);

int main() 
{
    //game data
    Player newplayer;
    int id;
    bool gameOver = false;
    bool Start = false;

    //socket data
    int s0, f, length, value=1000;	
    struct sockaddr_in server;
    int rval, i;
    char c ;

    s0=socket(AF_INET, SOCK_STREAM, 0);
    if (s0<0)
    {
    perror("opening stream socket");
    exit(1);
    }

    //conexao com o server
    bzero(&server, sizeof(server)) ;   
    server.sin_family = AF_INET ;
    server.sin_port = htons (PORT) ;
    server.sin_addr.s_addr = inet_addr(IP_SERVER); 

    connect(s0, (struct sockaddr *)&server, sizeof (server)) ;

    cout << "Digite seu nome: ";
    cin >> newplayer.Nome;

    rval = send(s0, &newplayer, sizeof(newplayer), 0);

    rval = recv(s0, &Start, sizeof(Start), 0);
    rval = recv(s0, &id, sizeof(id), 0);

    if( !Start ) 
    {
        cout << "ERRO ao começar o jogo" << endl;
    }
    else
    {
        while ( !gameOver )
        {
            Input();
            rval = send(s0, &dir, sizeof(dir), 0);
            rval = recv(s0, &players_, sizeof(players_), 0);
            rval = recv(s0, &Map, sizeof(Map), 0);
            rval = recv(s0, &gameOver, sizeof(gameOver), 0); 
            Draw(id);
        }
    }
    close (s0);
}

void Draw( int id )
{
    bool print;
    system("clear");
    cout << endl;
     
    for (int i=0; i<HEIGHT; i++)
    {
        for (int j = 0; j<WIDTH; j++)
        {
            switch ( Map[i][j] )
            {
                case '#':
                    cout << RED << Map[i][j] << RESET;
                    break;

                case '*':
                    cout << YELLOW << Map[i][j] << RESET;
                    break;

                case '@':
                    if( players_[id].head.x == j && players_[id].head.y == i ) cout << BLUE << Map[i][j];
                    else cout << Map[i][j];
                    break;
                
                case '0':

                    print = false;
                    for (int i=0; i<players_[id].BodyLength; i++)
                    {
                        if( players_[id].body[i].x == j && players_[id].body[i].y == i ) 
                        {
                            cout << GREEN << Map[i][j] << RESET;
                            print = true;
                        }
                    }
                    if( !print ) cout << RESET << Map[i][j];
                    break;

                default:
                    cout << RESET << Map[i][j];
                    break;
            }
        }  
        cout << endl;
    }

    //cout << "head : [" << players_[id].head.x << " , " << players_[id].head.y << "]" << endl;
}

void Input()
{
    char c;
    if( kbhit() )
    {
        c = getchar();
        c = toupper(c);
        
        Direction prev_dir = dir;

        switch (c)
        {
            case 'W':
                if (dir != DOWN)
                {
                    dir = UP;
                }
                break;

            case 'S':
                if (dir != UP)
                {
                    dir = DOWN;
                }
                break;

            case 'A':
                if (dir != RIGHT)
                {
                    dir = LEFT;
                }
                break;

            case 'D':
                if (dir != LEFT)
                {
                    dir = RIGHT;
                }
                break;

            default:
                break;
        }
    }
}

int kbhit(void) 
{
    struct termios oldt, newt;
    int ch;
    int oldf;

    // Obtenha as configurações atuais do terminal
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;

    // Desabilite a entrada canônica e o modo echo
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    // Altere as configurações do descritor de arquivos para não-bloqueante
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    // Tente ler um caractere do stdin
    ch = getchar();

    // Restaure as configurações antigas do descritor de arquivo e do terminal
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    // Se nenhum caractere foi lido, retorne 0, caso contrário, retorne 1
    if (ch != EOF) {
        ungetc(ch, stdin); // Coloque o caractere de volta no buffer
        return 1; // Tecla foi pressionada
    }

    return 0; // Nenhuma tecla pressionada
}