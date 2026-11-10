#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define W 1280
#define H 720


#define TARGET_FPS 60

#include "true_perlin_2d_multiple_tables.h"
#include "open_simplex_noise_2d.h"
// #include <cstdint.h>
struct Perlin2DContext *perlin_ctx;
struct osn_context *open_simplex_ctx;
// your noise function here
float noise2d(float x, float y) {
    return Perlin2D(perlin_ctx, x, y);
}
float noise2d_2(float x, float y) {
    return (float) (open_simplex_noise2(open_simplex_ctx, x, y) + 1.0 ) / 2.0; // normalize to [0, 1]
}

void regenTexture(float scale);
void regenTexture2(float scale);

Texture2D tex;
Texture2D tex2;

int main(void) {

    float scale = 2.0f;

    InitWindow(W, H, "noise test");

    Perlin2DSetSeed(&perlin_ctx, 78374);
    open_simplex_noise(&open_simplex_ctx, 77374);

    printf("perlin at 0,0: %f\n", Perlin2D(perlin_ctx, 0.0f, 0.0f));
    printf("perlin at 1,2: %f\n", Perlin2D(perlin_ctx, 0.1f, 0.2f));

    regenTexture(scale);
    regenTexture2(scale);


    char tex_enabled = 1;
    char tex2_enabled = 1;
    char swap_enabled = 0;

    int frame_keydown = 0;

    SetTargetFPS(TARGET_FPS);  

    uint8_t update = 0;

    IsKeyReleased(KEY_SPACE); 

    while (!WindowShouldClose()) {
        if (update == 1) {
            update = 0;
            regenTexture(scale);
            regenTexture2(scale);
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
            tex_enabled = !tex_enabled;
            update = 1;
        }
        if (IsKeyPressed(KEY_TWO)) {
            tex2_enabled = !tex2_enabled;
            update = 1;
        }
        if (IsKeyPressed(KEY_THREE)) {
            swap_enabled = !swap_enabled;
            update = 1;
        }
        // if (IsKeyPressed(KEY_THREE)) {__perlin2d_permutation[2]++;update = 1;}
        // if (IsKeyPressed(KEY_FOUR)) {__perlin2d_permutation[3]++;update = 1;}
        // if (IsKeyPressed(KEY_FIVE)) {__perlin2d_permutation[4]++;update = 1;}
        // if (IsKeyPressed(KEY_SIX)) {__perlin2d_permutation[5]++;update = 1;}
        // if (IsKeyPressed(KEY_SEVEN)) {__perlin2d_permutation[6]++;update = 1;}
        // if (IsKeyPressed(KEY_EIGHT)) {__perlin2d_permutation[7]++;update = 1;}
        // if (IsKeyPressed(KEY_NINE)) {__perlin2d_permutation[8]++;update = 1;}
        // if (IsKeyPressed(KEY_ZERO)) {__perlin2d_permutation[9]++;update = 1;}
        // if (IsKeyPressed(KEY_L)) {
        //     for (unsigned char i = 0; i < 250; i++)
        //     {
        //         __perlin2d_permutation[i] = (unsigned char)17;
        //     }
            
        //     update = 1;
        // }
        // if (IsKeyPressed(KEY_A)) {
        //     for (unsigned char i = 0; i < 250; i++) {
        //         __perlin2d_permutation[i] = (unsigned char) i;
        //     }
        //     update = 1;
        // }
        if (IsKeyPressed(KEY_R)) {
            uint16_t seed = rand();
            printf("Setting seed to %d\n", seed);
            Perlin2DSetSeed(&perlin_ctx, seed);
            // for (unsigned char i = 0; i < 250; i++) {
            //     __perlin2d_permutation[i] = (unsigned char) (rand() % 256);

            // }

            update = 1;
        }


        BeginDrawing();
        ClearBackground(BLACK);
        if (swap_enabled) {
            if (tex_enabled) {
                DrawTexture(tex, 0, 0, (Color){255, 255, 255, 255});
            }
            if (tex2_enabled) {
                DrawTexture(tex2, 0, 0, (Color){255, 255, 255, 128});
            }
        } else {
            if (tex2_enabled) {
                DrawTexture(tex2, 0, 0, (Color){255, 255, 255, 255});
            }

            if (tex_enabled) {
   
                DrawTexture(tex, 0, 0, (Color){255, 255, 255, 128});            

            }
        }
        
        EndDrawing();
    }
    UnloadTexture(tex);
    UnloadTexture(tex2);
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
            // ImageDrawPixel(&img, x, y, (Color){ v, v, v, 255 });
            ImageDrawPixel(&img, x, y, (Color){ 0, 255, 0, v});

        }
    }
    tex = LoadTextureFromImage(img);
    UnloadImage(img);
}
void regenTexture2(float scale) {
    UnloadTexture(tex2);
    Image img = GenImageColor(W, H, BLACK);
    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            float n = noise2d_2(x * 0.01f * scale, y * 0.01f * scale); // your function
            unsigned char v = (unsigned char)(255 * n);
            // ImageDrawPixel(&img, x, y, (Color){ v, v, v, 255 });
            ImageDrawPixel(&img, x, y, (Color){ 255, 0, 0, v});
        }
    }
    tex2 = LoadTextureFromImage(img);
    UnloadImage(img);
}