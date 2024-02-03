#include <varand/varand_types.h>
#include <varand/varand_util.h>
#include <varand/varand_raylibhelper.h>

#include <raylib/raylib.h>

#include <cstdlib>
#include <cstdio>

#define STORAGE_DATA_FILE "storage.data"

enum StorageData
{
    STORAGE_POSITION_SCORE = 0,
    STORAGE_POSITION_HISCORE = 1
};

internal b32 SaveStorageValue(u32 position, int value);
internal int LoadStorageValue(u32 position);

int main(int argv, char **argc)
{
    int screenWidth = 800;
    int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Learning");

    int score = 0;
    int hiscore = 0;
    int framesCounter = 0;

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_R))
        {
            score = GetRandomValue(1000, 2000);
            hiscore = GetRandomValue(2000, 4000);
        }

        if (IsKeyPressed(KEY_ENTER))
        {
            SaveStorageValue(STORAGE_POSITION_SCORE, score);
            SaveStorageValue(STORAGE_POSITION_HISCORE, hiscore);
        }
        else if (IsKeyPressed(KEY_SPACE))
        {
            score = LoadStorageValue(STORAGE_POSITION_SCORE);
            hiscore = LoadStorageValue(STORAGE_POSITION_HISCORE);
        }

        framesCounter++;

        BeginDrawing();
            ClearBackground(RAYWHITE);

            DrawText(TextFormat("SCORE: %i", score), 280, 130, 40, MAROON);
            DrawText(TextFormat("HI-SCORE: %i", hiscore), 210, 200, 50, BLACK);

            DrawText(TextFormat("frames: %i", framesCounter), 10, 10, 20, LIME);

            DrawText("Press R togenerate random numbers", 220, 40, 20, LIGHTGRAY);
            DrawText("Press ENTER to SAVE values", 250, 310, 20, LIGHTGRAY);
            DrawText("Press SPACE to LOAD values", 252, 350, 20, LIGHTGRAY);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}

#define TRACELOG TraceLog
internal b32 SaveStorageValue(u32 position, int value)
{
    b32 success = false;
    int dataSize = 0;
    u32 newDataSize = 0;
#if 0
    // For some reason there's a problem with this function, its allocation is invalid, and even freeing it right away
    // causes an issue
    // I used the version of LoadFileData from a commit from the 5.0 release and it works.
    // https://github.com/raysan5/raylib/blob/ae50bfa2cc569c0f8d5bc4315d39db64005b1b08/src/utils.c
    // u8 *fileData = LoadFileData(STORAGE_DATA_FILE, &dataSize);
    // RL_FREE(fileData);
#else
    const char *fileName = STORAGE_DATA_FILE;
    u8 *fileData = 0;

    FILE *file = fopen(fileName, "rb");

    if (file != NULL)
    {
        // WARNING: On binary streams SEEK_END could not be found,
        // using fseek() and ftell() could not work in some (rare) cases
        fseek(file, 0, SEEK_END);
        int size = ftell(file);     // WARNING: ftell() returns 'long int', maximum size returned is INT_MAX (2147483647 bytes)
        fseek(file, 0, SEEK_SET);

        if (size > 0)
        {
            fileData = (unsigned char *)RL_MALLOC(size*sizeof(unsigned char));

            if (fileData != NULL)
            {
                // NOTE: fread() returns number of read elements instead of bytes, so we read [1 byte, size elements]
                size_t count = fread(fileData, sizeof(unsigned char), size, file);
                
                // WARNING: fread() returns a size_t value, usually 'unsigned int' (32bit compilation) and 'unsigned long long' (64bit compilation)
                // dataSize is unified along raylib as a 'int' type, so, for file-sizes > INT_MAX (2147483647 bytes) we have a limitation
                if (count > 2147483647)
                {
                    TRACELOG(LOG_WARNING, "FILEIO: [%s] File is bigger than 2147483647 bytes, avoid using LoadFileData()", fileName);
                    
                    RL_FREE(fileData);
                    fileData = NULL;
                }
                else
                {
                    dataSize = (int)count;

                    if (dataSize != size) TRACELOG(LOG_WARNING, "FILEIO: [%s] File partially loaded (%i bytes out of %i)", fileName, dataSize, count);
                    else TRACELOG(LOG_INFO, "FILEIO: [%s] File loaded successfully", fileName);
                }
            }
            else TRACELOG(LOG_WARNING, "FILEIO: [%s] Failed to allocated memory for file reading", fileName);
        }
        else TRACELOG(LOG_WARNING, "FILEIO: [%s] Failed to read file", fileName);

        fclose(file);
    }
    else TRACELOG(LOG_WARNING, "FILEIO: [%s] Failed to open file", fileName);
#endif

    u8 *newFileData = NULL;

    if (fileData != NULL)
    {
        if (dataSize <= ((int) position * sizeof(int)))
        {
            newDataSize = (position + 1) * sizeof(int);
            newFileData = (u8 *) RL_REALLOC(fileData, newDataSize);

            if (newFileData != NULL)
            {
                // realloc succeeded
                int *dataPtr = (int *) newFileData;
                dataPtr[position] = value;
            }
            else
            {
                // realloc failed
                TraceLog(LOG_WARNING, "FILEIO: [%s] Failed to realloc data (%u), position in bytes (%u) bigger than actual file size", STORAGE_DATA_FILE, dataSize, position * sizeof(int));

                newFileData = fileData;
                newDataSize = dataSize;
            }
        }
        else
        {
            newFileData = fileData;
            newDataSize = dataSize;

            int *dataPtr = (int *)newFileData;
            dataPtr[position] = value;
        }

        success = SaveFileData(STORAGE_DATA_FILE, newFileData, newDataSize);
        RL_FREE(newFileData);
    }
    else
    {
        TraceLog(LOG_INFO, "FILEIO: [%s] File created successfully", STORAGE_DATA_FILE);

        dataSize = (position + 1) * sizeof(int);
        fileData = (u8 *) RL_MALLOC(dataSize);
        int *dataPtr = (int *)fileData;
        dataPtr[position] = value;

        success = SaveFileData(STORAGE_DATA_FILE, fileData, dataSize);
        RL_FREE(fileData);

        TraceLog(LOG_INFO, "FILEIO: [%s] Saved storage value: %i", STORAGE_DATA_FILE, value);
    }

    return success;
}

int LoadStorageValue(u32 position)
{
    int value = 0;
    int dataSize = 0;
    u8 *fileData = LoadFileData(STORAGE_DATA_FILE, &dataSize);

    if (fileData != NULL)
    {
        if (dataSize < ((int) position * 4)) TraceLog(LOG_WARNING, "FILEIO: [%s] Failed to find storage position: %i", STORAGE_DATA_FILE, position);
        else
        {
            int *dataPtr = (int *) fileData;
            value = dataPtr[position];
        }

        UnloadFileData(fileData);

        TraceLog(LOG_INFO, "FILEIO: [%s] Loaded storage value: %i", STORAGE_DATA_FILE, value);
    }

    return value;
}
