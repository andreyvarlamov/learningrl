#include <cstdio>

#include <raylib/raylib.h>
#include <raylib/raymath.h>

#include <varand/varand_types.h>
#include <varand/varand_util.h>
#include <varand/varand_raylibhelper.h>

#define GRAVITY 400
#define PLAYER_JUMP_SPD 350.0f
#define PLAYER_HOR_SPD 200.0f

#define MAX_ENVIRONMENT_ELEMENTS    5

typedef struct Player {
    Vector2 position;
    float speed;
    bool canJump;
} Player;

typedef struct EnvElement {
    Rectangle rect;
    int blocking;
    Color color;
} EnvElement;

int main(int argv, char **argc)
{
    int screenWidth = 800;
    int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Learning");

    Player player = { 0 };
    player.position = GetVector2(400, 280);
    player.speed = 0;
    player.canJump = false;

    EnvElement envElements[MAX_ENVIRONMENT_ELEMENTS] = {
        { GetRectangle(0, 0, 1000, 400), 0, LIGHTGRAY },
        { GetRectangle(0, 400, 1000, 200), 1, GRAY },
        { GetRectangle(300, 200, 400, 10), 1, GRAY },
        { GetRectangle(250, 300, 100, 10), 1, GRAY },
        { GetRectangle(650, 300, 100, 10), 1, GRAY }
    };

    Camera2D camera = { 0 };
    camera.target = player.position;
    camera.offset = GetVector2(screenWidth/2.0f, screenHeight/2.0f);
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    AutomationEventList aelist = LoadAutomationEventList(0);  // Initialize list of automation events to record new events
    SetAutomationEventList(&aelist);
    bool eventRecording = false;
    bool eventPlaying = false;

    unsigned int frameCounter = 0;
    unsigned int playFrameCounter = 0;
    unsigned int currentPlayEvent = 0;

    f32 frameRate = 60;
    SetTargetFPS((int) frameRate);

    while (!WindowShouldClose())
    {
        f32 deltaTime = 1 / frameRate;

        if (IsFileDropped())
        {
            FilePathList droppedFiles = LoadDroppedFiles();

            // Supports loading .rgs style files (text or binary) and .png style palette images
            if (IsFileExtension(droppedFiles.paths[0], ".txt;.rae"))
            {
                UnloadAutomationEventList(&aelist);
                aelist = LoadAutomationEventList(droppedFiles.paths[0]);
                
                eventRecording = false;
                
                // Reset scene state to play
                eventPlaying = true;
                playFrameCounter = 0;
                currentPlayEvent = 0;
                
                player.position = GetVector2(400, 280);
                player.speed = 0;
                player.canJump = false;

                camera.target = player.position;
                camera.offset = GetVector2(screenWidth/2.0f, screenHeight/2.0f);
                camera.rotation = 0.0f;
                camera.zoom = 1.0f;
            }

            UnloadDroppedFiles(droppedFiles);   // Unload filepaths from memory
        }

        if (IsKeyDown(KEY_LEFT)) player.position.x -= PLAYER_HOR_SPD*deltaTime;
        if (IsKeyDown(KEY_RIGHT)) player.position.x += PLAYER_HOR_SPD*deltaTime;
        if (IsKeyDown(KEY_SPACE) && player.canJump)
        {
            player.speed = -PLAYER_JUMP_SPD;
            player.canJump = false;
        }

        int hitObstacle = 0;
        for (int i = 0; i < MAX_ENVIRONMENT_ELEMENTS; i++)
        {
            EnvElement *element = &envElements[i];
            Vector2 *p = &(player.position);
            if (element->blocking &&
                element->rect.x <= p->x &&
                element->rect.x + element->rect.width >= p->x &&
                element->rect.y >= p->y &&
                element->rect.y <= p->y + player.speed*deltaTime)
            {
                hitObstacle = 1;
                player.speed = 0.0f;
                p->y = element->rect.y;
            }
        }

        if (!hitObstacle)
        {
            player.position.y += player.speed*deltaTime;
            player.speed += GRAVITY*deltaTime;
            player.canJump = false;
        }
        else player.canJump = true;

        camera.zoom += ((float)GetMouseWheelMove()*0.05f);

        if (camera.zoom > 3.0f) camera.zoom = 3.0f;
        else if (camera.zoom < 0.25f) camera.zoom = 0.25f;

        if (IsKeyPressed(KEY_R))
        {
            // Reset game state
            player.position = GetVector2(400, 280);
            player.speed = 0;
            player.canJump = false;

            camera.target = player.position;
            camera.offset = GetVector2(screenWidth/2.0f, screenHeight/2.0f);
            camera.rotation = 0.0f;
            camera.zoom = 1.0f;
        }

        camera.target = player.position;
        camera.offset = GetVector2(screenWidth/2.0f, screenHeight/2.0f);
        float minX = 1000, minY = 1000, maxX = -1000, maxY = -1000;

        for (int i = 0; i < MAX_ENVIRONMENT_ELEMENTS; i++)
        {
            EnvElement *element = &envElements[i];
            minX = fminf(element->rect.x, minX);
            maxX = fmaxf(element->rect.x + element->rect.width, maxX);
            minY = fminf(element->rect.y, minY);
            maxY = fmaxf(element->rect.y + element->rect.height, maxY);
        }

        Vector2 max = GetWorldToScreen2D(GetVector2(maxX, maxY), camera);
        Vector2 min = GetWorldToScreen2D(GetVector2(minX, minY), camera);

        if (max.x < screenWidth) camera.offset.x = screenWidth - (max.x - screenWidth/2);
        if (max.y < screenHeight) camera.offset.y = screenHeight - (max.y - screenHeight/2);
        if (min.x > 0) camera.offset.x = screenWidth/2 - min.x;
        if (min.y > 0) camera.offset.y = screenHeight/2 - min.y;

        if (IsKeyPressed(KEY_S))
        {
            if (!eventPlaying)
            {
                if (eventRecording)
                {
                    StopAutomationEventRecording();
                    eventRecording = false;

                    ExportAutomationEventList(aelist, "automation.rae");

                    TraceLog(LOG_INFO, "RECORDED FRAMES: %i", aelist.count);
                }
                else
                {
                    SetAutomationEventBaseFrame(180);
                    StartAutomationEventRecording();
                    eventRecording = true;
                }
            }
        }
        else if (IsKeyPressed(KEY_A))
        {
            if (!eventRecording && (aelist.count > 0))
            {
                eventPlaying = true;
                playFrameCounter = 0;
                currentPlayEvent = 0;

                player.position = GetVector2(400, 280);
                player.speed = 0;
                player.canJump = false;

                camera.target = player.position;
                camera.offset = GetVector2(screenWidth / 2.0f, screenHeight / 2.0f);
                camera.rotation = 0.0f;
                camera.zoom = 1.0f;
            }
        }

        if (eventPlaying)
        {
            while (playFrameCounter == aelist.events[currentPlayEvent].frame)
            {
                TraceLog(LOG_INFO, "PLAYING: PlayFrameCount: %i | currentPlayEvent: %i | Event Frame: %i, param: %i",
                         playFrameCounter, currentPlayEvent, aelist.events[currentPlayEvent].frame, aelist.events[currentPlayEvent].params[0]);

                PlayAutomationEvent(aelist.events[currentPlayEvent]);
                currentPlayEvent++;

                if (currentPlayEvent == aelist.count)
                {
                    eventPlaying = false;
                    currentPlayEvent = 0;
                    playFrameCounter = 0;

                    TraceLog(LOG_INFO, "FINISH PLAYING!");
                    break;
                }
            }

            playFrameCounter++;
        }

        if (eventRecording || eventPlaying) frameCounter++;
        else frameCounter = 0;

        BeginDrawing();

            ClearBackground(LIGHTGRAY);

            BeginMode2D(camera);

                // Draw environment elements
                for (int i = 0; i < MAX_ENVIRONMENT_ELEMENTS; i++)
                {
                    DrawRectangleRec(envElements[i].rect, envElements[i].color);
                }

                // Draw player rectangle
                DrawRectangleRec(GetRectangle(player.position.x - 20, player.position.y - 40, 40, 40), RED);

            EndMode2D();
            
            // Draw game controls
            DrawRectangle(10, 10, 290, 145, Fade(SKYBLUE, 0.5f));
            DrawRectangleLines(10, 10, 290, 145, Fade(BLUE, 0.8f));

            DrawText("Controls:", 20, 20, 10, BLACK);
            DrawText("- RIGHT | LEFT: Player movement", 30, 40, 10, DARKGRAY);
            DrawText("- SPACE: Player jump", 30, 60, 10, DARKGRAY);
            DrawText("- R: Reset game state", 30, 80, 10, DARKGRAY);

            DrawText("- S: START/STOP RECORDING INPUT EVENTS", 30, 110, 10, BLACK);
            DrawText("- A: REPLAY LAST RECORDED INPUT EVENTS", 30, 130, 10, BLACK);

            // Draw automation events recording indicator
            if (eventRecording)
            {
                DrawRectangle(10, 160, 290, 30, Fade(RED, 0.3f));
                DrawRectangleLines(10, 160, 290, 30, Fade(MAROON, 0.8f));
                DrawCircle(30, 175, 10, MAROON);

                if (((frameCounter/15)%2) == 1) DrawText(TextFormat("RECORDING EVENTS... [%i]", aelist.count), 50, 170, 10, MAROON);
            }
            else if (eventPlaying)
            {
                DrawRectangle(10, 160, 290, 30, Fade(LIME, 0.3f));
                DrawRectangleLines(10, 160, 290, 30, Fade(DARKGREEN, 0.8f));
                DrawTriangle(GetVector2(20, 155 + 10), GetVector2(20, 155 + 30), GetVector2(40, 155 + 20), DARKGREEN);

                if (((frameCounter/15)%2) == 1) DrawText(TextFormat("PLAYING RECORDED EVENTS... [%i]", currentPlayEvent), 50, 170, 10, DARKGREEN);
            }
            

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
