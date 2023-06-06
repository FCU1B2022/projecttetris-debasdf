#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <windows.h>

#define LEFT_KEY 0x25     // The key to move left, default = 0x25 (left arrow)
#define RIGHT_KEY 0x27    // The key to move right, default = 0x27 (right arrow)
#define ROTATE_KEY 0x26   // The key to rotate, default = 0x26 (up arrow)
#define DOWN_KEY 0x28     // The key to move down, default = 0x28 (down arrow)
#define FALL_KEY 0x20     // The key to fall, default = 0x20 (spacebar)

#define FALL_DELAY 500    // The delay between each fall, default = 500
#define RENDER_DELAY 100  // The delay between each frame, default = 100

#define LEFT_FUNC() GetAsyncKeyState(LEFT_KEY) & 0x8000
#define RIGHT_FUNC() GetAsyncKeyState(RIGHT_KEY) & 0x8000
#define ROTATE_FUNC() GetAsyncKeyState(ROTATE_KEY) & 0x8000
#define DOWN_FUNC() GetAsyncKeyState(DOWN_KEY) & 0x8000
#define FALL_FUNC() GetAsyncKeyState(FALL_KEY) & 0x8000

#define CANVAS_WIDTH 10
#define CANVAS_HEIGHT 20

typedef enum {
    RED = 41,
    GREEN,
    YELLOW,
    BLUE,
    PURPLE,
    CYAN,
    WHITE,
    BLACK = 0,
}Color;

typedef enum {
    EMPTY = -1,
    I,
    J,
    L,
    O,
    S,
    T,
    Z
}ShapeId;

typedef struct {
    ShapeId shape;
    Color color;
    int size;
    char rotates[4][4][4];
}Shape;
//int cd_time;
typedef struct
{
    int x;
    int y;
    int score;
    int rotate;
    int fallTime;
    int cd_time;//冷卻時間
    int ya_time;//YA動畫時間
    ShapeId queue[4];
}State;

typedef struct {
    Color color;
    ShapeId shape;
    bool current;
}Block;

Shape shapes[7] = {
    {
        .shape = I,
        .color = CYAN,
        .size = 4,
        .rotates =
        {
            {
                {0, 0, 0, 0},
                {1, 1, 1, 1},
                {0, 0, 0, 0},
                {0, 0, 0, 0}
            },
            {
                {0, 0, 1, 0},
                {0, 0, 1, 0},
                {0, 0, 1, 0},
                {0, 0, 1, 0}
            },
            {
                {0, 0, 0, 0},
                {0, 0, 0, 0},
                {1, 1, 1, 1},
                {0, 0, 0, 0}
            },
            {
                {0, 1, 0, 0},
                {0, 1, 0, 0},
                {0, 1, 0, 0},
                {0, 1, 0, 0}
            }
        }
    },
    {
        .shape = J,
        .color = BLUE,
        .size = 3,
        .rotates =
        {
            {
                {1, 0, 0},
                {1, 1, 1},
                {0, 0, 0}
            },
            {
                {0, 1, 1},
                {0, 1, 0},
                {0, 1, 0}
            },
            {
                {0, 0, 0},
                {1, 1, 1},
                {0, 0, 1}
            },
            {
                {0, 1, 0},
                {0, 1, 0},
                {1, 1, 0}
            }
        }
    },
    {
        .shape = L,
        .color = YELLOW,
        .size = 3,
        .rotates =
        {
            {
                {0, 0, 1},
                {1, 1, 1},
                {0, 0, 0}
            },
            {
                {0, 1, 0},
                {0, 1, 0},
                {0, 1, 1}
            },
            {
                {0, 0, 0},
                {1, 1, 1},
                {1, 0, 0}
            },
            {
                {1, 1, 0},
                {0, 1, 0},
                {0, 1, 0}
            }
        }
    },
    {
        .shape = O,
        .color = WHITE,
        .size = 2,
        .rotates =
        {
            {
                {1, 1},
                {1, 1}
            },
            {
                {1, 1},
                {1, 1}
            },
            {
                {1, 1},
                {1, 1}
            },
            {
                {1, 1},
                {1, 1}
            }
        }
    },
    {
        .shape = S,
        .color = GREEN,
        .size = 3,
        .rotates =
        {
            {
                {0, 1, 1},
                {1, 1, 0},
                {0, 0, 0}
            },
            {
                {0, 1, 0},
                {0, 1, 1},
                {0, 0, 1}
            },
            {
                {0, 0, 0},
                {0, 1, 1},
                {1, 1, 0}
            },
            {
                {1, 0, 0},
                {1, 1, 0},
                {0, 1, 0}
            }
        }
    },
    {
        .shape = T,
        .color = PURPLE,
        .size = 3,
        .rotates =
        {
            {
                {0, 1, 0},
                {1, 1, 1},
                {0, 0, 0}
            },

                {{0, 1, 0},
                {0, 1, 1},
                {0, 1, 0}
            },
            {
                {0, 0, 0},
                {1, 1, 1},
                {0, 1, 0}
            },
            {
                {0, 1, 0},
                {1, 1, 0},
                {0, 1, 0}
            }
        }
    },
    {
        .shape = Z,
        .color = RED,
        .size = 3,
        .rotates =
        {
            {
                {1, 1, 0},
                {0, 1, 1},
                {0, 0, 0}
            },
            {
                {0, 0, 1},
                {0, 1, 1},
                {0, 1, 0}
            },
            {
                {0, 0, 0},
                {1, 1, 0},
                {0, 1, 1}
            },
            {
                {0, 1, 0},
                {1, 1, 0},
                {1, 0, 0}
            }
        }
    },
};

