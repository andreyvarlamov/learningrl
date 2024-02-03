#include <varand/varand_types.h>
#include <varand/varand_util.h>
#include <varand/varand_raylibhelper.h>

#include <raylib/raylib.h>
#include <raylib/raymath.h>
#include <raylib/rlgl.h>

// Custom blend modes
#define RLGL_SRC_ALPHA 0x0302
#define RLGL_MIN 0x8007
#define RLGL_MAX 0x8008

#define MAX_BOXES 20
#define MAX_SHADOWS MAX_BOXES * 3
#define MAX_LIGHTS 16

struct ShadowGeometry
{
    Vector2 vertices[4];
};

struct LightInfo
{
    bool active; // Is this light slot active?
    bool dirty; // Does this light need to be updated?
    bool valid; // Is this light in a valid position?

    Vector2 position; // Light position
    RenderTexture mask; // ALpha mask for the light
    float outerRadius; // The destance the light touches
    Rectangle bounds; // A cached rectangle of the light bounds to help with culling

    ShadowGeometry shadows[MAX_SHADOWS];
    int shadowCount;
};

LightInfo lights[MAX_LIGHTS] = {};

// Move a light and mark it as dirty so that we update its mask next frame
void MoveLight(int slot, float x, float y)
{
    lights[slot].dirty = true;
    lights[slot].position.x = x;
    lights[slot].position.y = y;

    lights[slot].bounds.x = x - lights[slot].outerRadius;
    lights[slot].bounds.y = y - lights[slot].outerRadius;
}


// Compute a shadow volume for the edge
// it takes the edge and projects it back by the light radius and turns it into a quad
void ComputeShadowVolumeForEdge(int slot, Vector2 sp, Vector2 ep)
{
    if (lights[slot].shadowCount >= MAX_SHADOWS) return;

    float extension = lights[slot].outerRadius * 2;

    Vector2 spVector = Vector2Normalize(Vector2Subtract(sp, lights[slot].position));
    Vector2 spProjection = Vector2Add(sp, Vector2Scale(spVector, extension));

    Vector2 epVector = Vector2Normalize(Vector2Subtract(ep, lights[slot].position));
    Vector2 epProjection = Vector2Add(ep, Vector2Scale(epVector, extension));

    lights[slot].shadows[lights[slot].shadowCount].vertices[0] = sp;
    lights[slot].shadows[lights[slot].shadowCount].vertices[1] = ep;
    lights[slot].shadows[lights[slot].shadowCount].vertices[2] = epProjection;
    lights[slot].shadows[lights[slot].shadowCount].vertices[3] = spProjection;

    lights[slot].shadowCount++;
}

void DrawLightMask(int slot)
{
    BeginTextureMode(lights[slot].mask);
        ClearBackground(WHITE);

        // Force the blend mode to only set the alpha of the destination
        rlSetBlendFactors(RLGL_SRC_ALPHA, RLGL_SRC_ALPHA, RLGL_MIN);
        rlSetBlendMode(BLEND_CUSTOM);

        if (lights[slot].valid) DrawCircleGradient((int) lights[slot].position.x, (int) lights[slot].position.y, lights[slot].outerRadius, ColorAlpha(WHITE, 0), WHITE);

        rlDrawRenderBatchActive();

        rlSetBlendMode(BLEND_ALPHA);
        rlSetBlendFactors(RLGL_SRC_ALPHA, RLGL_SRC_ALPHA, RLGL_MAX);
        rlSetBlendMode(BLEND_CUSTOM);

        // Draw the shadows to the alpha mask
        for (int i = 0; i < lights[slot].shadowCount; i++)
        {
            DrawTriangleFan(lights[slot].shadows[i].vertices, 4, WHITE);
        }

        rlDrawRenderBatchActive();

        // Go back to normal blend mode
        rlSetBlendMode(BLEND_ALPHA);

    EndTextureMode();
}

