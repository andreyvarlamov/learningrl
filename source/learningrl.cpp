#include <varand/varand_types.h>
#include <varand/varand_util.h>
#include <varand/varand_raylibhelper.h>

#include <raylib/raylib.h>

#define RAYGUI_IMPLEMENTATION
#include <raylib/raygui.h>

//#include <stdlib.h>

#define MAX_SPLINE_POINTS 32

struct ControlPoint
{
    Vector2 start;
    Vector2 end;
};

enum SplineType
{
    SPLINE_LINEAR  = 0,
    SPLINE_BASIS,
    SPLINE_CATMULLROM,
    SPLINE_BEZIER
};

#define DRAW_INDIVIDUAL 0

int main(int argv, char **argc)
{
    int screenWidth = 800;
    int screenHeight = 600;

    // Set configuration flags for window creation
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "Learning");

    Vector2 points[MAX_SPLINE_POINTS] = {
        GetVector2(50.0f, 400.0f),
        GetVector2(160.0f, 220.0f),
        GetVector2(340.0f, 380.0f),
        GetVector2(520.0f, 60.0f),
        GetVector2(710.0f, 260.0f),
    };

    int pointCount = 5;
    int selectedPoint = -1;
    int focusedPoint = -1;
    Vector2 *selectedControlPoint = NULL;
    Vector2 *focusedControlPoint = NULL;
    
    // Cube Bezier control points need additional state
    ControlPoint control[MAX_SPLINE_POINTS] = {};
    for (int i = 0; i < pointCount - 1; i++)
    {
        control[i].start = GetVector2(points[i].x + 50, points[i].y);
        control[i].end = GetVector2(points[i + 1].x - 50, points[i + 1].y);
    }

    f32 splineThickness = 8.0f;
    int splineTypeActive = SPLINE_LINEAR;
    bool splineTypeEditMode = false;
    bool splineHelpersActive = true;

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON) && (pointCount < MAX_SPLINE_POINTS))
        {
            points[pointCount] = GetMousePosition();
            pointCount++;
        }

        for (int i = 0; i < pointCount; i++)
        {
            if (CheckCollisionPointCircle(GetMousePosition(), points[i], 8.0f))
            {
                focusedPoint = i;
                if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) selectedPoint = i;
                break;
            }
            else focusedPoint = -1;
        }

        if (selectedPoint >= 0)
        {
            points[selectedPoint] = GetMousePosition();
            if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) selectedPoint = -1;
        }

        if ((splineTypeActive == SPLINE_BEZIER) && (focusedPoint == -1))
        {
            for (int i = 0; i < pointCount; i++)
            {
                if (CheckCollisionPointCircle(GetMousePosition(), control[i].start, 6.0f))
                {
                    focusedControlPoint = &control[i].start;
                    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) selectedControlPoint = &control[i].start;
                    break;
                }
                else if (CheckCollisionPointCircle(GetMousePosition(), control[i].end, 6.0f))
                {
                    focusedControlPoint = &control[i].end;
                    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) selectedControlPoint = &control[i].end;
                    break;
                }
                else
                {
                    focusedControlPoint = NULL;
                }
            }

            if (selectedControlPoint != NULL)
            {
                *selectedControlPoint = GetMousePosition();
                if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) selectedControlPoint = NULL;
            }
        }

        if (IsKeyPressed(KEY_ONE)) splineTypeActive = 0;
        else if (IsKeyPressed(KEY_TWO)) splineTypeActive = 1;
        else if (IsKeyPressed(KEY_THREE)) splineTypeActive = 2;
        else if (IsKeyPressed(KEY_FOUR)) splineTypeActive = 3;

        BeginDrawing();
            ClearBackground(RAYWHITE);

            if (splineTypeActive == SPLINE_LINEAR)
            {
                DrawSplineLinear(points, pointCount, splineThickness, RED);
            }
            else if (splineTypeActive == SPLINE_BASIS)
            {
#if !(DRAW_INDIVIDUAL)
                DrawSplineBasis(points, pointCount, splineThickness, RED);
#else
                for (int i = 0; i < (pointCount - 3); i++)
                {
                    // Drawing individual segments, not considering thickness connection compensation
                    DrawSplineSegmentBasis(points[i], points[i + 1], points[i + 2], points[i + 3], splineThickness, MAROON);
                }
#endif
            }
            else if (splineTypeActive == SPLINE_CATMULLROM)
            {
#if !(DRAW_INDIVIDUAL)
                DrawSplineCatmullRom(points, pointCount, splineThickness, RED);
#else
                for (int i = 0; i < (pointCount - 3); i++)
                {
                    // Drawing individual segments, not considering thickness connection compensation
                    DrawSplineSegmentCatmullRom(points[i], points[i + 1], points[i + 2], points[i + 3], splineThickness, MAROON);
                }   
#endif
            }
            else if (splineTypeActive == SPLINE_BEZIER)
            {
                for (int i = 0; i < pointCount - 1; i++)
                {
                    DrawSplineSegmentBezierCubic(points[i], control[i].start, control[i].end, points[i + 1], splineThickness, RED);

                    // Control points
                    DrawCircleV(control[i].start, 6, GOLD);
                    DrawCircleV(control[i].end, 6, GOLD);
                    if (focusedControlPoint == &control[i].start) DrawCircleV(control[i].start, 8, GREEN);
                    else if (focusedControlPoint == &control[i].end) DrawCircleV(control[i].end, 8, GREEN);
                    DrawLineEx(points[i], control[i].start, 1.0f, LIGHTGRAY);
                    DrawLineEx(points[i + 1], control[i].end, 1.0f, LIGHTGRAY);

                    DrawLineV(points[i], control[i].start, GRAY);
                    DrawLineV(control[i].end, points[i + 1], GRAY);
                }
            }

            if (splineHelpersActive)
            {
                for (int i = 0; i < pointCount; i++)
                {
                    DrawCircleLinesV(points[i], (focusedPoint == i) ? 12.0f : 8.0f, (focusedPoint == i) ? BLUE : DARKBLUE);
                    if ((splineTypeActive != SPLINE_LINEAR) &&
                        (splineTypeActive != SPLINE_BEZIER) &&
                        (i < pointCount - 1)) DrawLineV(points[i], points[i + 1], GRAY);

                    DrawText(TextFormat("[%.0f, %.0f]", points[i].x, points[i].y), (int) points[i].x, (int) points[i].y + 10, 10, BLACK);
                }
            }

            if (splineTypeEditMode) GuiLock();

            GuiLabel(GetRectangle(12, 62, 140, 24), TextFormat("Spline thickness: %i", (int)splineThickness));
            GuiSliderBar(GetRectangle(12, 60 + 24, 140, 16), NULL, NULL, &splineThickness, 1.0f, 40.0f);

            GuiCheckBox(GetRectangle(12, 110, 20, 20), "Show point helpers", &splineHelpersActive);

            GuiUnlock();

            GuiLabel(GetRectangle(12, 10, 140, 24), "Spline type:");
            if (GuiDropdownBox(GetRectangle(12, 8 + 24, 140, 28), "LINEAR;BSPLINE;CATMULLROM;BEZIER", &splineTypeActive, splineTypeEditMode)) splineTypeEditMode = !splineTypeEditMode;
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
