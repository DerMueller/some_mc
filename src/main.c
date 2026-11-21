// this is the main entry point of the game
// created 27.10.2026

#include <raylib.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "rcamera.h"
#include "true_perlin_2d_multiple_tables.h"
#include "open_simplex_noise_2d.h"

#define GEN_COLUMNS 2
#define MAX_COLUMNS 10
#define TARGET_FPS 180

#define CHUNK_SIZE 16
#define CHUNKS 8

#define SCREEN_WIDTH 1560
#define SCREEN_HEIGHT 880


struct Perlin2DContext *perlin_ctx = NULL;
struct Perlin2DContext *perlin_ctx2 = NULL;
struct Perlin2DContext *perlin_ctx_b = NULL;
struct osn_context *open_simplex_ctx = NULL;
Vector3 blockPositions[CHUNK_SIZE*CHUNK_SIZE*CHUNKS*CHUNKS] = { 0 };
Color blockColors[CHUNK_SIZE*CHUNK_SIZE*CHUNKS*CHUNKS] = { 0 };

float factorE0 = 0.002f;
float factorE1 = 0.02f;
float factorE2 = 0.25f;
float factorE3 = 0.50f;
float factorE4 = 1.11f;
float factorE5 = 1.49f;
float factorE6 = 0.16f;

int generateChunk(int chunkX, int chunkY) {
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_SIZE; y++) {
            float height = 0;
            height += (Perlin2D(perlin_ctx,(x+chunkX*CHUNK_SIZE)*0.1f*factorE0,
                            (y+chunkY*CHUNK_SIZE)*0.1f*factorE0)*50.0);
            height += (Perlin2D(perlin_ctx2,(x+chunkX*CHUNK_SIZE)*0.1f*factorE1,
                            (y+chunkY*CHUNK_SIZE)*0.1f*factorE1)*20.0);
            height += (Perlin2D(perlin_ctx,(x+chunkX*CHUNK_SIZE)*0.1f*factorE2,
                            (y+chunkY*CHUNK_SIZE)*0.1f*factorE2)*12.0);
            height += (Perlin2D(perlin_ctx2,(x+chunkX*CHUNK_SIZE)*0.1f*factorE3,
                            (y+chunkY*CHUNK_SIZE)*0.1f*factorE3)*8.0);
            height += (Perlin2D(perlin_ctx,(x+chunkX*CHUNK_SIZE)*0.1f*factorE4,
                            (y+chunkY*CHUNK_SIZE)*0.1f*factorE4)*5.0);
            height += (Perlin2D(perlin_ctx2,(x+chunkX*CHUNK_SIZE)*0.1f*factorE5,
                            (y+chunkY*CHUNK_SIZE)*0.1f*factorE5)*2.0);
            height += (open_simplex_noise2(open_simplex_ctx,(x+chunkX*CHUNK_SIZE)*0.1f*factorE0,
                            (y+chunkY*CHUNK_SIZE)*0.1f*factorE0)*60.0);
            float r_hg = (Perlin2D(perlin_ctx,(x+chunkX*CHUNK_SIZE)*0.087f*factorE0,
                            (y+chunkY*CHUNK_SIZE)*0.1055f*factorE0));
            r_hg = (0.6f - fabsf(r_hg - 0.6f))*20.0f;
            r_hg = (r_hg * r_hg) / 144.0f;
            height -= r_hg * 8.0;
            blockPositions[x * CHUNK_SIZE + y + chunkX * CHUNK_SIZE * CHUNK_SIZE + chunkY * CHUNK_SIZE * CHUNK_SIZE * CHUNKS] = 
                (Vector3){ 
                    (x+chunkX*CHUNK_SIZE) + 0.5f, 
                    0.5f + (int) ((height) - 20.0f),
                    (y+chunkY*CHUNK_SIZE) + 0.5f 
                };
            float colorV = Perlin2D(perlin_ctx_b,
                            (x+chunkX*CHUNK_SIZE)*0.1f*factorE6,
                            (y+chunkY*CHUNK_SIZE)*0.1f*factorE6);
            blockColors[x * CHUNK_SIZE + y + chunkX * CHUNK_SIZE * CHUNK_SIZE + chunkY * CHUNK_SIZE * CHUNK_SIZE * CHUNKS] = colorV > 0.5f ? GetColor(0x416833ff) : GetColor(0xdbd0a8ff);
        }
    }    
    return 0;
}

