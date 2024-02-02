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

#define MAX_BUILDINGS 100

int main(int argv, char **argc)
{
    int screenWidth = 800;
    int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Learning");

    Rectangle player = { 400, 280, 40, 40 };
    Rectangle buildings[MAX_BUILDINGS] = { 0 };
    Color buildColors[MAX_BUILDINGS] = { 0 };

    int spacing =  0;

    for (int i = 0; i < MAX_BUILDINGS; i++)
    {
        buildings[i].width = (f32) GetRandomValue(50, 200);
        buildings[i].height = (f32) GetRandomValue(100, 800);
        buildings[i].y = screenHeight - 130.0f - buildings[i].height;
        buildings[i].x = -6000.0f + spacing;

        spacing += (int) buildings[i].width;
        buildColors[i] = { (u8) GetRandomValue(200, 240), (u8) GetRandomValue(200, 240), (u8) GetRandomValue(200, 250), 255 };
    }

    Camera2D camera = { 0 };
    camera.target = { player.x + 20.0f, player.y + 20.0f };
    camera.offset = { screenWidth / 2.0f, screenHeight / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        if (IsKeyDown(KEY_RIGHT)) player.x += 2;
        else if (IsKeyDown(KEY_LEFT)) player.x -= 2;

        camera.target = { player.x + 20.0f, player.y + 20.0f };

        if (IsKeyDown(KEY_A)) camera.rotation--;
        else if (IsKeyDown(KEY_S)) camera.rotation++;

        if (camera.rotation > 40) camera.rotation = 40;
        else if (camera.rotation < -40) camera.rotation = -40;

        camera.zoom += ((f32) GetMouseWheelMove() * 0.05f);

        if (camera.zoom > 3.0f) camera.zoom = 3.0f;
        else if (camera.zoom < 0.3f) camera.zoom = 0.1f;

        if (IsKeyPressed(KEY_R))
        {
            camera.zoom = 1.0f;
            camera.rotation = 0.0f;
        }

        BeginDrawing();

            ClearBackground(RAYWHITE);

            BeginMode2D(camera);

                DrawRectangle(-6000, 320, 13000, 8000, DARKGRAY);

                for (int i = 0; i < MAX_BUILDINGS; i++) DrawRectangleRec(buildings[i], buildColors[i]);

                DrawRectangleRec(player, RED);

                DrawLine((int) camera.target.x, -screenHeight * 10, (int) camera.target.x, screenHeight * 10, GREEN);
                DrawLine(-screenWidth * 10, (int) camera.target.x, screenWidth * 10, (int) camera.target.y, GREEN);

            EndMode2D();

            DrawText("SCREEN AREA", 640, 10, 20, RED);

            DrawRectangle(0, 0, screenWidth, 5, RED);
            DrawRectangle(0, 5, 5, screenHeight - 10, RED);
            DrawRectangle(screenWidth - 5, 5, 5, screenHeight - 10, RED);
            DrawRectangle(0, screenHeight - 5, screenWidth, 5, RED);

            DrawRectangle(10, 10, 250, 113, Fade(SKYBLUE, 0.5f));
            DrawRectangleLines(10, 10, 250, 113, BLUE);

            DrawText("Free 2d camera controls:", 20, 20, 10, BLACK);
            DrawText("- Right/left to move Offset", 40, 40, 10, DARKGRAY);
            DrawText("- Mouse Wheel to Zoom in-out", 40, 60, 10, DARKGRAY);
            DrawText("- A/S to Rotate", 40, 80, 10, DARKGRAY);
            DrawText("- R to reset Zoom and Rotation", 40, 100, 10, DARKGRAY);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