void setBlock(Block* block, Color color, ShapeId shape, bool current)
{
    block->color = color;
    block->shape = shape;
    block->current = current;
}

void resetBlock(Block* block)
{
    block->color = BLACK;
    block->shape = EMPTY;
    block->current = false;
}

bool move(Block canvas[CANVAS_HEIGHT][CANVAS_WIDTH], int originalX, int originalY, int originalRotate, int newX, int newY, int newRotate, ShapeId shapeId) {
    Shape shapeData = shapes[shapeId];
    int size = shapeData.size;

    // check if the new position is valid to place the block
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (shapeData.rotates[newRotate][i][j]) {
                if (newX + j < 0 || newX + j >= CANVAS_WIDTH || newY + i < 0 || newY + i >= CANVAS_HEIGHT) {
                    return false;
                }
                if (!canvas[newY + i][newX + j].current && canvas[newY + i][newX + j].shape != EMPTY) {
                    return false;
                }
            }
        }
    }

    // remove the old position
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (shapeData.rotates[originalRotate][i][j]) {
                resetBlock(&canvas[originalY + i][originalX + j]);
            }
        }
    }

    // move the block
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (shapeData.rotates[newRotate][i][j]) {
                setBlock(&canvas[newY + i][newX + j], shapeData.color, shapeId, true);
            }
        }
    }

    return true;
}

void printCanvas(Block canvas[CANVAS_HEIGHT][CANVAS_WIDTH], State* state)
{
    printf("\033[0;0H\n");
    for (int i = 0; i < CANVAS_HEIGHT; i++) {
        printf("|");
        for (int j = 0; j < CANVAS_WIDTH; j++) {
            printf("\033[%dm\u3000", canvas[i][j].color);
            //printf("\033[%dm\u3000", canvas[i][j].color);
        }
        printf("\033[0m|\n");
    }

    Shape shapeData = shapes[state->queue[1]];
    printf("\033[%d;%dHNext:", 3, CANVAS_WIDTH * 2 + 5);
    for (int i = 1; i <= 3; i++)
    {
        shapeData = shapes[state->queue[i]];
        for (int j = 0; j < 4; j++) {
            printf("\033[%d;%dH", i * 4 + j, CANVAS_WIDTH * 2 + 15);
            for (int k = 0; k < 4; k++) {
                if (j < shapeData.size && k < shapeData.size && shapeData.rotates[0][j][k]) {
                    printf("\x1b[%dm  ", shapeData.color);
                }
                else {
                    printf("\x1b[0m  ");
                }
            }
        }
    }
    return;
}