int generateChunks() {
    for (int ch_x = 0; ch_x < CHUNKS; ch_x++) {
        for (int ch_y = 0; ch_y < CHUNKS; ch_y++) {
            generateChunk(ch_x, ch_y);
        }
    }
    return 0;
}

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void) {

    Perlin2DSetSeed(&perlin_ctx, 31833);
    Perlin2DSetSeed(&perlin_ctx2, 523893);
    Perlin2DSetSeed(&perlin_ctx_b, 1235);
    open_simplex_noise(&open_simplex_ctx, 12345);

    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = SCREEN_WIDTH;
    const int screenHeight = SCREEN_HEIGHT;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - 3d camera first person");
    // 1. Disable the default ESC close behavior
    SetExitKey(KEY_NULL); 

    // Define the camera to look into our 3d world (position, target, up vector)
    Camera camera = { 0 };
    camera.position = (Vector3){ 0.5f, 46.0f, 0.5f };    // Camera position
    camera.target = (Vector3){ 1.8f, 45.1f, 1.8f };      // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 68.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type

    int cameraMode = CAMERA_CUSTOM;                // Camera mode type
    uint8_t isPaused = 0;


    // Generates some random columns
    float heights[MAX_COLUMNS] = { 0 };
    Vector3 positions[MAX_COLUMNS] = { 0 };
    Color colors[MAX_COLUMNS] = { 0 };

    for (int i = 0; i < GEN_COLUMNS; i++)
    {
        heights[i] = (float)GetRandomValue(1, 12);
        positions[i] = (Vector3){ (float)GetRandomValue(-15, 15), heights[i]/2.0f, (float)GetRandomValue(-15, 15) };
        colors[i] = (Color){ GetRandomValue(20, 255), GetRandomValue(10, 55), 30, 255 };
    }

    generateChunks();

    DisableCursor();                    // Limit cursor to relative movement inside the window

    SetTargetFPS(TARGET_FPS);                   // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    uint8_t game_exit = 0;
    // Main game loop
    while (!game_exit)        // Detect window close button or ESC key
    {
        if (isPaused) {
            if (IsWindowFocused()) {
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    isPaused = false;
                    DisableCursor();
                    cameraMode = CAMERA_FREE;
                }
            }
        } else {
            if (!IsWindowFocused()) {
                isPaused = true;
                EnableCursor();
                cameraMode = CAMERA_CUSTOM;
            } else {
                UpdateCamera(&camera, cameraMode);  
            }
        }

        if (IsKeyPressed(KEY_ESCAPE)) {
            if (isPaused) {
                game_exit = 1;
            } else {
                isPaused = true;
                EnableCursor();
                cameraMode = CAMERA_CUSTOM;
            }
        }

        // Update
        //----------------------------------------------------------------------------------
        // Switch camera mode
        if (IsKeyPressed(KEY_ONE))
        {
            cameraMode = CAMERA_FREE;
            camera.up = (Vector3){ 0.0f, 1.0f, 0.0f }; // Reset roll
        }

        if (IsKeyPressed(KEY_TWO))
        {
            cameraMode = CAMERA_FIRST_PERSON;
            camera.up = (Vector3){ 0.0f, 1.0f, 0.0f }; // Reset roll
        }

        if (IsKeyPressed(KEY_THREE))
        {
            cameraMode = CAMERA_THIRD_PERSON;
            camera.up = (Vector3){ 0.0f, 1.0f, 0.0f }; // Reset roll
        }

        if (IsKeyPressed(KEY_FOUR))
        {
            cameraMode = CAMERA_ORBITAL;
            camera.up = (Vector3){ 0.0f, 1.0f, 0.0f }; // Reset roll
        }

        // Switch camera projection
        if (IsKeyPressed(KEY_P))
        {
            if (camera.projection == CAMERA_PERSPECTIVE)
            {
                // Create isometric view
                cameraMode = CAMERA_THIRD_PERSON;
                // Note: The target distance is related to the render distance in the orthographic projection
                camera.position = (Vector3){ 0.0f, 2.0f, -100.0f };
                camera.target = (Vector3){ 0.0f, 2.0f, 0.0f };
                camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
                camera.projection = CAMERA_ORTHOGRAPHIC;
                camera.fovy = 20.0f; // near plane width in CAMERA_ORTHOGRAPHIC
                CameraYaw(&camera, -135*DEG2RAD, true);
                CameraPitch(&camera, -45*DEG2RAD, true, true, false);
            }
            else if (camera.projection == CAMERA_ORTHOGRAPHIC)
            {
                // Reset to default view
                cameraMode = CAMERA_THIRD_PERSON;
                camera.position = (Vector3){ 0.0f, 2.0f, 10.0f };
                camera.target = (Vector3){ 0.0f, 2.0f, 0.0f };
                camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
                camera.projection = CAMERA_PERSPECTIVE;
                camera.fovy = 60.0f;
            }
        }
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            cameraMode = CAMERA_FREE;
        }
        if (IsKeyPressed(KEY_T)) {
            // rand seed
            uint16_t seed = rand();
            printf("Setting seed to %d\n", seed);
            Perlin2DSetSeed(&perlin_ctx, seed);
            generateChunks();
        }
        if (IsKeyPressed(KEY_U)) {
            // rand seed
            uint16_t seed = rand();
            printf("Setting seed to %d\n", seed);
            Perlin2DSetSeed(&perlin_ctx2, seed);
            generateChunks();
        }
        if (IsKeyPressed(KEY_Y)) {
            // rand seed
            uint16_t seed = rand();
            printf("Setting seed to %d\n", seed);
            Perlin2DSetSeed(&perlin_ctx_b, seed);
            generateChunks();
        }

        // Update camera computes movement internally depending on the camera mode
        // Some default standard keyboard/mouse inputs are hardcoded to simplify use
        // For advanced camera controls, it's recommended to compute camera movement manually
        // UpdateCamera(&camera, cameraMode);                  // Update camera
