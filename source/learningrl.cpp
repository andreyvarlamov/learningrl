#include <cstdio>

#include <raylib/raylib.h>

#include <varand/varand_types.h>
#include <varand/varand_util.h>
#include <varand/varand_raylibhelper.h>

#define PLAYER_SIZE 40

int main(int argv, char **argc)
{
    int screenWidth = 800;
    int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Learning");

    Rectangle player1 = GetRectangle(200, 200, PLAYER_SIZE);
    Rectangle player2 = GetRectangle(250, 200, PLAYER_SIZE);

    Camera2D camera1 = {};
    camera1.target = GetRectangleMin(player1);
    camera1.offset = GetVector2(200.0f, 200.0f);
    camera1.rotation = 0.0f;
    camera1.zoom = 1.0f;

    Camera2D camera2 = {};
    camera2.target = GetRectangleMin(player2);
    camera2.offset = GetVector2(200.0f, 200.0f);
    camera2.rotation = 0.0f;
    camera2.zoom = 1.0f;

    RenderTexture screenCamera1 = LoadRenderTexture(screenWidth / 2, screenHeight);
    RenderTexture screenCamera2 = LoadRenderTexture(screenWidth / 2, screenHeight);

    Rectangle splitScreenRect = GetRectangle(0.0f, 0.0f, (f32) screenCamera1.texture.width, (f32) -screenCamera1.texture.height);

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        if (IsKeyDown(KEY_S)) player1.y += 3.0f;
        if (IsKeyDown(KEY_W)) player1.y -= 3.0f;
        if (IsKeyDown(KEY_D)) player1.x += 3.0f;
        if (IsKeyDown(KEY_A)) player1.x -= 3.0f;

        if (IsKeyDown(KEY_DOWN)) player2.y += 3.0f;
        if (IsKeyDown(KEY_UP)) player2.y -= 3.0f;
        if (IsKeyDown(KEY_RIGHT)) player2.x += 3.0f;
        if (IsKeyDown(KEY_LEFT)) player2.x -= 3.0f;

        camera1.target = GetRectangleMin(player1);
        camera2.target = GetRectangleMin(player2);

        BeginTextureMode(screenCamera1);
            ClearBackground(RAYWHITE);

            BeginMode2D(camera1);

                for (int i = 0; i < screenWidth / PLAYER_SIZE + 1; i++)
                {
                    DrawLineV(GetVector2((f32) PLAYER_SIZE * i, 0), GetVector2((f32) PLAYER_SIZE * i, (f32) screenHeight), LIGHTGRAY);
                }

                for (int i = 0; i < screenHeight / PLAYER_SIZE + 1; i++)
                {
                    DrawLineV(GetVector2(0, (f32) PLAYER_SIZE * i), GetVector2((f32) screenWidth, (f32) PLAYER_SIZE * i), LIGHTGRAY);
                }

                for (int i = 0; i < screenWidth / PLAYER_SIZE; i++)
                {
                    for (int j = 0; j < screenHeight / PLAYER_SIZE; j++)
                    {
                        DrawText(TextFormat("[%i,%i]", i, j), 10 + PLAYER_SIZE * i, 15 + PLAYER_SIZE*j, 10, LIGHTGRAY);
                    }
                }

                DrawRectangleRec(player1, RED);
                DrawRectangleRec(player2, BLUE);

            EndMode2D();

            DrawRectangle(0, 0, GetScreenWidth() / 2, 30, Fade(RAYWHITE, 0.6f));
            DrawText("PLAYER1: W/S/A/D to move", 10, 10, 10, MAROON);

        EndTextureMode();

        BeginTextureMode(screenCamera2);
            ClearBackground(RAYWHITE);

            BeginMode2D(camera2);

                for (int i = 0; i < screenWidth / PLAYER_SIZE + 1; i++)
                {
                    DrawLineV(GetVector2((f32) PLAYER_SIZE * i, 0), GetVector2((f32) PLAYER_SIZE * i, (f32) screenHeight), LIGHTGRAY);
                }

                for (int i = 0; i < screenHeight / PLAYER_SIZE + 1; i++)
                {
                    DrawLineV(GetVector2(0, (f32) PLAYER_SIZE * i), GetVector2((f32) screenWidth, (f32) PLAYER_SIZE * i), LIGHTGRAY);
                }

                for (int i = 0; i < screenWidth / PLAYER_SIZE; i++)
                {
                    for (int j = 0; j < screenHeight / PLAYER_SIZE; j++)
                    {
                        DrawText(TextFormat("[%i,%i]", i, j), 10 + PLAYER_SIZE * i, 15 + PLAYER_SIZE*j, 10, LIGHTGRAY);
                    }
                }

                DrawRectangleRec(player1, RED);
                DrawRectangleRec(player2, BLUE);

            EndMode2D();

            DrawRectangle(0, 0, GetScreenWidth() / 2, 30, Fade(RAYWHITE, 0.6f));
            DrawText("PLAYER2: UP/DOWN/LEFT/RIGHT to move", 10, 10, 10, DARKBLUE);

        EndTextureMode();

        BeginDrawing();
            ClearBackground(LIGHTGRAY);

            DrawTextureRec(screenCamera1.texture, splitScreenRect, GetVector2(0, 0), WHITE);
            DrawTextureRec(screenCamera2.texture, splitScreenRect, GetVector2(screenWidth / 2.0f, 0), WHITE);

            DrawRectangle(GetScreenWidth() / 2 - 2, 0, 4, GetScreenHeight(), LIGHTGRAY);

        EndDrawing();
    }

    UnloadRenderTexture(screenCamera1);
    UnloadRenderTexture(screenCamera2);

    CloseWindow();

    return 0;
}