int clearLine(Block canvas[CANVAS_HEIGHT][CANVAS_WIDTH]) {
    for (int i = 0; i < CANVAS_HEIGHT; i++) {
        for (int j = 0; j < CANVAS_WIDTH; j++) {
            if (canvas[i][j].current) {
                canvas[i][j].current = false;
            }
        }
    }

    int linesCleared = 0;

    for (int i = CANVAS_HEIGHT - 1; i >= 0; i--)
    {
        bool isFull = true;
        for (int j = 0; j < CANVAS_WIDTH; j++)
        {
            if (canvas[i][j].shape == EMPTY) {
                isFull = false;
                break;
            }
        }

        if (isFull) {
            linesCleared += 1;

            for (int j = i; j > 0; j--)
            {
                for (int k = 0; k < CANVAS_WIDTH; k++)
                {
                    setBlock(&canvas[j][k], canvas[j - 1][k].color, canvas[j - 1][k].shape, false);
                    resetBlock(&canvas[j - 1][k]);
                }
            }
            i++;
        }
    }


    return linesCleared;
}

void logic(Block canvas[CANVAS_HEIGHT][CANVAS_WIDTH], State* state)
{
    if (ROTATE_FUNC()) {
        int newRotate = (state->rotate + 1) % 4;
        if (move(canvas, state->x, state->y, state->rotate, state->x, state->y, newRotate, state->queue[0]))
        {
            state->rotate = newRotate;
        }
    }
    else if (LEFT_FUNC()) {
        if (move(canvas, state->x, state->y, state->rotate, state->x - 1, state->y, state->rotate, state->queue[0]))
        {
            state->x -= 1;
        }
    }
    else if (RIGHT_FUNC()) {
        if (move(canvas, state->x, state->y, state->rotate, state->x + 1, state->y, state->rotate, state->queue[0]))
        {
            state->x += 1;
        }
    }
    else if (DOWN_FUNC()) {
        state->fallTime = FALL_DELAY;
    }
    else if (FALL_FUNC()) {
        state->fallTime += FALL_DELAY * CANVAS_HEIGHT;
    }

    state->fallTime += RENDER_DELAY;

    while (state->fallTime >= FALL_DELAY) 
    {
          state->fallTime -= FALL_DELAY;
          printf("\033[%d;%dH\x1b[1;36mSCORE : %d \x1b[0m\033[%d;%dH", CANVAS_HEIGHT - 3, CANVAS_WIDTH * 2 + 5, state->score, CANVAS_HEIGHT + 5, 0);
          printf("\033[%d;%dH\x1b[1;36mcd_time : %d \x1b[0m\033[%d;%dH", CANVAS_HEIGHT -2, CANVAS_WIDTH * 2 + 5, state->cd_time, CANVAS_HEIGHT + 5, 0);
          if (state->ya_time > 0)
          {
              printf("\033[%d;%dH\x1b[33m       _    (^)\x1b[0m\033[%d;%dH", CANVAS_HEIGHT - 1, CANVAS_WIDTH * 2 + 5, CANVAS_HEIGHT + 5, 0);
              printf("\033[%d;%dH\x1b[33m      (_\\   |_|\x1b[0m\033[%d;%dH", CANVAS_HEIGHT , CANVAS_WIDTH * 2 + 5, CANVAS_HEIGHT + 5, 0);
              printf("\033[%d;%dH\x1b[33m       \\_\\  |_|\x1b[0m\033[%d;%dH", CANVAS_HEIGHT+1, CANVAS_WIDTH * 2 + 5, CANVAS_HEIGHT + 5, 0);
              printf("\033[%d;%dH\x1b[33m       _\\_\\,/_|\x1b[0m\033[%d;%dH", CANVAS_HEIGHT+2, CANVAS_WIDTH * 2 + 5, CANVAS_HEIGHT + 5, 0);
              printf("\033[%d;%dH\x1b[33m      (`\\(_|`\\|\x1b[0m\033[%d;%dH", CANVAS_HEIGHT+3, CANVAS_WIDTH * 2 + 5, CANVAS_HEIGHT + 5, 0);
              printf("\033[%d;%dH\x1b[33m     (`\\,)  \\ \\\x1b[0m\033[%d;%dH", CANVAS_HEIGHT+4, CANVAS_WIDTH * 2 + 5, CANVAS_HEIGHT + 5, 0);
              printf("\033[%d;%dH\x1b[33m      \\,)   | |\x1b[0m\033[%d;%dH", CANVAS_HEIGHT+5, CANVAS_WIDTH * 2 + 5, CANVAS_HEIGHT + 5, 0);
              printf("\033[%d;%dH\x1b[33m        \\__(__|\x1b[0m\033[%d;%dH", CANVAS_HEIGHT+6, CANVAS_WIDTH * 2 + 5, CANVAS_HEIGHT + 5, 0);
          }
          else 
          {
              printf("\033[%d;%dH\x1b[30m         _    (^)\x1b[0m\033[%d;%dH", CANVAS_HEIGHT - 1, CANVAS_WIDTH * 2 + 5, CANVAS_HEIGHT + 5, 0);
              printf("\033[%d;%dH\x1b[30m      (_\\   |_|\x1b[0m\033[%d;%dH", CANVAS_HEIGHT, CANVAS_WIDTH * 2 + 5, CANVAS_HEIGHT + 5, 0);
              printf("\033[%d;%dH\x1b[30m       \\_\\  |_|\x1b[0m\033[%d;%dH", CANVAS_HEIGHT + 1, CANVAS_WIDTH * 2 + 5, CANVAS_HEIGHT + 5, 0);
              printf("\033[%d;%dH\x1b[30m       _\\_\\,/_|\x1b[0m\033[%d;%dH", CANVAS_HEIGHT + 2, CANVAS_WIDTH * 2 + 5, CANVAS_HEIGHT + 5, 0);
              printf("\033[%d;%dH\x1b[30m      (`\\(_|`\\|\x1b[0m\033[%d;%dH", CANVAS_HEIGHT + 3, CANVAS_WIDTH * 2 + 5, CANVAS_HEIGHT + 5, 0);
              printf("\033[%d;%dH\x1b[30m     (`\\,)  \\ \\\x1b[0m\033[%d;%dH", CANVAS_HEIGHT + 4, CANVAS_WIDTH * 2 + 5, CANVAS_HEIGHT + 5, 0);
              printf("\033[%d;%dH\x1b[30m      \\,)   | |\x1b[0m\033[%d;%dH", CANVAS_HEIGHT + 5, CANVAS_WIDTH * 2 + 5, CANVAS_HEIGHT + 5, 0);
              printf("\033[%d;%dH\x1b[30m        \\__(__|\x1b[0m\033[%d;%dH", CANVAS_HEIGHT + 6, CANVAS_WIDTH * 2 + 5, CANVAS_HEIGHT + 5, 0);
          }
          //printf("\033[%d;%dH\x1b[1;36m       _    (^)\x1b[0m\033[%d;%dH", CANVAS_HEIGHT - 3, CANVAS_WIDTH * 2 + 9, CANVAS_HEIGHT + 5, 0);
          if (move(canvas, state->x, state->y, state->rotate, state->x, state->y + 1, state->rotate, state->queue[0])) 
          {
              state->y++;
          }
          else 
          {
               state->score += clearLine(canvas);

               state->x = CANVAS_WIDTH / 2;
               state->y = 0;
               state->rotate = 0;
               state->fallTime = 0;
               state->queue[0] = state->queue[1];
               state->queue[1] = state->queue[2];
               state->queue[2] = state->queue[3];
               state->queue[3] = rand() % 7;

               if (!move(canvas, state->x, state->y, state->rotate, state->x, state->y, state->rotate, state->queue[0]))
               {
                  //printf("\033[%d;%dH\x1b[41m GAME OVER \x1b[0m\033[%d;%dH", CANVAS_HEIGHT - 3, CANVAS_WIDTH * 2 + 5, CANVAS_HEIGHT + 5, 0);
                  system("cls");
                  printf("\n");
                  //printf("\n");
                  printf("\033[5;1;31m ,----.      ,---.   ,--.   ,--. ,------.          ,-----.  ,--.   ,--. ,------. ,------. \033[m\n");
                  printf("\033[5;1;33m'  .-./     /  O  \\  |   `.'   | |  .---'         '  .-.  '  \\  `.'  /  |  .---' |  .--. '\033[m\n");
                  printf("\033[5;1;32m|  | .---. |  .-.  | |  |'.'|  | |  `--,          |  | |  |   \\     /   |  `--,  |  '--'.'\033[m\n");
                  printf("\033[5;1;34m'  '--'  | |  | |  | |  |   |  | |  `---.         '  '-'  '    \\   /    |  `---. |  |\\  \\ \033[m\n");
                  printf("\033[5;1;36m `------'  `--' `--' `--'   `--' `------'          `-----'      `-'     `------' `--' '--'\033[m\n");
                  printf("\n");
                  printf("                                       ");
                  printf("\033[1;41m YOUR SCORE : %d\033[m\n", state->score);
                  exit(0);
               }
          }
    }
    return;
}



