// zcc +zx81 -startup=2 -lm -create-app -o ..\robot robot.c

/* robotfindskitten ZX81 Port
   @dadecoza 2021
   dade@dade.co.za
*/

#include <stdio.h>
#include <stdlib.h>
#include <input.h>
#include "messages.h"

#define SCREEN_HEIGHT 23
#define SCREEN_WIDTH 31
#define ROBOT 'R'

#define ROBOT_INDEX 0
#define KITTEN_INDEX 1

#define TOTAL_OBJECTS 22 //Including robot and kitten

const char *CLEARED = "                                                                                                --------------------------------";
const char *TITLE = "robotfindskitten v1600000.3";
const char *INSTRUCTIONS = "       by the illustrious\n  leonard richardson (c) 1997\n\nwritten especially for the\nnerth pork robotfindskitten\ncontest. in this game, you are\nrobot (%c). your job is to find\nkitten. this task is complicated\nby the existance of various\nthings which are not kitten.\nrobot must touch items to\ndetermine if they are kitten or\nnot. the game ends when\nrobotfindskitten. alternatively,\nyou may end the game by hitting\nthe Q key. see the documentation\nfor more information.\n\n     press any key to start.\n";

typedef struct
{
    int x;
    int y;
    int message;
    char character;
} object;

enum directions
{
    up,
    down,
    left,
    right
};

object objects[TOTAL_OBJECTS];
int object_index;
char kitten, k;

void place(int i)
{
    zx_setcursorpos(objects[i].y, objects[i].x);
    printf("%c", objects[i].character);
}

void clear(int i)
{
    zx_setcursorpos(objects[i].y, objects[i].x);
    printk(" ");
}

int collision(int x, int y)
{
    if ((x < 0) || (x > SCREEN_WIDTH) || (y < 4) || (y >= SCREEN_HEIGHT))
        return -2;

    int i;
    for (i = 0; i < object_index; i++)
    {
        if ((x == objects[i].x) && (y == objects[i].y))
            return i;
    }
    return -1;
}

int move(int d)
{
    int x = objects[ROBOT_INDEX].x;
    int y = objects[ROBOT_INDEX].y;

    switch (d)
    {
    case up:
        y--;
        break;
    case down:
        y++;
        break;
    case left:
        x--;
        break;
    case right:
        x++;
        break;
    }

    int c = collision(x, y);
    if (c == -1)
    {
        clear(ROBOT_INDEX);
        objects[ROBOT_INDEX].x = x;
        objects[ROBOT_INDEX].y = y;
        place(ROBOT_INDEX);
    }
    return c;
}

int is_unique_character(char c)
{
    if (c == ROBOT) return 0;
    if (c == 124) return 0;
    int i;
    for (i = 0; i < object_index; i++)
    {
        if (c == objects[i].character)
        {
            return 0;
        }
    }
    return 1;
}

int is_unique_message(int m)
{
    int i;
    for (i = 0; i < object_index; i++)
    {
        if (objects[i].message == m)
            return 0;
    }
    return 1;
}

char get_unique_character()
{
    char c;
    do
    {
        c = (rand() % 93) + 33;
    } while (!is_unique_character(c));
    return c;
}

int get_unique_message()
{
    int m;
    do
    {
        m = rand() % MESSAGES;
    } while (!is_unique_message(m));
    return m;
}

void create_objects()
{
    object_index = 0;
    int i, x, y;
    for (i = 0; i < TOTAL_OBJECTS; i++)
    {
        do
        {
            x = rand() % SCREEN_WIDTH;
            y = (rand() % (SCREEN_HEIGHT - 4)) + 4;
        } while (collision(x, y) != -1);
        objects[object_index].x = x;
        objects[object_index].y = y;
        objects[object_index].message = get_unique_message();
        objects[object_index].character = get_unique_character();
        object_index++;
    }
    objects[ROBOT_INDEX].character = ROBOT;
    kitten = objects[KITTEN_INDEX].character;

    for (i = 0; i < TOTAL_OBJECTS; i++)
    {
        place(i);
    }
}

void display_message(char *m)
{
    zx_setcursorpos(0, 0);
    printk(CLEARED);
    zx_setcursorpos(0, 0);
    printk(m);
}

void delay(int t)
{
    int n, i;
    for (n = 0; n < t; n++)
    {
        int i;
        for (i = 0; i < 300; i = i + 1)
        {
            ;
            ; // Delay
        }
    }
}

int animation()
{
    static char *frames[] = {
        "%c       %c",
        " %c     %c ",
        "   %c  %c ",
        "    %c%c "};
    zx_setcursorpos(0, 0);
    printk(CLEARED);
    int i;
    for (i = 0; i < 4; i++)
    {
        zx_setcursorpos(0, 18);
        printf(frames[i], ROBOT, kitten);
        delay(10);
    }
    zx_setcursorpos(0, 0);
    printf("you found kitten.\nway to go, robot.");
}

char wait_key()
{
    do
    {
        k = in_Inkey();
    } while (k == '\0');
    delay(1);
    return k;
}

void instructions()
{
    zx_cls();
    printf(" ");
    invtxt();
    printf(" ");
    printf(TITLE);
    printf(" ");
    invtxt();
    printf("\n");
    printf(INSTRUCTIONS, ROBOT);
    wait_key();
}

int main()
{
    instructions();
    srand((unsigned)clock());
    while (1)
    {
        object_index = 0;
        zx_cls();
        display_message(TITLE);
        create_objects();
        int oi;
        int running = 1;
        while (!zx_break() && running)
        {
            k = wait_key();
            oi = -1;
            switch (k)
            {
            case 'W':
                oi = move(up);
                break;
            case 'S':
                oi = move(down);
                break;
            case 'A':
                oi = move(left);
                break;
            case 'D':
                oi = move(right);
                break;
            case 'Q':
                running = 0;
                break;
            default:
                display_message("invalid command: use w,a,s,d or q.");
            }

            if (oi >= 0)
            {
                if (oi == KITTEN_INDEX)
                {
                    animation();
                    running = 0;
                }
                else
                {
                    display_message(messages[objects[oi].message]);
                }
            }
        }
        zx_setcursorpos(2, 0);
        printf("play again?                     ");
        k = wait_key();
        if (k == 'N')
        {
            zx_cls();
            return 0;
        }
    }
}