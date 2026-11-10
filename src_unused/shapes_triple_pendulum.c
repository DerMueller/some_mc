/*******************************************************************************************
*
*   raylib [shapes] example - double pendulum
*
*   Example complexity rating: [★★☆☆] 2/4
*
*   Example originally created with raylib 5.5, last time updated with raylib 5.5
*
*   Example contributed by JoeCheong (@Joecheong2006) and reviewed by Ramon Santamaria (@raysan5)
*
*   Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software
*
*   Copyright (c) 2025 JoeCheong (@Joecheong2006)
*
********************************************************************************************/

#include "raylib.h"

#include <math.h>       // Required for: sin(), cos(), PI

// Constant for Simulation
#define SIMULATION_SPEED 1
#define SIMULATION_STEPS 30
#define TARGET_FPS 90
#define G 9.81f

//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
static Vector2 CalculatePendulumEndPoint(float l, float theta);
static Vector2 CalculateDoublePendulumEndPoint(float l1, float theta1, float l2, float theta2);
static Vector2 CalculateTriplePendulumEndPoint(float l1, float theta1, float l2, float theta2, float l3, float theta3);

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1300;
    const int screenHeight = 700;

    SetConfigFlags(FLAG_WINDOW_HIGHDPI);
    InitWindow(screenWidth, screenHeight, "raylib [shapes] example - double pendulum");

    // Simulation Parameters
    float l1 = 12.0f, m1 = 0.2f, theta1 = DEG2RAD*160, w1 = 0;
    float l2 = 12.0f, m2 = 0.15f, theta2 = DEG2RAD*161, w2 = 0;
    float l3 = 12.0f, m3 = 0.1f, theta3 = DEG2RAD*162, w3 = 0;
    float lengthScaler = 0.1f;
    float totalM = m1 + m2 + m3;

    Vector2 previousPosition = CalculateTriplePendulumEndPoint(l1, theta1, l2, theta2, l3, theta3);
    previousPosition.x += ((float)screenWidth/2);
    previousPosition.y += ((float)screenHeight/2 - 100);

    // Scale length
    float L1 = l1*lengthScaler;
    float L2 = l2*lengthScaler;
    float L3 = l3*lengthScaler;

    // Draw parameters
    float lineThick = 12, trailThick = 2;
    float fateAlpha = 0.01f;

    // Create framebuffer
    RenderTexture2D target = LoadRenderTexture(screenWidth, screenHeight);
    SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);

    SetTargetFPS(TARGET_FPS);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        float dt = GetFrameTime()*SIMULATION_SPEED;
        float step = dt/SIMULATION_STEPS, step2 = step*step;

        // Update Physics - larger steps = better approximation
        for (int i = 0; i < SIMULATION_STEPS*SIMULATION_SPEED; i++)
        {
            // Pairwise angle differences
            float d12 = theta1 - theta2;
            float d13 = theta1 - theta3;
            float d23 = theta2 - theta3;

            float s12 = sinf(d12), c12 = cosf(d12);
            float s13 = sinf(d13), c13 = cosf(d13);
            float s23 = sinf(d23), c23 = cosf(d23);

            float w1sq = w1*w1, w2sq = w2*w2, w3sq = w3*w3;

            // Mass matrix M (3x3, symmetric)
            // M11 M12 M13
            // M21 M22 M23
            // M31 M32 M33
            float M11 = (m1 + m2 + m3)*L1*L1;
            float M22 = (m2 + m3)*L2*L2;
            float M33 = m3*L3*L3;
            float M12 = (m2 + m3)*L1*L2*c12;
            float M13 = m3*L1*L3*c13;
            float M23 = m3*L2*L3*c23;

            // Forcing vector f
            float f1 = -(m1 + m2 + m3)*G*L1*sinf(theta1)
                    - (m2 + m3)*L1*L2*w2sq*s12
                    - m3*L1*L3*w3sq*s13;

            float f2 = -(m2 + m3)*G*L2*sinf(theta2)
                    + (m2 + m3)*L1*L2*w1sq*s12
                    - m3*L2*L3*w3sq*s23;

            float f3 = -m3*G*L3*sinf(theta3)
                    + m3*L1*L3*w1sq*s13
                    + m3*L2*L3*w2sq*s23;

            // Solve M * a = f via Cramer's rule
            // det(M) = M11*(M22*M33 - M23*M23)
            //        - M12*(M12*M33 - M23*M13)
            //        + M13*(M12*M23 - M22*M13)
            float detM = M11*(M22*M33 - M23*M23)
                    - M12*(M12*M33 - M23*M13)
                    + M13*(M12*M23 - M22*M13);

            // Guard against degenerate determinant (shouldn't happen physically)
            if (fabsf(detM) < 1e-12f) detM = 1e-12f;

            // Replace column 1 with f -> det for a1
            float det1 = f1*(M22*M33 - M23*M23)
                    - M12*(f2*M33 - M23*f3)
                    + M13*(f2*M23 - M22*f3);

            // Replace column 2 with f -> det for a2
            float det2 = M11*(f2*M33 - M23*f3)
                    - f1*(M12*M33 - M23*M13)
                    + M13*(M12*f3 - f2*M13);

            // Replace column 3 with f -> det for a3
            float det3 = M11*(M22*f3 - f2*M23)
                    - M12*(M12*f3 - f2*M13)
                    + f1*(M12*M23 - M22*M13);

            float a1 = det1/detM;
            float a2 = det2/detM;
            float a3 = det3/detM;

            // Update thetas (semi-implicit / midpoint style as in original)
            theta1 += w1*step + 0.5f*a1*step2;
            theta2 += w2*step + 0.5f*a2*step2;
            theta3 += w3*step + 0.5f*a3*step2;

            // Update omegas
            w1 += a1*step;
            w2 += a2*step;
            w3 += a3*step;
        }

        // Calculate position
        Vector2 currentPosition = CalculateTriplePendulumEndPoint(l1, theta1, l2, theta2, l3, theta3);
        currentPosition.x += (float)screenWidth/2;
        currentPosition.y += (float)screenHeight/2 - 100;

        // Draw to render texture
        BeginTextureMode(target);
            // Draw a transparent rectangle - smaller alpha = longer trails
            DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, fateAlpha));

            // Draw trail
            DrawCircleV(previousPosition, trailThick, RED);
            DrawLineEx(previousPosition, currentPosition, trailThick*2, RED);
        EndTextureMode();

        // Update previous position
        previousPosition = currentPosition;
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(BLACK);

            // Draw trails texture
            DrawTextureRec(target.texture, (Rectangle){ 0, 0, (float)target.texture.width, (float)-target.texture.height }, (Vector2){ 0, 0 }, WHITE);

            // Draw double pendulum
            DrawRectanglePro((Rectangle){ screenWidth/2.0f, screenHeight/2.0f - 100, 10*l1, lineThick },
                (Vector2){0, lineThick*0.5f}, 90 - RAD2DEG*theta1, RAYWHITE);

            Vector2 endpoint1 = CalculatePendulumEndPoint(l1, theta1);
            DrawRectanglePro((Rectangle){ screenWidth/2.0f + endpoint1.x, screenHeight/2.0f - 100 + endpoint1.y, 10*l2, lineThick },
                (Vector2){0, lineThick*0.5f}, 90 - RAD2DEG*theta2, RAYWHITE);

            Vector2 endpoint2 = CalculatePendulumEndPoint(l2, theta2);
            DrawRectanglePro((Rectangle){ screenWidth/2.0f + endpoint1.x + endpoint2.x, screenHeight/2.0f - 100 + endpoint1.y + endpoint2.y, 10*l3, lineThick },
                (Vector2){0, lineThick*0.5f}, 90 - RAD2DEG*theta3, RAYWHITE);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadRenderTexture(target);

    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

//----------------------------------------------------------------------------------
// Module Functions Definition
//----------------------------------------------------------------------------------
// Calculate pendulum end point
static Vector2 CalculatePendulumEndPoint(float l, float theta)
{
    return (Vector2){ 10*l*sinf(theta), 10*l*cosf(theta) };
}

// Calculate double pendulum end point
static Vector2 CalculateDoublePendulumEndPoint(float l1, float theta1, float l2, float theta2)
{
    Vector2 endpoint1 = CalculatePendulumEndPoint(l1, theta1);
    Vector2 endpoint2 = CalculatePendulumEndPoint(l2, theta2);
    return (Vector2){ endpoint1.x + endpoint2.x, endpoint1.y + endpoint2.y };
}

static Vector2 CalculateTriplePendulumEndPoint(float l1, float theta1, float l2, float theta2, float l3, float theta3)
{
    Vector2 endpoint1 = CalculatePendulumEndPoint(l1, theta1);
    Vector2 endpoint2 = CalculatePendulumEndPoint(l2, theta2);
    Vector2 endpoint3 = CalculatePendulumEndPoint(l3, theta3);
    return (Vector2){ endpoint1.x + endpoint2.x + endpoint3.x, endpoint1.y + endpoint2.y + endpoint3.y };
}