// Setup a light
void SetupLight(int slot, float x, float y, float radius)
{
    lights[slot].active = true;
    lights[slot].valid = false;
    lights[slot].mask = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
    lights[slot].outerRadius = radius;

    lights[slot].bounds.width = radius * 2;
    lights[slot].bounds.height = radius * 2;

    MoveLight(slot, x, y);

    // Force the render texture to have something in it
    DrawLightMask(slot);
}

// See if a light needs to update its mask
bool UpdateLight(int slot, Rectangle *boxes, int count)
{
    if (!lights[slot].active || !lights[slot].dirty) return false;

    lights[slot].dirty = false;
    lights[slot].shadowCount = 0;
    lights[slot].valid = false;

    for (int i = 0; i < count; i++)
    {
        // Is the light in a box? if so it's not valid
        if (CheckCollisionPointRec(lights[slot].position, boxes[i])) return false;

        // If this box is outside bounds, can skip it
        if (!CheckCollisionRecs(lights[slot].bounds, boxes[i])) continue;

        // Check the edges that are on the same side the light is, cast shadow volumes from them

        // Top
        Vector2 sp = GetVector2(boxes[i].x, boxes[i].y);
        Vector2 ep = GetVector2(boxes[i].x + boxes[i].width, boxes[i].y);

        if (lights[slot].position.y > ep.y) ComputeShadowVolumeForEdge(slot, sp, ep);

        // Right
        sp = ep;
        ep.y += boxes[i].height;
        if (lights[slot].position.x < ep.x) ComputeShadowVolumeForEdge(slot, sp, ep);

        // Bottom
        sp = ep;
        ep.x -= boxes[i].width;
        if (lights[slot].position.y < ep.y) ComputeShadowVolumeForEdge(slot, sp, ep);

        // Left
        sp = ep;
        ep.y -= boxes[i].height;
        if (lights[slot].position.x > ep.x) ComputeShadowVolumeForEdge(slot, sp, ep);

        // The box itself
        lights[slot].shadows[lights[slot].shadowCount].vertices[0] = GetVector2(boxes[i].x, boxes[i].y);
        lights[slot].shadows[lights[slot].shadowCount].vertices[1] = GetVector2(boxes[i].x, boxes[i].y + boxes[i].height);
        lights[slot].shadows[lights[slot].shadowCount].vertices[2] = GetVector2(boxes[i].x + boxes[i].width, boxes[i].y + boxes[i].height);
        lights[slot].shadows[lights[slot].shadowCount].vertices[3] = GetVector2(boxes[i].x + boxes[i].width, boxes[i].y);
        lights[slot].shadowCount++;
    }

    lights[slot].valid = true;

    DrawLightMask(slot);

    return true;
}

// Set up some boxes
void SetupBoxes(Rectangle *boxes, int *count)
{
    boxes[0] = GetRectangle(150,80, 40, 40);
    boxes[1] = GetRectangle(1200, 700, 40, 40);
    boxes[2] = GetRectangle(200, 600, 40, 40);
    boxes[3] = GetRectangle(1000, 50, 40, 40);
    boxes[4] = GetRectangle(500, 350, 40, 40);

    for (int i = 5; i < MAX_BOXES; i++)
    {
        boxes[i] = GetRectangle((float)GetRandomValue(0,GetScreenWidth()), (float)GetRandomValue(0,GetScreenHeight()), (float)GetRandomValue(10,100), (float)GetRandomValue(10,100));
    }

    *count = MAX_BOXES;
}