/*
        // Camera PRO usage example (EXPERIMENTAL)
        // This new camera function allows custom movement/rotation values to be directly provided
        // as input parameters, with this approach, rcamera module is internally independent of raylib inputs
        UpdateCameraPro(&camera,
            (Vector3){
                (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))*0.1f -      // Move forward-backward
                (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))*0.1f,
                (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))*0.1f -   // Move right-left
                (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))*0.1f,
                0.0f                                                // Move up-down
            },
            (Vector3){
                GetMouseDelta().x*0.05f,                            // Rotation: yaw
                GetMouseDelta().y*0.05f,                            // Rotation: pitch
                0.0f                                                // Rotation: roll
            },
            GetMouseWheelMove()*2.0f);                              // Move to target (zoom)
*/
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            BeginMode3D(camera);

                DrawPlane((Vector3){ 0.0f, 0.0f, 0.0f }, (Vector2){ 32.0f, 32.0f }, LIGHTGRAY); // Draw ground
                // DrawCube((Vector3){ -16.0f, 2.5f, 0.0f }, 1.0f, 5.0f, 32.0f, BLUE);     // Draw a blue wall
                // DrawCube((Vector3){ 16.0f, 2.5f, 0.0f }, 1.0f, 5.0f, 32.0f, LIME);      // Draw a green wall
                // DrawCube((Vector3){ 0.0f, 2.5f, 16.0f }, 32.0f, 5.0f, 1.0f, GOLD);      // Draw a yellow wall

                // Draw some cubes around
                for (int i = 0; i < MAX_COLUMNS; i++)
                {
                    DrawCube(positions[i], 2.0f, heights[i], 2.0f, colors[i]);
                    DrawCubeWires(positions[i], 2.0f, heights[i], 2.0f, MAROON);
                }
                for (int i = 0; i < CHUNKS*CHUNKS*CHUNK_SIZE*CHUNK_SIZE; i++)
                {
                    DrawCube(blockPositions[i], 1.0f, 1.0f, 1.0f, blockColors[i]);
                    DrawCubeWires(blockPositions[i], 1.0f, 1.0f, 1.0f, DARKGRAY);
                }
                

                // Draw player cube
                if (cameraMode == CAMERA_THIRD_PERSON)
                {
                    DrawCube(camera.target, 0.5f, 0.5f, 0.5f, PURPLE);
                    DrawCubeWires(camera.target, 0.5f, 0.5f, 0.5f, DARKPURPLE);
                }

            EndMode3D();

            // Draw info boxes
            DrawRectangle(5, 5, 330, 100, Fade(SKYBLUE, 0.5f));
            DrawRectangleLines(5, 5, 330, 100, BLUE);

            DrawText("Camera controls:", 15, 15, 10, BLACK);
            DrawText("- Move keys: W, A, S, D, Space, Left-Ctrl", 15, 30, 10, BLACK);
            DrawText("- Look around: arrow keys or mouse", 15, 45, 10, BLACK);
            DrawText("- Camera mode keys: 1, 2, 3, 4", 15, 60, 10, BLACK);
            DrawText("- Zoom keys: num-plus, num-minus or mouse scroll", 15, 75, 10, BLACK);
            DrawText("- Camera projection key: P", 15, 90, 10, BLACK);

            DrawRectangle(SCREEN_WIDTH-200, 5, 195, 100, Fade(SKYBLUE, 0.5f));
            DrawRectangleLines(SCREEN_WIDTH-200, 5, 195, 100, BLUE);

            DrawText("Camera status:", SCREEN_WIDTH-190, 15, 10, BLACK);
            DrawText(TextFormat("- Mode: %s", (cameraMode == CAMERA_FREE) ? "FREE" :
                                              (cameraMode == CAMERA_FIRST_PERSON) ? "FIRST_PERSON" :
                                              (cameraMode == CAMERA_THIRD_PERSON) ? "THIRD_PERSON" :
                                              (cameraMode == CAMERA_ORBITAL) ? "ORBITAL" : "CUSTOM"), SCREEN_WIDTH-190, 30, 10, BLACK);
            DrawText(TextFormat("- Projection: %s", (camera.projection == CAMERA_PERSPECTIVE) ? "PERSPECTIVE" :
                                                    (camera.projection == CAMERA_ORTHOGRAPHIC) ? "ORTHOGRAPHIC" : "CUSTOM"), SCREEN_WIDTH-190, 45, 10, BLACK);
            DrawText(TextFormat("- Position: (%06.3f, %06.3f, %06.3f)", camera.position.x, camera.position.y, camera.position.z), SCREEN_WIDTH-190, 60, 10, BLACK);
            DrawText(TextFormat("- Target: (%06.3f, %06.3f, %06.3f)", camera.target.x, camera.target.y, camera.target.z), SCREEN_WIDTH-190, 75, 10, BLACK);
            DrawText(TextFormat("- Up: (%06.3f, %06.3f, %06.3f)", camera.up.x, camera.up.y, camera.up.z), SCREEN_WIDTH-190, 90, 10, BLACK);

            if (isPaused) {
                DrawText("PAUSED", SCREEN_WIDTH/2-15, 15, 20, BLACK);
            }

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}