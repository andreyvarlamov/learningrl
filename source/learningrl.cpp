#include <varand/varand_types.h>
#include <varand/varand_util.h>
#include <varand/varand_raylibhelper.h>

#include <raylib/raylib.h>
#include <raylib/raymath.h>

#define MAX(a, b) ((a)>(b)? (a) : (b))
#define MIN(a, b) ((a)<(b)? (a) : (b))

int main(int argv, char **argc)
{
    int screenWidth = 800;
    int screenHeight = 600;

    // Set configuration flags for window creation
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(screenWidth, screenHeight, "Learning");
    SetWindowMinSize(320, 240);

    int gameScreenWidth = 640;
    int gameScreenHeight = 480;

    RenderTexture2D target = LoadRenderTexture(gameScreenWidth, gameScreenHeight);
    SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);

    Color colors[10] = { 0 };
    for (int i = 0; i < 10; i++)
    {
        colors[i] = GetColor(GetRandomValue(100, 250), GetRandomValue(50, 150), GetRandomValue(10, 100), 255);
    }

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        float scale = MIN((float) GetScreenWidth() / gameScreenWidth, (float) GetScreenHeight() / gameScreenHeight);

        if (IsKeyPressed(KEY_SPACE))
        {
            for (int i = 0; i < 10; i++)
            {
                colors[i] = GetColor(GetRandomValue(100, 250), GetRandomValue(50, 150), GetRandomValue(10, 100), 255);
            }
        }

        Vector2 mouse = GetMousePosition();
        Vector2 virtualMouse = {};
        virtualMouse.x = (mouse.x - (GetScreenWidth() - (gameScreenWidth * scale)) * 0.5f) / scale;
        virtualMouse.y = (mouse.y - (GetScreenHeight() - (gameScreenHeight * scale)) * 0.5f) / scale;
        virtualMouse = Vector2Clamp(virtualMouse, GetVector2(), GetVector2((f32) gameScreenWidth, (f32) gameScreenHeight));

        // Apply the same transformation as the virtual mouse to the real mouse (i.e. to work with raygui)
        //SetMouseOffset(-(GetScreenWidth() - (gameScreenWidth*scale))*0.5f, -(GetScreenHeight() - (gameScreenHeight*scale))*0.5f);
        //SetMouseScale(1/scale, 1/scale);

        BeginTextureMode(target);
            ClearBackground(RAYWHITE);

            for (int i = 0; i < 10; i++)
            {
                DrawRectangle(0, (gameScreenHeight / 10) * i, gameScreenWidth, gameScreenHeight / 10, colors[i]);
            }

            DrawText("If executed inside a window,\nyou can resize the window,\nand see the screen scaling!", 10, 25, 20, WHITE);
            DrawText(TextFormat("Default Mouse: [%i , %i]", (int)mouse.x, (int)mouse.y), 350, 25, 20, GREEN);
            DrawText(TextFormat("Virtual Mouse: [%i , %i]", (int)virtualMouse.x, (int)virtualMouse.y), 350, 55, 20, YELLOW);
        EndTextureMode();

        BeginDrawing();
            ClearBackground(BLACK);

            DrawTexturePro(target.texture, GetRectangle((f32) target.texture.width, (f32) -target.texture.height),
                           GetRectangle((GetScreenWidth() - ((f32) gameScreenWidth * scale)) * 0.5f,
                                        (GetScreenHeight() - ((f32) gameScreenHeight * scale)) * 0.5f,
                                        (f32) gameScreenWidth * scale,
                                        (f32) gameScreenHeight * scale),
                           GetVector2(),
                           0.0f,
                           WHITE);

        EndDrawing();
    }

    UnloadRenderTexture(target);

    CloseWindow();

    return 0;
}
