#include <varand/varand_types.h>
#include <varand/varand_util.h>
#include <varand/varand_raylibhelper.h>

#include <raylib/raylib.h>

int main(int argv, char **argc)
{
    int screenWidth = 800;
    int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Learning");

    // NOTE: Textures must be loaded after window initialization (opengl context is required)

    Image cat = LoadImage("resources/cat.png"); // Load image in RAM
    ImageCrop(&cat, GetRectangle(100,10,280,380));
    ImageFlipHorizontal(&cat);
    ImageResize(&cat, 150, 200);

    Image parrots = LoadImage("resources/parrots.png");

    // Draw one image over the other with a scaling of 1.5f
    ImageDraw(&parrots,
              cat,
              GetRectangle((float) cat.width, (float) cat.height),
              GetRectangle(30, 40, cat.width * 1.5f, cat.height * 1.5f),
              WHITE);
    ImageCrop(&parrots, GetRectangle(0, 50, (float) parrots.width, (float) parrots.height - 100));

    // Draw on the image witha few image draw methods
    ImageDrawPixel(&parrots, 10, 10, RAYWHITE);
    ImageDrawCircleLines(&parrots, 10, 10, 5, RAYWHITE);
    ImageDrawRectangle(&parrots, 5, 20, 10, 10, RAYWHITE);

    UnloadImage(cat); // Unload image from RAM

    // Load custom font for drawing on image
    Font font = LoadFont("resources/custom_jupiter_crash.png");

    ImageDrawTextEx(&parrots, font, "PARROTS & CAT", GetVector2(300, 230), (float) font.baseSize, -2, WHITE);

    UnloadFont(font);

    Texture2D texture = LoadTextureFromImage(parrots); // Image converted to texture, uploaded to VRAM

    UnloadImage(parrots); // Unload image from RAM

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();
            ClearBackground(RAYWHITE);
    
            DrawTexture(texture, screenWidth / 2 - texture.width / 2, screenHeight / 2 - texture.height / 2 - 40, WHITE);
            DrawRectangleLines(screenWidth / 2 - texture.width / 2, screenHeight / 2 - texture.height / 2 - 40, texture.width, texture.height, DARKGRAY);

            DrawText("We are drawing only one texture from various images composed!", 240, 350, 10, DARKGRAY);
            DrawText("Source images have been cropped, scaled, flipped and copied one over the other.", 190, 370, 10, DARKGRAY);

        EndDrawing();
    }

    UnloadTexture(texture);

    CloseWindow();

    return 0;
}