int main()
{
    printf("\n");
    printf("\n");
    printf("\033[5;1;31m     :::::::::::        ::::::::::    :::::::::::        :::::::::        :::::::::::        ::::::::\033[m\n");
    printf("\033[5;33m        :+:            :+:               :+:            :+:    :+:           :+:           :+:    :+:\033[m\n");
    printf("\033[5;32m       +:+            +:+               +:+            +:+    +:+           +:+           +:+\033[m\n");
    printf("\033[5;34m      +#+            +#++:++#          +#+            +#++:++#:            +#+           +#++:++#++\033[m\n");
    printf("\033[5;36m     +#+            +#+               +#+            +#+    +#+           +#+                  +#+\033[m\n");
    printf("\033[5;35m    #+#            #+#               #+#            #+#    #+#           #+#           #+#    #+#\033[m\n");
    printf("\033[5;31m   ###            ##########        ###            ###    ###       ###########        ########\033[m\n");
    printf("\n");
    printf("\n");
    printf("                 ");
    printf("\033[36m ps : Pressing the number 2 can add two points, but with a cd_time. \033[m\n");
    printf("                              ");
    printf("\033[1;33;45mPress the number 1 to start the game.\033[m\n");
    //printf("Press the number 1 to start the game.\n");
    // 檢測鍵盤輸入
    while (1)
    {
        if (_kbhit())  // 檢測是否有鍵盤輸入
        {
            char key = _getch();  // 獲取按下的鍵值

            if (key == '1')  // 判斷是否按下數字鍵 1
            {
                break;  // 結束循環
            }
        }
    }
    srand(time(NULL));
    State state = {
        .x = CANVAS_WIDTH / 2,
        .y = 0,
        .score = 0,
        .rotate = 0,
        .fallTime = 0
    };
    state.cd_time = 0;
    state.ya_time = 0;
    for (int i = 0; i < 4; i++)
    {
        state.queue[i] = rand() % 7;
    }

    Block canvas[CANVAS_HEIGHT][CANVAS_WIDTH];
    for (int i = 0; i < CANVAS_HEIGHT; i++)
    {
        for (int j = 0; j < CANVAS_WIDTH; j++)
        {
            resetBlock(&canvas[i][j]);
        }
    }

    system("cls");
    // printf("\e[?25l"); // hide cursor

    move(canvas, state.x, state.y, state.rotate, state.x, state.y, state.rotate, state.queue[0]);

    while (1)
    {
        if (_kbhit())
        {
            char ch = _getch();
            if (ch == '2' && state.cd_time == 0 )
            {
                state.cd_time = 70;
                state.ya_time = 20;
                state.score += 2;
            }
        }
        if (state.ya_time > 0)
        {
            state.ya_time -= 1;
            state.cd_time -= 1;
        }
        if (state.cd_time > 0)
        {
            state.cd_time -= 1;
        }
        logic(canvas, &state);
        printCanvas(canvas, &state);
        Sleep(100);
    }

}
