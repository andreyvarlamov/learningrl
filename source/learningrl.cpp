#include <cstdio>

#include <raylib/raylib.h>
#include <raylib/rcamera.h>

#include <varand/varand_types.h>
#include <varand/varand_util.h>
#include <varand/varand_raylibhelper.h>

#define MAX_COLUMNS 20

int main(int argv, char **argc)
{
    int screenWidth = 800;
    int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Learning");

    Camera camera = {};
    camera.position = GetVector3(0.0f, 2.0f, 4.0f);
    camera.target = GetVector3(0.0f, 2.0f, 0.0f);
    camera.up = GetVector3(0.0f, 1.0f, 0.0f);
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    int cameraMode = CAMERA_FIRST_PERSON;

    f32 heights[MAX_COLUMNS] = {};
    Vector3 positions[MAX_COLUMNS] = {};
    Color colors[MAX_COLUMNS] = {};

    for (int i = 0; i < MAX_COLUMNS; i++)
    {
        heights[i] = (f32) GetRandomValue(1, 12);
        positions[i] = GetVector3((f32) GetRandomValue(-15, 15), heights[i] / 2.0f, (f32) GetRandomValue(-15, 15));
        colors[i] = GetColor(GetRandomValue(20, 255), GetRandomValue(10, 55), 30);
    }

    DisableCursor();

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        if (IsKeyDown(KEY_ONE))
        {
            cameraMode = CAMERA_FREE;
            camera.up = GetVector3(0.0f, 1.0f, 0.0f); // Reset roll
        }

        if (IsKeyDown(KEY_TWO))
        {
            cameraMode = CAMERA_FIRST_PERSON;
            camera.up = GetVector3(0.0f, 1.0f, 0.0f);
        }

        if (IsKeyDown(KEY_THREE))
        {
            cameraMode = CAMERA_THIRD_PERSON;
            camera.up = GetVector3(0.0f, 1.0f, 0.0f);
        }

        if (IsKeyDown(KEY_FOUR))
        {
            cameraMode = CAMERA_ORBITAL;
            camera.up = GetVector3(0.0f, 1.0f, 0.0f);
        }

        if (IsKeyPressed(KEY_P))
        {
            if (camera.projection == CAMERA_PERSPECTIVE)
            {
                cameraMode = CAMERA_THIRD_PERSON;
                camera.position = GetVector3(0.0f, 2.0f, -100.0f);
                camera.target = GetVector3(0.0f, 2.0f, 0.0f);
                camera.up = GetVector3(0.0f, 1.0f, 0.0f);
                camera.projection = CAMERA_ORTHOGRAPHIC;
                camera.fovy = 20.0f; // near plane width in CAMERA_ORTHOGRAPHIC
                CameraYaw(&camera, -135 * DEG2RAD, true);
                CameraPitch(&camera, -45 * DEG2RAD, true, true, false);
            }
            else if (camera.projection == CAMERA_ORTHOGRAPHIC)
            {
                cameraMode = CAMERA_THIRD_PERSON;
                camera.position = GetVector3(0.0f, 2.0f, 10.0f);
                camera.target = GetVector3(0.0f, 2.0f, 0.0f);
                camera.up = GetVector3(0.0f, 1.0f, 0.0f);
                camera.projection = CAMERA_PERSPECTIVE;
                camera.fovy = 60.0f;
            }
        }

        // Computes movement internally with some default keyboard/mouse inputs
        UpdateCamera(&camera, cameraMode);

        // Custom movement
        #if 0
        UpdateCameraPro(&camera,
                        GetVector3(IsKeyDown(KEY_W) * 0.1f - IsKeyDown(KEY_S) * 0.1f,
                                   IsKeyDown(KEY_D) * 0.1f - IsKeyDown(KEY_A) * 0.1f,
                                   0.0f),
                        GetVector3(GetMouseDelta().x * 0.05f,
                                   GetMouseDelta().y * 0.05f,
                                   0.0f),
                        GetMouseWheelMove() * 2.0f); // Move to target (zoom)
        #endif

        BeginDrawing();
            ClearBackground(RAYWHITE);

            BeginMode3D(camera);

                DrawPlane(GetVector3(), GetVector2(32.0f), LIGHTGRAY);
                DrawCube(GetVector3(-16.0f, 2.5f), 1.0f, 5.0f, 32.0f, BLUE);
                DrawCube(GetVector3(16.0f, 2.5f), 1.0f, 5.0f, 32.0f, LIME);
                DrawCube(GetVector3(0.0f, 2.5f, 16.0f), 32.0f, 5.0f, 1.0f, GOLD);

                for (int i = 0; i < MAX_COLUMNS; i++)
                {
                    DrawCube(positions[i], 2.0f, heights[i], 2.0f, colors[i]);
                    DrawCubeWires(positions[i], 2.0f, heights[i], 2.0f, MAROON);
                }

                if (cameraMode == CAMERA_THIRD_PERSON)
                {
                    DrawCube(camera.target, 0.5f, 0.5f, 0.5f, PURPLE);
                    DrawCubeWires(camera.target, 0.5f, 0.5f, 0.5f, DARKPURPLE);
                }

            EndMode3D();

            DrawRectangle(5, 5, 330, 100, Fade(SKYBLUE, 0.5f));
            DrawRectangleLines(5, 5, 330, 100, BLUE);

            DrawText("Camera controls:", 15, 15, 10, BLACK);
            DrawText("- Move keys: W, A, S, D, Space, Left-Ctrl", 15, 30, 10, BLACK);
            DrawText("- Look around: arrow keys or mouse", 15, 45, 10, BLACK);
            DrawText("- Camera mode keys: 1, 2, 3, 4", 15, 60, 10, BLACK);
            DrawText("- Zoom keys: num-plus, num-minus or mouse scroll", 15, 75, 10, BLACK);
            DrawText("- Camera projection key: P", 15, 90, 10, BLACK);

            DrawRectangle(600, 5, 195, 100, Fade(SKYBLUE, 0.5f));
            DrawRectangleLines(600, 5, 195, 100, BLUE);

            DrawText("Camera status:", 610, 15, 10, BLACK);
            DrawText(TextFormat("- Mode: %s", (cameraMode == CAMERA_FREE) ? "FREE" :
                                              (cameraMode == CAMERA_FIRST_PERSON) ? "FIRST_PERSON" :
                                              (cameraMode == CAMERA_THIRD_PERSON) ? "THIRD_PERSON" :
                                              (cameraMode == CAMERA_ORBITAL) ? "ORBITAL" : "CUSTOM"), 610, 30, 10, BLACK);
            DrawText(TextFormat("- Projection: %s", (camera.projection == CAMERA_PERSPECTIVE) ? "PERSPECTIVE" :
                                                    (camera.projection == CAMERA_ORTHOGRAPHIC) ? "ORTHOGRAPHIC" : "CUSTOM"), 610, 45, 10, BLACK);
            DrawText(TextFormat("- Position: (%06.3f, %06.3f, %06.3f)", camera.position.x, camera.position.y, camera.position.z), 610, 60, 10, BLACK);
            DrawText(TextFormat("- Target: (%06.3f, %06.3f, %06.3f)", camera.target.x, camera.target.y, camera.target.z), 610, 75, 10, BLACK);
            DrawText(TextFormat("- Up: (%06.3f, %06.3f, %06.3f)", camera.up.x, camera.up.y, camera.up.z), 610, 90, 10, BLACK);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
