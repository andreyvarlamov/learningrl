#include <cstdio>

#include <raylib/raylib.h>
#include <raylib/raymath.h>

#include <varand/varand_types.h>
#include <varand/varand_util.h>
#include <varand/varand_raylibhelper.h>

#define G 400
#define PLAYER_JUMP_SPD 350.0f
#define PLAYER_HOR_SPD 200.0f

struct Player
{
    Vector2 position;
    f32 speed;
    b32 canJump;
};

struct EnvItem
{
    Rectangle rect;
    int blocking;
    Color color;
};

void UpdatePlayer(Player *player, EnvItem *envItems, int envItemCount, f32 delta);
void UpdateCameraCenter(Camera2D *camera, Player *player, EnvItem *envItems, int envItemCount, f32 delta, i32 width, i32 height);
void UpdateCameraCenterInsideMap(Camera2D *camera, Player *player, EnvItem *envItems, int envItemCount, f32 delta, i32 width, i32 height);
void UpdateCameraCenterSmoothFollow(Camera2D *camera, Player *player, EnvItem *envItems, int envItemCount, f32 delta, i32 width, i32 height);
void UpdateCameraEvenOutOnLanding(Camera2D *camera, Player *player, EnvItem *envItems, int envItemCount, f32 delta, i32 width, i32 height);
void UpdateCameraPlayerBoundsPush(Camera2D *camera, Player *player, EnvItem *envItems, int envItemCount, f32 delta, i32 width, i32 height);

int main(int argv, char **argc)
{
    int screenWidth = 800;
    int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Learning");

    Player player = {};
    player.position = GetVector2(400, 280);
    player.speed = 0;
    player.canJump = false;
    EnvItem envItems[] = {
        { GetRectangle(0, 0, 1000, 400), 0, LIGHTGRAY },
        { GetRectangle(0, 400, 1000, 200), 1, GRAY },
        { GetRectangle(300, 200, 400, 10), 1, GRAY },
        { GetRectangle(250, 300, 100, 10), 1, GRAY },
        { GetRectangle(650, 300, 100, 10), 1, GRAY }
    };
    int envItemCount = ArrayCount(envItems);

    Camera2D camera = {};
    camera.target = player.position;
    camera.offset = GetVector2(screenWidth / 2.0f, screenHeight / 2.0f);
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    void (*cameraUpdaters[])(Camera2D *, Player *, EnvItem *, int, f32, int, int) = {
        UpdateCameraCenter,
        UpdateCameraCenterInsideMap,
        UpdateCameraCenterSmoothFollow,
        UpdateCameraEvenOutOnLanding,
        UpdateCameraPlayerBoundsPush
    };
    int cameraUpdatersCount = ArrayCount(cameraUpdaters);

    int cameraOption = 0;

    const char *cameraDescriptions[] = {
        "Follow player center",
        "Follow player center, but clamp to map edges",
        "Follow player center; smoothed",
        "Follow player center horizontally; update player center vertically after landing",
        "Player push camera on getting too close to screen edge"
    };

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        f32 deltaTime = GetFrameTime();

        UpdatePlayer(&player, envItems, envItemCount, deltaTime);

        camera.zoom += ((f32) GetMouseWheelMove() * 0.05f);
        if (camera.zoom > 3.0f) camera.zoom = 3.0f;
        else if (camera.zoom < 0.25f) camera.zoom = 0.25f;

        if (IsKeyPressed(KEY_R))
        {
            camera.zoom = 1.0f;
            player.position = GetVector2(400, 280);
        }

        if (IsKeyPressed(KEY_C)) cameraOption = (cameraOption + 1) % cameraUpdatersCount;

        cameraUpdaters[cameraOption](&camera, &player, envItems, envItemCount, deltaTime, screenWidth, screenHeight);

        BeginDrawing();
            ClearBackground(LIGHTGRAY);

            BeginMode2D(camera);

                for (int i = 0; i < envItemCount; i++) DrawRectangleRec(envItems[i].rect, envItems[i].color);

                Rectangle playerRect = GetRectangle(player.position.x - 20, player.position.y - 40, 40, 40);
                DrawRectangleRec(playerRect, RED);

                DrawCircle((int) player.position.x, (int) player.position.y, 5, GOLD);

            EndMode2D();

            DrawText("Controls:", 20, 20, 10, BLACK);
            DrawText("- Right/Left to move", 40, 40, 10, DARKGRAY);
            DrawText("- Space to jump", 40, 60, 10, DARKGRAY);
            DrawText("- Mouse Wheel to Zoom in-out, R to reset zoom", 40, 80, 10, DARKGRAY);
            DrawText("- C to change camera mode", 40, 100, 10, DARKGRAY);
            DrawText("Current camera mode:", 20, 120, 10, BLACK);
            DrawText(cameraDescriptions[cameraOption], 40, 140, 10, DARKGRAY);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}

