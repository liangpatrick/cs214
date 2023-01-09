#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

int boardX;
int boardY;
int plrX;
int plrY;
int monX;
int monY;
int goalX;
int goalY;

bool pWin = false;
bool mWin = false;

void printGrid()
{

    for (int y = boardY - 1; y >= 0; y--)
    {
        for (int x = 0; x < boardX; x++)
        {
            if (x == plrX && y == plrY)
            {
                printf("P ");
            }
            else if (x == monX && y == monY)
            {
                printf("M ");
            }
            else if (x == goalX && y == goalY)
            {
                printf("G ");
            }
            else
            {
                printf("* ");
            }
        }
        printf("\n");
    }
}

void monstersMove()
{
    int yDiff = plrY - monY;
    int xDiff = plrX - monX;
    char move;

    if (abs(yDiff) >= abs(xDiff))
    {
        if (yDiff < 0)
        {
            move = 'S';
            monY--;
        }
        else
        {
            move = 'N';
            monY++;
        }
    }
    else
    {
        if (xDiff < 0)
        {
            move = 'W';
            monX--;
        }
        else
        {
            move = 'E';
            monX++;
        }
    }

    if (plrX == monX && plrY == monY)
    {
        mWin = true;
    }

    printf("monster moves %c \n", move);
}

// z is x or y
bool valid(int z, int boardZ)
{
    if (z < 0 || z >= boardZ)
    {
        printf("invalid move\n");
        return false;
    }
    return true;
}

int main(int argc, char *argv[])
{

    if (argc != 9)
    {
        printf("Error: Invalid Param(s): %s\n", argv[1]);
        return 1;
    }
    else
    {
        boardX = atoi(argv[1]);
        boardY = atoi(argv[2]);
        plrX = atoi(argv[3]);
        plrY = atoi(argv[4]);
        goalX = atoi(argv[5]);
        goalY = atoi(argv[6]);
        monX = atoi(argv[7]);
        monY = atoi(argv[8]);
    }

    char **grid = (char **)malloc(boardY * sizeof(char *));
    for (unsigned char i = 0; i < boardY; i++)
        grid[i] = malloc(boardX * sizeof(char));

    char buf;
    while (mWin != true && pWin != true)
    {
        printGrid();

        buf = getchar();
        while ((getchar()) != '\n')
            ; //clear buffer
        if (buf == 'N')
        {
            if (!valid(plrY + 1, boardY))
                continue;

            plrY++;
        }
        else if (buf == 'S')
        {
            if (!valid(plrY - 1, boardY))
                continue;

            plrY--;
        }
        else if (buf == 'W')
        {
            if (!valid(plrX - 1, boardX))
                continue;

            plrX--;
        }
        else if (buf == 'E')
        {
            if (!valid(plrX + 1, boardX))
                continue;

            plrX++;
        }
        else
        {
            printf("Error, please try again with, 'N', 'W', 'S', or 'E'. \n");
            continue;
        }

        if (plrX == goalX && plrY == goalY)
        {
            pWin = true;
        }
        if (plrX == monX && plrY == monY)
        {
            mWin = true;
            goto monsterWin;
        }

        if (pWin)
        {
            printf("player wins!\n");
            break;
        }
        monstersMove();

    monsterWin:
        if (mWin)
        {
            printf("monster wins!\n");
            break;
        }
    }
    for (unsigned char i = 0; i < boardY; i++)
        free(grid[i]);

    free(grid);
}