int main(int argv, char **argc)
{
    int screenWidth = 800;
    int screenHeight = 600;


    // Set configuration flags for window creation
    InitWindow(screenWidth, screenHeight, "Learning");

    // Initialize our 'world' of boxes
    int boxCount = 0;
    Rectangle boxes[MAX_BOXES] = { 0 };
    SetupBoxes(boxes, &boxCount);

    // Create a checkerboard groudn texture
    Image img = GenImageChecked(64, 64, 32, 32, DARKBROWN, DARKGRAY);
    Texture2D backgroundTexture = LoadTextureFromImage(img);
    UnloadImage(img);

    // Create a global light mask to hold all the blended lights
    RenderTexture lightMask = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());

    // Setup initial light
    SetupLight(0, 600, 400, 300);
    int nextLight = 1;

    bool showLines = false;

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        // Drag light 0
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) MoveLight(0, GetMousePosition().x, GetMousePosition().y);

        // Make a new light
        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && (nextLight < MAX_LIGHTS))
        {
            SetupLight(nextLight, GetMousePosition().x, GetMousePosition().y, 200);
            nextLight++;
        }

        // Toggle debug info
        if (IsKeyPressed(KEY_F1)) showLines = !showLines;

        // Update the lights and keep track if any were dirty so we know if we need to update the master light mask
        bool dirtyLights = false;
        for (int i = 0; i < MAX_LIGHTS; i++)
        {
            if (UpdateLight(i, boxes, boxCount)) dirtyLights = true;
        }

        // Update the light mask
        if (dirtyLights)
        {
            // Build up the light mask
            BeginTextureMode(lightMask);
            
                ClearBackground(BLACK);

                // Force the blend mode to only set the alpha of the destination
                rlSetBlendFactors(RLGL_SRC_ALPHA, RLGL_SRC_ALPHA, RLGL_MIN);
                rlSetBlendMode(BLEND_CUSTOM);

                // Merge in all the light masks
                for (int i = 0; i < MAX_LIGHTS; i++)
                {
                    if (lights[i].active) DrawTextureRec(lights[i].mask.texture, GetRectangle(0, 0, (float)GetScreenWidth(), -(float)GetScreenHeight()), Vector2Zero(), WHITE);
                }

                rlDrawRenderBatchActive();

                // Go back to normal blend
                rlSetBlendMode(BLEND_ALPHA);
            EndTextureMode();
        }

        BeginDrawing();

            ClearBackground(BLACK);
            
            // Draw the tile background
            DrawTextureRec(backgroundTexture, GetRectangle(0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()), Vector2Zero(), WHITE);
            
            // Overlay the shadows from all the lights
            DrawTextureRec(lightMask.texture, GetRectangle(0, 0, (float)GetScreenWidth(), -(float)GetScreenHeight()), Vector2Zero(), ColorAlpha(WHITE, showLines? 0.75f : 1.0f));

            // Draw the lights
            for (int i = 0; i < MAX_LIGHTS; i++)
            {
                if (lights[i].active) DrawCircle((int)lights[i].position.x, (int)lights[i].position.y, 10, (i == 0)? YELLOW : WHITE);
            }

            if (showLines)
            {
                for (int s = 0; s < lights[0].shadowCount; s++)
                {
                    DrawTriangleFan(lights[0].shadows[s].vertices, 4, DARKPURPLE);
                }

                for (int b = 0; b < boxCount; b++)
                {
                    if (CheckCollisionRecs(boxes[b],lights[0].bounds)) DrawRectangleRec(boxes[b], PURPLE);

                    DrawRectangleLines((int)boxes[b].x, (int)boxes[b].y, (int)boxes[b].width, (int)boxes[b].height, DARKBLUE);
                }

                DrawText("(F1) Hide Shadow Volumes", 10, 50, 10, GREEN);
            }
            else
            {
                DrawText("(F1) Show Shadow Volumes", 10, 50, 10, GREEN);
            }

            DrawFPS(screenWidth - 80, 10);
            DrawText("Drag to move light #1", 10, 10, 10, DARKGREEN);
            DrawText("Right click to add new light", 10, 30, 10, DARKGREEN);

        EndDrawing();
    }

    UnloadTexture(backgroundTexture);
    UnloadRenderTexture(lightMask);
    for (int i = 0; i < MAX_LIGHTS; i++)
    {
        if (lights[i].active) UnloadRenderTexture(lights[i].mask);
    }

    CloseWindow();

    return 0;
}
