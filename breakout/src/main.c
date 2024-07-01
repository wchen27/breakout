#include <stdio.h>
#include "raylib.h"
#include "raymath.h"


typedef enum GameScreen {TITLE = 0, GAMEPLAY, SCORE} GameScreen;

struct ball {
    int x;
    int y;
    float xv;
    float yv;
};

struct paddle {
    int x;
    float xv;
};

struct brick {
    int x;
    int y;
    int active;
};

int main(void)
{
    const int screenWidth = 400;
    const int screenHeight = 600;

    int framesCounter = 0;

    InitWindow(screenWidth, screenHeight, "breakout!");

    SetTargetFPS(60);
    GameScreen currScreen = TITLE;

    struct ball ball = {screenWidth / 2, screenHeight / 2, 3.0f, 4.0f};
    struct paddle paddle = {screenWidth / 2 - 25, 0.0f};
    struct brick bricks[12][5] = {0};
    int score = 0;
    int highScore = 0;
    int textwidth = 0;

    while (!WindowShouldClose())
    {
        switch(currScreen) {
            case TITLE: 
            {
                // create 1 row of bricks
                for (int i = 0; i < 5; i++) {
                    bricks[0][i].x = i * 80 + 10;
                    bricks[0][i].y = 10;
                    bricks[0][i].active = 1;
                }
                if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                    currScreen = GAMEPLAY;
                }
            } break;
            
            case GAMEPLAY:
            {
                framesCounter++;

                if (IsKeyPressed(KEY_ENTER)) {
                    currScreen = SCORE;
                }

                // check if ball hits walls
                if (ball.x < 10) {
                    ball.x = 10;
                    ball.xv = -ball.xv;
                }

                if (ball.x > screenWidth - 10) {
                    ball.x = screenWidth - 10;
                    ball.xv = -ball.xv;
                }

                if (ball.y < 10) {
                    ball.y = 10;
                    ball.yv = -ball.yv;
                }


                if (ball.y > screenHeight - 10) {
                    if (score > highScore) {
                        highScore = score;
                    }
                    currScreen = SCORE;
                }

                ball.x += ball.xv;
                ball.y += ball.yv;
                
                if (IsKeyDown(KEY_LEFT)) {
                    paddle.xv = -5.0f;
                } else if (IsKeyDown(KEY_RIGHT)) {
                    paddle.xv = 5.0f;
                } else {
                    paddle.xv = 0.0f;
                }
                
                // make sure paddle doesn't go out of bounds
                if (paddle.x + paddle.xv > 0 && paddle.x + paddle.xv < screenWidth - 50) {
                    paddle.x += paddle.xv;
                } else {
                    paddle.xv = 0.0f;
                }

                if (CheckCollisionCircleRec((Vector2){ball.x, ball.y}, 10, (Rectangle){paddle.x, screenHeight - 30, 50, 10})) {

                    // adjust ball velocity based on where it hits the paddle
                    ball.xv = (ball.x - (paddle.x + 25)) / 5;
                    if (ball.xv > 4.0) {
                        ball.xv = 4.0;
                    } else if (ball.xv < -4.0) {
                        ball.xv = -4.0;
                    }
                    if (ball.xv < 1.0 && ball.xv > 0) {
                        ball.xv = 1.0;
                    } else if (ball.xv > -1.0 && ball.xv < 0) {
                        ball.xv = -1.0;
                    }
                    ball.yv = -sqrt(25 - ball.xv * ball.xv);
                    printf("ball.xv: %f, ball.yv: %f\n", ball.xv, ball.yv);

                }
                // every 5 seconds, spawn a new row of 5 bricks
                if (framesCounter % 600 == 0) {
                    for (int i = 0; i < 5; i++) {
                        for (int j = 11; j > 0; j--) {
                            bricks[j][i].x = bricks[j - 1][i].x;
                            bricks[j][i].y = bricks[j - 1][i].y + 30;
                            bricks[j][i].active = bricks[j - 1][i].active;
                        }
                    }
                    for (int i = 0; i < 5; i++) {
                        bricks[0][i].x = i * 80 + 10;
                        bricks[0][i].y = 10;
                        bricks[0][i].active = 1;
                    }
                }

                // check if ball hits bricks
                for (int i = 0; i < 5; i++) {
                    for (int j = 0; j < 5; j++) {
                        if (bricks[j][i].active) {
                            if (CheckCollisionCircleRec((Vector2){ball.x, ball.y}, 10, (Rectangle){bricks[j][i].x, bricks[j][i].y, 60, 10})) {
                                bricks[j][i].active = 0;
                                ball.yv = -ball.yv;
                                score++;
                            }
                        }
                    }
                }
                

            } break;

            case SCORE:
            {
                // reset ball and paddle
                ball.x = screenWidth / 2;
                ball.y = screenHeight / 2;
                ball.xv = 3.0f;
                ball.yv = 3.0f;

                // reset bricks
                for (int i = 0; i < 5; i++) {
                    for (int j = 1; j < 12; j++) {
                        bricks[j][i].x = bricks[j - 1][i].x;
                        bricks[j][i].y = bricks[j - 1][i].y + 30;
                        bricks[j][i].active = 0;
                    }
                }
                for (int i = 0; i < 5; i++) {
                    bricks[0][i].x = i * 80 + 5;
                    bricks[0][i].y = 10;
                    bricks[0][i].active = 1;
                }
                if (IsKeyPressed(KEY_ENTER)) {
                    currScreen = TITLE;
                }

            } break;
            default: break;

        }

        BeginDrawing();
        switch(currScreen) {
            case TITLE: 
            {
                DrawRectangle(0, 0, screenWidth, screenHeight, RAYWHITE);
                DrawText("Breakout!", screenWidth / 2, screenHeight / 2 - 20, 20, DARKGRAY);
                DrawText("Press Enter to Start", screenWidth / 2, screenHeight / 2, 10, DARKGRAY);
            } break;
            
            case GAMEPLAY:
            {
                DrawRectangle(0, 0, screenWidth, screenHeight, RAYWHITE);
                DrawCircle(ball.x, ball.y, 10, RED);
                DrawRectangle(paddle.x, screenHeight - 30, 50, 10, BLUE);

                // draw bricks
                for (int i = 0; i < 5; i++) {
                    for (int j = 0; j < 12; j++) {
                        if (bricks[j][i].active == 1) {
                            DrawRectangle(bricks[j][i].x, bricks[j][i].y, 60, 10, GREEN);
                        }
                    }
                }

                // draw score
                textwidth = MeasureText(TextFormat("%d", score), 20);
                // center text
                DrawText(TextFormat("%d", score), screenWidth / 2 - textwidth / 2, screenHeight - 100, 20, DARKGRAY);

            } break;

            case SCORE:
            {
                DrawRectangle(0, 0, screenWidth, screenHeight, RAYWHITE);
                DrawText("Game Over!", screenWidth / 2, screenHeight / 2 - 20, 20, DARKGRAY);
                DrawText(TextFormat("Score: %d", score), screenWidth / 2, screenHeight / 2, 20, DARKGRAY);
                DrawText(TextFormat("High Score: %d", highScore), screenWidth / 2, screenHeight / 2 + 20, 20, DARKGRAY);
            } break;
            default: break;

        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
