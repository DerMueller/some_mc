#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define W 1280
#define H 720


#define TARGET_FPS 60

#include "true_perlin_2d.h"
// #include <cstdint.h>

// your noise function here
float noise2d(float x, float y) {
    return Perlin2D(x, y);
}

void regenTexture(float scale);

Texture2D tex;

int main(void) {

    float scale = 2.0f;

    InitWindow(W, H, "noise test");

    Perlin2DSetSeed(78374);



    regenTexture(scale);


    int frame_keydown = 0;

    SetTargetFPS(TARGET_FPS);  

    uint8_t update = 0;

    IsKeyReleased(KEY_SPACE); 

    while (!WindowShouldClose()) {
        if (update == 1) {
            update = 0;
            regenTexture(scale);
        }
                if (IsKeyReleased(KEY_SPACE)) {
            scale = 1.0f;
            update = 1;
        }
        if (frame_keydown > TARGET_FPS/2) {
            update = 1;
            frame_keydown = 0;
        }
        if (IsKeyDown(KEY_UP))    {scale *= 1.01f;frame_keydown++;}
        if (IsKeyDown(KEY_DOWN))  {scale *= 0.99f;frame_keydown++;}
        if (IsKeyReleased(KEY_UP))   {update = 1;frame_keydown = 0;}
        if (IsKeyReleased(KEY_DOWN)) {update = 1;frame_keydown = 0;}

        if (IsKeyPressed(KEY_ONE)) {
            __perlin2d_permutation[0]++;
            update = 1;
        }
        if (IsKeyPressed(KEY_TWO)) {
            __perlin2d_permutation[1]++;
            update = 1;
        }
        if (IsKeyPressed(KEY_THREE)) {__perlin2d_permutation[2]++;update = 1;}
        if (IsKeyPressed(KEY_FOUR)) {__perlin2d_permutation[3]++;update = 1;}
        if (IsKeyPressed(KEY_FIVE)) {__perlin2d_permutation[4]++;update = 1;}
        if (IsKeyPressed(KEY_SIX)) {__perlin2d_permutation[5]++;update = 1;}
        if (IsKeyPressed(KEY_SEVEN)) {__perlin2d_permutation[6]++;update = 1;}
        if (IsKeyPressed(KEY_EIGHT)) {__perlin2d_permutation[7]++;update = 1;}
        if (IsKeyPressed(KEY_NINE)) {__perlin2d_permutation[8]++;update = 1;}
        if (IsKeyPressed(KEY_ZERO)) {__perlin2d_permutation[9]++;update = 1;}
        if (IsKeyPressed(KEY_L)) {
            for (unsigned char i = 0; i < 250; i++)
            {
                __perlin2d_permutation[i] = (unsigned char)17;
            }
            
            update = 1;
        }
        if (IsKeyPressed(KEY_A)) {
            for (unsigned char i = 0; i < 250; i++) {
                __perlin2d_permutation[i] = (unsigned char) i;
            }
            update = 1;
        }
        if (IsKeyPressed(KEY_R)) {
            uint16_t seed = rand();
            printf("Setting seed to %d\n", seed);
            Perlin2DSetSeed(seed);
            // for (unsigned char i = 0; i < 250; i++) {
            //     __perlin2d_permutation[i] = (unsigned char) (rand() % 256);

            // }

            update = 1;
        }
        if (IsKeyPressed(KEY_T)) {
            for (int x = 0; x < 16; x++) {
                for (int y = 0; y < 16; y++) {
                    printf("height at %d,%d: %f\n", x, y, Perlin2D(x*scale*0.1f, y*scale*0.1f));
                    // blockPositions[x * 16 + y] = (Vector3){ (x) + 0.5f, 0.5f + Perlin2D(x*10.0, y*10.0), y + 0.5f };
                }
            }

            update = 1;
        }

        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexture(tex, 0, 0, WHITE);
        EndDrawing();
    }
    UnloadTexture(tex);
    CloseWindow();
    return 0;
}

void regenTexture(float scale) {
    UnloadTexture(tex);
    Image img = GenImageColor(W, H, BLACK);
    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            float n = noise2d(x * 0.01f * scale, y * 0.01f * scale); // your function
            unsigned char v = (unsigned char)(255 * n);
            ImageDrawPixel(&img, x, y, (Color){ v, v, v, 255 });
        }
    }
    tex = LoadTextureFromImage(img);
    UnloadImage(img);
}