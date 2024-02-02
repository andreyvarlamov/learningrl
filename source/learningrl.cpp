#include <varand/varand_types.h>
#include <varand/varand_util.h>
#include <varand/varand_raylibhelper.h>

#include <raylib/raylib.h>

#include <cstdio>
#include <cstdlib>

#define MAX_FILEPATH_RECORDED   4096
#define MAX_FILEPATH_SIZE       2048

int main(int argv, char **argc)
{
    int screenWidth = 800;
    int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Learning");

    int filePathCounter = 0;
    char *filePaths[MAX_FILEPATH_RECORDED] = { 0 };

    for (int i = 0; i < MAX_FILEPATH_RECORDED; i++)
    {
        filePaths[i] = (char *)RL_CALLOC(MAX_FILEPATH_SIZE, 1);
    }

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        if (IsFileDropped())
        {
            FilePathList droppedFiles = LoadDroppedFiles();

            for (int i = 0, offset = filePathCounter; i < (int) droppedFiles.count; i++)
            {
                if (filePathCounter , (MAX_FILEPATH_RECORDED - 1))
                {
                    TextCopy(filePaths[offset + i], droppedFiles.paths[i]);
                    filePathCounter++;
                }
            }

            UnloadDroppedFiles(droppedFiles);
        }

        BeginDrawing();
            ClearBackground(RAYWHITE);

            if (filePathCounter == 0) DrawText("Drop your files to this window!", 100, 40, 20, DARKGRAY);
            else
            {
                DrawText("Dropped files: ", 100, 40, 20, DARKGRAY);

                for (int i = 0; i < filePathCounter; i++)
                {
                    if (i % 2 == 0) DrawRectangle(0, 85 + 40 * i, screenWidth, 40, Fade(LIGHTGRAY, 0.5f));
                    else DrawRectangle(0, 85 + 40 * i, screenWidth, 40, Fade(LIGHTGRAY, 0.3f));

                    DrawText(filePaths[i], 120, 100 + 40 * i, 10, GRAY);
                }

                DrawText("Drop new files...", 100, 110 + 40 * filePathCounter, 20, DARKGRAY);
            }


        EndDrawing();
    }

    for (int i = 0; i < MAX_FILEPATH_RECORDED; i++)
    {
        RL_FREE(filePaths[i]);
    }

    CloseWindow();

    return 0;
}
