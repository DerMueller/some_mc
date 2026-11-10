#include "raylib.h"
#include <math.h>

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"                 // Required for GUI controls

#define DEPTH_MAX 14.0f

#define BRANCHES_MAX 16390 // 16384 = 2^14   +6
// #define BRANCHES_MAX 4102 // 4096 = 2^12   +6



//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
typedef struct {
    Vector2 start;
    Vector2 end;
    float angle;
    float length;
} Branch;

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1280;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "raylib [shapes] example - recursive tree");

    Vector2 start = { (screenWidth/2.0f) - 125.0f, (float)screenHeight };
    float angle = 40.0f;
    float thick = 1.0f;
    float treeDepth = 10.0f;
    float branchDecay = 0.66f;
    float length = 120.0f;
    bool bezier = false;

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        angle = (float) ((int)angle); // floor it

        treeDepth = (float) ((int)treeDepth);

        float theta = angle*DEG2RAD;
        int maxBranches = (int)(powf(2, floorf(treeDepth)));
        Branch branches[BRANCHES_MAX] = { 0 };
        int count = 0;

        Vector2 initialEnd = { start.x + length*sinf(0.0f), start.y - length*cosf(0.0f) };
        branches[count++] = (Branch){start, initialEnd, 0.0f, length};

        for (int i = 0; i < count; i++)
        {
            Branch branch = branches[i];
            if (branch.length < 2) continue;

            float nextLength = branch.length*branchDecay;

            if (count < maxBranches && nextLength >= 2)
            {
                Vector2 branchStart = branch.end;

                float angle1 = branch.angle + theta;
                Vector2 branchEnd1 = { branchStart.x + nextLength*sinf(angle1), branchStart.y - nextLength*cosf(angle1) };
                branches[count++] = (Branch){branchStart, branchEnd1, angle1, nextLength};

                float angle2 = branch.angle - theta;
                Vector2 branchEnd2 = { branchStart.x + nextLength*sinf(angle2), branchStart.y - nextLength*cosf(angle2) };
                branches[count++] = (Branch){branchStart, branchEnd2, angle2, nextLength};
            }
        }
        //----------------------------------------------------------------------------------
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            for (int i = 0; i < count; i++)
            {
                Branch branch = branches[i];
                if (branch.length >= 2)
                {
                    if (bezier) DrawLineBezier(branch.start, branch.end, thick, RED);
                    else DrawLineEx(branch.start, branch.end, thick, RED);
                }
            }

            DrawLine(screenWidth-220, 0, screenWidth-220, GetScreenHeight(), (Color){ 218, 218, 218, 255 });
            DrawRectangle(screenWidth-220, 0, GetScreenWidth(), GetScreenHeight(), (Color){ 232, 232, 232, 255 });

            // Draw GUI controls
            //------------------------------------------------------------------------------
            GuiSliderBar((Rectangle){ screenWidth-160, 40, 120, 20}, "Angle", TextFormat("%.0f", angle), &angle, 0, 180);
            GuiSliderBar((Rectangle){ screenWidth-160, 70, 120, 20 }, "Length", TextFormat("%.1f", length), &length, 12.0f, 240.0f);
            GuiSliderBar((Rectangle){ screenWidth-160, 100, 120, 20}, "Decay", TextFormat("%.2f", branchDecay), &branchDecay, 0.1f, 0.78f);
            GuiSliderBar((Rectangle){ screenWidth-160, 130, 120, 20 }, "Depth", TextFormat("%.0f", treeDepth), &treeDepth, 1.0f, DEPTH_MAX);
            GuiSliderBar((Rectangle){ screenWidth-160, 160, 120, 20}, "Thick", TextFormat("%.0f", thick), &thick, 1, 10);
            GuiCheckBox((Rectangle){ screenWidth-160, 190, 20, 20 }, "Bezier", &bezier);
            //------------------------------------------------------------------------------

            DrawFPS(10, 10);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}