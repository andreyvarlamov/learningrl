#include <cstdio>

#include <raylib/raylib.h>
#include <raylib/rlgl.h>
#include <raylib/raymath.h>

#include <varand/varand_types.h>

int main(int argv, char **argc)
{
    int screenWidth = 800;
    int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Learning");

    Camera2D camera = {};
    camera.zoom = 1.0f;

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
        {
            Vector2 delta = GetMouseDelta();
            delta = Vector2Scale(delta, -1.0f / camera.zoom);

            camera.target = Vector2Add(camera.target, delta);
        }

        float wheel = GetMouseWheelMove();

        if (wheel != 0)
        {
            Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);

            camera.offset = GetMousePosition();

            camera.target = mouseWorldPos;

            f32 zoomIncrement = 0.125f;

            camera.zoom += (wheel * zoomIncrement);
            if (camera.zoom < zoomIncrement) camera.zoom = zoomIncrement;
        }

        BeginDrawing();
            ClearBackground(BLACK);

            BeginMode2D(camera);

                rlPushMatrix();
                    rlTranslatef(0, 25*50, 0);
                    rlRotatef(90, 1, 0, 0);
                    DrawGrid(100, 50);
                rlPopMatrix();

                DrawCircle(100, 100, 50, YELLOW);

            EndMode2D();

            DrawText("Mouse right button drag to move, mouse wheel to zoom", 10, 10, 20, WHITE);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
