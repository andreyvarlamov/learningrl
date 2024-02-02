#include <cstdio>

#include <raylib/raylib.h>

#include <varand/varand_types.h>

#include "learningrl.h"

int main(int argv, char **argc)
{
    i32 screenWidth = 800;
    i32 screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Learning");

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();

            ClearBackground(RAYWHITE);
            DrawText("Hello world", 190, 200, 20, LIGHTGRAY);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
