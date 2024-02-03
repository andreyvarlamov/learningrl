#include <varand/varand_types.h>
#include <varand/varand_util.h>
#include <varand/varand_raylibhelper.h>

#include <raylib/raylib.h>

int main(int argv, char **argc)
{
    int screenWidth = 800;
    int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Learning");

    // NOTE: Textures must be loaded after window initialization (opengl context is required)

    Texture2D scarfy = LoadTexture("resources/scarfy.png");

    int frameWidth = scarfy.width / 6; // first from animated sprite atlas
    int frameHeight = scarfy.height;

    Rectangle sourceRec = GetRectangle(0.0f, 0.0f, (float) frameWidth, (float) frameHeight);
    Rectangle destRec = GetRectangle(screenWidth / 2.0f, screenHeight / 2.0f, frameWidth * 2.0f, frameHeight * 2.0f);

    Vector2 origin = GetVector2((float) frameWidth, (float) frameHeight);

    int rotation = 0;

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        rotation++;

        BeginDrawing();
            ClearBackground(RAYWHITE);
            
            DrawTexturePro(scarfy, sourceRec, destRec, origin, (float) rotation, WHITE);

            DrawLine((int) destRec.x, 0, (int) destRec.x, screenHeight, GRAY);
            DrawLine(0, (int) destRec.y, screenWidth, (int) destRec.y, GRAY);

            DrawText("(c) Scarfy sprite by Eiden Marsal", screenWidth - 200, screenHeight - 20, 10, GRAY);

        EndDrawing();
    }

    UnloadTexture(scarfy);

    CloseWindow();

    return 0;
}
