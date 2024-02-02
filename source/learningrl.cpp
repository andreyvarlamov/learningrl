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

    Camera camera = { 0 };
    camera.position = GetVector3(10.0f, 10.0f, 10.0f); // Camera position
    camera.target = GetVector3(0.0f, 0.0f, 0.0f);      // Camera looking at point
    camera.up = GetVector3(0.0f, 1.0f, 0.0f);          // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE; 

    Vector3 cubePosition = GetVector3(0.0f, 1.0f, 0.0f);
    Vector3 cubeSize  = GetVector3(2.0f, 2.0f, 2.0f);

    Ray ray = {};
    RayCollision collision = {};

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        if (IsCursorHidden()) UpdateCamera(&camera, CAMERA_FIRST_PERSON);

        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
        {
            if (IsCursorHidden()) EnableCursor();
            else DisableCursor();
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            if (!collision.hit)
            {
                ray = GetMouseRay(GetMousePosition(), camera);

                // Check collision between ray and box
                collision = GetRayCollisionBox(ray,
                                               GetBoundingBox(GetVector3(cubePosition.x - cubeSize.x/2,
                                                                         cubePosition.y - cubeSize.y/2,
                                                                         cubePosition.z - cubeSize.z/2),
                                                              GetVector3(cubePosition.x + cubeSize.x/2,
                                                                         cubePosition.y + cubeSize.y/2,
                                                                         cubePosition.z + cubeSize.z/2)));
            }
            else collision.hit = false;
        }

        BeginDrawing();
            ClearBackground(RAYWHITE);

            BeginMode3D(camera);

                if (collision.hit)
                {
                    DrawCube(cubePosition, cubeSize.x, cubeSize.y, cubeSize.z, RED);
                    DrawCubeWires(cubePosition, cubeSize.x, cubeSize.y, cubeSize.z, MAROON);

                    DrawCubeWires(cubePosition, cubeSize.x + 0.2f, cubeSize.y + 0.2f, cubeSize.z + 0.2f, GREEN);
                }
                else
                {
                    DrawCube(cubePosition, cubeSize.x, cubeSize.y, cubeSize.z, GRAY);
                    DrawCubeWires(cubePosition, cubeSize.x, cubeSize.y, cubeSize.z, DARKGRAY);
                }

                DrawRay(ray, MAROON);
                DrawGrid(10, 1.0f);

            EndMode3D();

            DrawText("Try clicking on the box with your mouse!", 240, 10, 20, DARKGRAY);

            if (collision.hit) DrawText("BOX SELECTED", (screenWidth - MeasureText("BOX SELECTED", 30)) / 2, (int)(screenHeight * 0.1f), 30, GREEN);

            DrawText("Right click mouse to toggle camera controls", 10, 430, 10, GRAY);

            DrawFPS(10, 10);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
