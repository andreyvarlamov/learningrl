#include <cstdio>

#include <raylib/raylib.h>

#include <varand/varand_types.h>
#include <varand/varand_util.h>
#include <varand/varand_raylibhelper.h>

int main(int argv, char **argc)
{
    int screenWidth = 800;
    int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Learning");

    Camera cameraPlayer1 = { 0 };
    cameraPlayer1.fovy = 45.0f;
    cameraPlayer1.up.y = 1.0f;
    cameraPlayer1.target.y = 1.0f;
    cameraPlayer1.position.z = -3.0f;
    cameraPlayer1.position.y = 1.0f;

    RenderTexture screenPlayer1 = LoadRenderTexture(screenWidth/2, screenHeight);

    Camera cameraPlayer2 = { 0 };
    cameraPlayer2.fovy = 45.0f;
    cameraPlayer2.up.y = 1.0f;
    cameraPlayer2.target.y = 3.0f;
    cameraPlayer2.position.x = -3.0f;
    cameraPlayer2.position.y = 3.0f;

    RenderTexture screenPlayer2 = LoadRenderTexture(screenWidth / 2, screenHeight);

    Rectangle splitScreenRect = GetRectangle(0.0f, 0.0f, (float)screenPlayer1.texture.width, (float)-screenPlayer1.texture.height);

    // Grid data
    int count = 5;
    float spacing = 4;

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        float offsetThisFrame = 10.0f*GetFrameTime();

        if (IsKeyDown(KEY_W))
        {
            cameraPlayer1.position.z += offsetThisFrame;
            cameraPlayer1.target.z += offsetThisFrame;
        }
        else if (IsKeyDown(KEY_S))
        {
            cameraPlayer1.position.z -= offsetThisFrame;
            cameraPlayer1.target.z -= offsetThisFrame;
        }

        if (IsKeyDown(KEY_UP))
        {
            cameraPlayer2.position.x += offsetThisFrame;
            cameraPlayer2.target.x += offsetThisFrame;
        }
        else if (IsKeyDown(KEY_DOWN))
        {
            cameraPlayer2.position.x -= offsetThisFrame;
            cameraPlayer2.target.x -= offsetThisFrame;
        }

        BeginTextureMode(screenPlayer1);
            ClearBackground(SKYBLUE);

            BeginMode3D(cameraPlayer1);
            
                DrawPlane(GetVector3(), GetVector2(50), BEIGE);

                for (float x = -count*spacing; x <= count*spacing; x += spacing)
                {
                    for (float z = -count*spacing; z <= count*spacing; z += spacing)
                    {
                        DrawCube(GetVector3(x, 1.5f, z), 1, 1, 1, LIME);
                        DrawCube(GetVector3(x, 0.5f, z), 0.25f, 1, 0.25f, BROWN);
                    }
                }

                DrawCube(cameraPlayer1.position, 1, 1, 1, RED);
                DrawCube(cameraPlayer2.position, 1, 1, 1, BLUE);
            
            EndMode3D();

            DrawRectangle(0, 0, GetScreenWidth()/2, 40, Fade(RAYWHITE, 0.8f));
            DrawText("PLAYER1: W/S to move", 10, 10, 20, MAROON);

        EndTextureMode();

        BeginTextureMode(screenPlayer2);
            ClearBackground(MAROON);

            BeginMode3D(cameraPlayer2);
            
                DrawPlane(GetVector3(), GetVector2(50, 50), BEIGE);

                for (float x = -count*spacing; x <= count*spacing; x += spacing)
                {
                    for (float z = -count*spacing; z <= count*spacing; z += spacing)
                    {
                        DrawCube(GetVector3(x, 1.5f, z), 1, 1, 1, LIME);
                        DrawCube(GetVector3(x, 0.5f, z), 0.25f, 1, 0.25f, BROWN);
                    }
                }

                DrawCube(cameraPlayer1.position, 1, 1, 1, RED);
                DrawCube(cameraPlayer2.position, 1, 1, 1, BLUE);
            
            EndMode3D();

            DrawRectangle(0, 0, GetScreenWidth()/2, 40, Fade(RAYWHITE, 0.8f));
            DrawText("PLAYER2: UP/DOWN to move", 10, 10, 20, DARKBLUE);

        EndTextureMode();

        BeginDrawing();
            ClearBackground(RAYWHITE);

            DrawTextureRec(screenPlayer1.texture, splitScreenRect, GetVector2(), WHITE);
            DrawTextureRec(screenPlayer2.texture, splitScreenRect, GetVector2(screenWidth/2.0f, 0), WHITE);
            
            DrawRectangle(GetScreenWidth()/2 - 2, 0, 4, GetScreenHeight(), LIGHTGRAY);
        EndDrawing();
    }

    UnloadRenderTexture(screenPlayer1);
    UnloadRenderTexture(screenPlayer2);

    CloseWindow();

    return 0;
}
