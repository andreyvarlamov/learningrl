#include <cstdio>

#include <raylib/raylib.h>

#include <varand/varand_types.h>

#include "learningrl.h"

enum GameScreen
{
    LOGO = 0,
    TITLE,
    GAMEPLAY,
    ENDING
};

int main(int argv, char **argc)
{
    int screenWidth = 800;
    int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Learning");

    GameScreen currentScreen = LOGO;

    int framesCounter = 0;

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        switch (currentScreen)
        {
            case LOGO:
            {
                framesCounter++;

                if (framesCounter > 120)
                {
                    currentScreen = TITLE;
                }
            } break;

            case TITLE:
            {
                if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
                {
                    currentScreen = GAMEPLAY;
                }
            } break;

            case GAMEPLAY:
            {
                if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
                {
                    currentScreen = ENDING;
                }
            } break;

            case ENDING:
            {
                if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
                {
                    currentScreen = TITLE;
                }
            } break;

            default: break;
        }

        BeginDrawing();

            ClearBackground(RAYWHITE);

            switch (currentScreen)
            {
                case LOGO:
                {
                    DrawText("Logo screen", 20, 20, 40, LIGHTGRAY);
                    DrawText("Wait for 2 seconds", 290, 220, 20, GRAY);
                } break;

                case TITLE:
                {
                    DrawRectangle(0, 0, screenWidth, screenHeight, GREEN);
                    DrawText("Title screen", 20, 20, 40, DARKGREEN);
                    DrawText("Press ENTER or TAP to jump to gameplay screen", 120, 220, 20, DARKGREEN);
                } break;

                case GAMEPLAY:
                {
                    DrawRectangle(0, 0, screenWidth, screenHeight, PURPLE);
                    DrawText("Gameplay screen", 20, 20, 40, MAROON);
                    DrawText("Press ENTER or TAP to jump to ending screen", 120, 220, 20, MAROON);
                } break;

                case ENDING:
                {
                    DrawRectangle(0, 0, screenWidth, screenHeight, BLUE);
                    DrawText("Ending screen", 20, 20, 40, DARKBLUE);
                    DrawText("Press ENTER or TAP to return to title screen", 120, 220, 20, DARKBLUE);
                } break;

                default: break;
            }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
