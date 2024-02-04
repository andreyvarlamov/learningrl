#include <varand/varand_types.h>
#include <varand/varand_util.h>
#include <varand/varand_raylibhelper.h>

#include <raylib/raylib.h>

#include <cmath>

global_variable float gExponent = 1.0f; // Audio exponentiation value
global_variable float averageVolume[400] = {}; // Average volume history

void ProcessAudio(void *buffer, u32 frames)
{
    float *samples = (float *) buffer;
    float average = 0.0f;

    for (u32 frame = 0; frame < frames; frame++)
    {
        float *left = &samples[frame * 2 + 0], *right = &samples[frame * 2 + 1];

        *left = powf(fabsf(*left), gExponent) * ((*left < 0.0f) ? -1.0f : 1.0f);
        *right = powf(fabsf(*right), gExponent) * ((*right < 0.0f) ? -1.0f : 1.0f);

        average += fabsf(*left) / frames;
        average += fabsf(*right) / frames;
    }

    // Moving history to the left
    for (int i = 0; i < 399; i++) averageVolume[i] = averageVolume[i + 1];

    averageVolume[399] = average;
}

int main(int argv, char **argc)
{
    int screenWidth = 800;
    int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Learning");

    InitAudioDevice();

    AttachAudioMixedProcessor(ProcessAudio);

    Music music = LoadMusicStream("resources/country.mp3");
    Sound sound = LoadSound("resources/coin.wav");

    PlayMusicStream(music);

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        UpdateMusicStream(music);

        if (IsKeyPressed(KEY_LEFT)) gExponent -= 0.05f;
        if (IsKeyPressed(KEY_RIGHT)) gExponent += 0.05f;

        if (gExponent <= 0.5f) gExponent = 0.5f;
        if (gExponent >= 3.0f) gExponent = 3.0f;

        if (IsKeyPressed(KEY_SPACE)) PlaySound(sound);

        BeginDrawing();
            ClearBackground(RAYWHITE);
    
            DrawText("MUSIC SHOULD BE PLAYING!", 255, 150, 20, LIGHTGRAY);

            DrawText(TextFormat("EXPONENT = %.2f", gExponent), 215, 180, 20, LIGHTGRAY);

            DrawRectangle(199, 199, 402, 34, LIGHTGRAY);
            for (int i = 0; i < 400; i++)
            {
                DrawLine(201 + i, (int) (232 - averageVolume[i] * 32), 201 + i, 232, MAROON);
            }
            DrawRectangleLines(199, 199, 402, 34, GRAY);

            DrawText("PRESS SPACE TO PLAY OTHER SOUND", 200, 250, 20, LIGHTGRAY);
            DrawText("USE LEFT AND RIGHT ARROWS TO ALTER DISTORTION", 140, 280, 20, LIGHTGRAY);

        EndDrawing();
    }

    UnloadMusicStream(music); 

    DetachAudioMixedProcessor(ProcessAudio);

    CloseAudioDevice();

    CloseWindow();

    return 0;
}
