#include <varand/varand_types.h>
#include <varand/varand_util.h>
#include <varand/varand_raylibhelper.h>

#include <raylib/raylib.h>

#include <cstdio>
#include <cstdlib>
#include <atomic>
#include <ctime>
#include <thread>

global_variable std::atomic_bool gDataLoaded = false;
global_variable std::atomic_int gDataProgress = 0;

internal void LoadDataThread();

int main(int argv, char **argc)
{
    int screenWidth = 800;
    int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Learning");

    std::thread loadDataThread;

    enum { STATE_WAITING, STATE_LOADING, STATE_FINISHED } state = STATE_WAITING;
    int framesCounter = 0;

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        switch (state)
        {
            case STATE_WAITING:
            {
                if (IsKeyPressed(KEY_ENTER))
                {
                    loadDataThread = std::thread(LoadDataThread);
                    TraceLog(LOG_INFO, "Loading thread initialized successfully");

                    state = STATE_LOADING;
                }
            } break;

            case STATE_LOADING:
            {
                framesCounter++;
                if (std::atomic_load_explicit(&gDataLoaded, std::memory_order_relaxed))
                {
                    framesCounter = 0;
                    loadDataThread.join();
                    TraceLog(LOG_INFO, "Loading thread terminated successfully");

                    state = STATE_FINISHED;
                }
            } break;

            case STATE_FINISHED:
            {
                if (IsKeyPressed(KEY_ENTER))
                {
                    std::atomic_store_explicit(&gDataLoaded, false, std::memory_order_relaxed);
                    std::atomic_store_explicit(&gDataProgress, 0, std::memory_order_relaxed);

                    state = STATE_WAITING;
                }
            } break;

            default: break;
        }

        BeginDrawing();
            ClearBackground(RAYWHITE);

            switch (state)
            {
                case STATE_WAITING: DrawText("PRESS ENTER to START LOADING DATA", 150, 170, 20, DARKGRAY); break;

                case STATE_LOADING:
                {
                    DrawRectangle(150, 200, std::atomic_load_explicit(&gDataProgress, std::memory_order_relaxed), 60, SKYBLUE);
                    if ((framesCounter/15)%2) DrawText("LOADING DATA...", 240, 210, 40, DARKBLUE);
                } break;

                case STATE_FINISHED:
                {
                    DrawRectangle(150, 200, 500, 60, LIME);
                    DrawText("DATA LOADED!", 250, 210, 40, GREEN);
                } break;

                default: break;
            }

            DrawRectangleLines(150, 200, 500, 60, DARKGRAY);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}

internal void LoadDataThread()
{
    int timeCounter = 0;
    clock_t prevTime = clock();

    while (timeCounter < 5000)
    {
        clock_t currentTime = clock() - prevTime;
        timeCounter = currentTime*1000/CLOCKS_PER_SEC;

        std::atomic_store_explicit(&gDataProgress, timeCounter/10, std::memory_order_relaxed);
    }

    std::atomic_store_explicit(&gDataLoaded, true, std::memory_order_relaxed);
}
