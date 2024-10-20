#include <stdio.h>
#include <iostream>
#include <cstdlib>


#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */


#define WIDTH 40
#define HEIGHT 20
#define MAX_PLAYERS 2

using namespace std;

enum Direction{STOP=0, UP, DOWN, RIGHT, LEFT};

struct Fruit
{
    int x;
    int y;
};

struct Body
{
    int x;
    int y;
};

struct Player
{
    char Nome[50];
    int Id = -1;
    int Score=0;
    bool Alive=false;
    Direction S_Dir = STOP;
    int BodyLength=0;
    Body body[WIDTH*HEIGHT];
    Body head;
};

class Game
{

public: 
    char MAP[HEIGHT][WIDTH];
    Player Players[MAX_PLAYERS];

private:
    int countPlayers=0;
    bool GameOver=false;
    Fruit fruit;

    void setFruit()
    {
        //set Fruit
        int x = rand() % (WIDTH-2);
        int y = rand() % (HEIGHT-2);
        fruit.x = x+1;
        fruit.y = y+1;
    };

    void setMap()
    {
        // set Map
        for (int i=0; i<HEIGHT; i++)
        {
            for (int j = 0; j<WIDTH; j++)
            {
                if( i==0 || j==0 || i==HEIGHT-1 || j==WIDTH-1) MAP[i][j] = '#';
                else MAP[i][j] = ' ';
            }  
        }
    };
    
    void moveTail(int id)
    {
        int prevX = Players[id].body[0].x;
        int prevY = Players[id].body[0].y;
        Players[id].body[0].x = Players[id].head.x;
        Players[id].body[0].y = Players[id].head.y;
        int prev2X;
        int prev2Y;

        for (int i = 1; i < Players[id].BodyLength; i++)
        { 
            prev2X = Players[id].body[i].x;
            prev2Y = Players[id].body[i].y;
            Players[id].body[i].x = prevX;
            Players[id].body[i].y = prevY;
            prevX = prev2X;
            prevY = prev2Y;
        }
        
    };

    Direction randDir()
    {
        int i = (rand() % 3);

        switch (i)
        {
            case 0:
                return UP;
                break;
            case 1:
                return DOWN;
                break;
            case 2:
                return RIGHT;
                break;
            case 3:
                return LEFT;

            default:
                return STOP;
                break;
        }
    };

    void die(int Id)
    {
        Players[Id].Alive = false;
        Players[Id].S_Dir = STOP;
    };

public:

    int getCountPlyers()
    {
        return countPlayers;
    };

    bool getGameover()
    {
        return GameOver;
    };

    int SetupPlayer(Player newplayer)
    {
        newplayer.Id = countPlayers;
        newplayer.Alive = true;
        newplayer.S_Dir = randDir();
        newplayer.head.x = rand() % WIDTH;
        newplayer.head.y = rand() % HEIGHT;
        Players[countPlayers] = newplayer;
        countPlayers++;
        return newplayer.Id;
    };

    void RenderMap()
    {
        //render blanc map
        for (int i = 0; i < HEIGHT; i++)
        {
            for (int j = 0; j < WIDTH; j++)
            {
                if( i==0 || j==0 || i==HEIGHT-1 || j==WIDTH-1) MAP[i][j] = '#';
                else MAP[i][j] = ' '; 
            }
        }
        
        //render Fruit
        MAP[fruit.y][fruit.x] = '*';

        //render Players head
        for (int i=0; i<countPlayers; i++)
        {
            if( Players[i].Alive )
            {
                int x = Players[i].head.x;
                int y = Players[i].head.y;   
                MAP[y][x] = '@';
            }
        }

        //render Players body
        for (int i=0; i<countPlayers; i++)
        {
            if( Players[i].Alive )
            {
                for (int j=0; j<Players[i].BodyLength; j++)
                {
                    int x = Players[i].body[j].x;
                    int y = Players[i].body[j].y;   
                    MAP[y][x] = '0';
                }
            }
        }


    };

    void Setup()
    {
        GameOver = false;

        //set the map
        setMap();

        //set Fruit
        setFruit();
    };

    void Set_Dir(Direction dir, int id)
    {
        if ( Players[id].Alive )
        {
            if (dir == STOP)
            {
                Players[id].S_Dir = Players[id].S_Dir;
            }
            else
            {
                Players[id].S_Dir = dir;
            }
        }
    };

    void Logic(int Id)
    {
        // verify if player is alive
        if( Players[Id].Alive == 0) return;

        // move tail
        moveTail(Id);

        // move players
        switch ( Players[Id].S_Dir )
        {
            case UP:
                Players[Id].head.y--;
                break;

            case DOWN:
                Players[Id].head.y++;
                break;

            case RIGHT:
                Players[Id].head.x++;
                break;

            case LEFT:
                Players[Id].head.x--;
                break;

            default:
                break;
        }
        

        // limit logic   
        if (Players[Id].head.x <= 0       || Players[Id].head.y <= 0  ||
            Players[Id].head.x >= WIDTH-1 || Players[Id].head.y >= HEIGHT-1)
        {
            die(Id);
        }
        
        

        // fruit logic
        if ( Players[Id].head.x == fruit.x && Players[Id].head.y == fruit.y )
        {
            Players[Id].Score++;
            Players[Id].BodyLength++;
            setFruit();
        }
        
        // gameover
        for (int i=0, j=0; i < countPlayers; i++)
        {
            if( Players[i].Alive == false ) j++;
            if( j == countPlayers ) GameOver = true;
        }
        
    };

    void Draw()
    {
        system("clear");

        for (int i = 0; i<HEIGHT; i++)
        {
            for (int j = 0; j<WIDTH; j++)
            {
                cout << MAP[i][j];
            }  
            cout << endl;
        }

/*
        for (int i=0; i<countPlayers; i++)
        {
            cout << "Id: " << Players[i].Id << endl;
            cout << "Nome: " << Players[i].Nome << endl;
            cout << "Score: " << Players[i].Score << endl;
            cout << "Alive: " << Players[i].Alive << endl;
            cout << "Nome: [" << Players[i].head.x << "," << Players[i].head.y << "]" << endl;
            cout << "Body length: " << Players[i].BodyLength << endl;
        }
*/

    };

};