void UpdatePlayer(Player *player, EnvItem *envItems, int envItemCount, f32 delta)
{
    if (IsKeyDown(KEY_LEFT)) player->position.x -= PLAYER_HOR_SPD * delta;
    if (IsKeyDown(KEY_RIGHT)) player->position.x += PLAYER_HOR_SPD * delta;
    if (IsKeyDown(KEY_SPACE) && player->canJump)
    {
        player->speed = -PLAYER_JUMP_SPD;
        player->canJump = false;
    }

    b32 hitObstacle = false;

    for (int i = 0; i < envItemCount; i++)
    {
        EnvItem *ei = envItems + i;
        Vector2 *p = &player->position;
        if (ei->blocking &&
            ei->rect.x <= p->x &&
            ei->rect.x + ei->rect.width >= p->x &&
            ei->rect.y >= p->y &&
            ei->rect.y <= p->y + player->speed * delta)
        {
            hitObstacle = true;
            player->speed = 0.0f;
            p->y = ei->rect.y;
            break;
        }
    }

    if (!hitObstacle)
    {
        player->position.y += player->speed * delta;
        player->speed += G * delta;
        player->canJump = false;
    }
    else player->canJump = true;
}


void UpdateCameraCenter(Camera2D *camera, Player *player, EnvItem *envItems, int envItemCount, f32 delta, i32 width, i32 height)
{
    camera->offset = GetVector2(width / 2.0f, height / 2.0f);
    camera->target = player->position;
}

void UpdateCameraCenterInsideMap(Camera2D *camera, Player *player, EnvItem *envItems, int envItemCount, f32 delta, i32 width, i32 height)
{
    camera->offset = GetVector2(width / 2.0f, height / 2.0f);
    camera->target = player->position;
    f32 minX = 1000, minY = 1000, maxX = -1000, maxY = -1000;

    for (int i = 0; i < envItemCount; i++)
    {
        EnvItem *ei = envItems + i;
        minX = fminf(ei->rect.x, minX);
        maxX = fmaxf(ei->rect.x + ei->rect.width, maxX);
        minY = fminf(ei->rect.y, minY);
        maxY = fmaxf(ei->rect.y + ei->rect.height, maxY);
    }

    Vector2 max = GetWorldToScreen2D(GetVector2(maxX, maxY), *camera);
    Vector2 min = GetWorldToScreen2D(GetVector2(minX, minY), *camera);

    if (max.x < width)
    {
        camera->offset.x = width - (max.x - width / 2);
    }
    if (max.y > height)
    {
        camera->offset.y = height - (max.y - height / 2);
    }
    if (min.x > 0)
    {
        camera->offset.x = width / 2 - min.x;
    }
    if (min.y < 0)
    {
        camera->offset.y = height / 2 - min.y;
    }
}

void UpdateCameraCenterSmoothFollow(Camera2D *camera, Player *player, EnvItem *envItems, int envItemCount, f32 delta, i32 width, i32 height)
{
    local_persist f32 minSpeed = 30;
    local_persist f32 minEffectLength = 10;
    local_persist f32 fractionSpeed = 0.8f;

    camera->offset = GetVector2(width / 2.0f, height / 2.0f);
    Vector2 diff = Vector2Subtract(player->position, camera->target);
    f32 length = Vector2Length(diff);

    if (length > minEffectLength)
    {
        f32 speed = fmaxf(fractionSpeed * length, minSpeed);
        camera->target = Vector2Add(camera->target, Vector2Scale(diff, speed * delta / length));
    }
}

void UpdateCameraEvenOutOnLanding(Camera2D *camera, Player *player, EnvItem *envItems, int envItemCount, f32 delta, i32 width, i32 height)
{
    local_persist f32 evenOutSpeed = 700;
    local_persist b32 eveningOut = false;
    local_persist f32 evenOutTarget;

    camera->offset = GetVector2(width / 2.0f, height / 2.0f);
    camera->target.x = player->position.x;

    if (eveningOut)
    {
        if (evenOutTarget > camera->target.y)
        {
            camera->target.y += evenOutSpeed * delta;

            if (camera->target.y > evenOutTarget)
            {
                camera->target.y = evenOutTarget;
                eveningOut = false;
            }
        }
        else
        {
            camera->target.y -= evenOutSpeed * delta;

            if (camera->target.y < evenOutTarget)
            {
                camera->target.y = evenOutTarget;
                eveningOut = false;
            }
        }
    }
    else
    {
        if (player->canJump && (player->speed == 0) && (player->position.y != camera->target.y))
        {
            eveningOut = true;
            evenOutTarget = player->position.y;
        }
    }
}

void UpdateCameraPlayerBoundsPush(Camera2D *camera, Player *player, EnvItem *envItems, int envItemCount, f32 delta, i32 width, i32 height)
{
    local_persist Vector2 bbox = GetVector2(0.2f);

    Vector2 bboxWorldMin = GetScreenToWorld2D(GetVector2((1 - bbox.x) * 0.5f * width, (1 - bbox.y) * 0.5f * height), *camera);
    Vector2 bboxWorldMax = GetScreenToWorld2D(GetVector2((1 + bbox.x) * 0.5f * width, (1 + bbox.y) * 0.5f * height), *camera);
    camera->offset = GetVector2((1 - bbox.x) * 0.5f * width , (1 - bbox.y) * 0.5f * height);

    if (player->position.x < bboxWorldMin.x) camera->target.x = player->position.x;
    if (player->position.y < bboxWorldMin.y) camera->target.y = player->position.y;
    if (player->position.x > bboxWorldMax.x) camera->target.x = bboxWorldMin.x + (player->position.x - bboxWorldMax.x);
    if (player->position.y > bboxWorldMax.y) camera->target.y = bboxWorldMin.y + (player->position.y - bboxWorldMax.y);
}

