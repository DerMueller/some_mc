#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>

#define W 1280
#define H 720

#define TARGET_FPS 60

// #include "true_perlin_2d.h"
#include "open-simplex-noise.h"
// #include <cstdint.h>

Texture2D tex;
struct osn_context *ctx;

// your noise function here
double noise2d(double x, double y) {
    return (double) (open_simplex_noise2(ctx, x, y) + 1.0 ) / 2.0; // normalize to [0, 1]
}

void regenTexture(double scale);


int main(void) {

    double scale = 1.0;

    InitWindow(W, H, "noise test");

    open_simplex_noise(77374, &ctx);

    double min = 1.0;
    double max = -1.0;
    double value = 0.0;
    for (short i = 0; i < 1000; i++)
    {
        // printf("noise at %d,%d: %f\n", i, i, open_simplex_noise2(ctx, i, i));
        value = noise2d(2, i);
        if (value < min)
        {
            min = value;
        }
        if (value > max)
        {
            max = value;
        }
    }

    printf("min: %f\n", min);
    printf("max: %f\n", max);
    min = 1.0;
    max = -1.0;
    for (short i = 0; i < 1000; i++)
    {
        // printf("noise at %d,%d: %f\n", i, i, open_simplex_noise2(ctx, i, i));
        value = noise2d(2, i * 0.01 * scale);
        if (value < min)
        {
            min = value;
        }
        if (value > max)
        {
            max = value;
        }
    }

    printf("min: %f\n", min);
    printf("max: %f\n", max);
    

    regenTexture(scale);


    int frame_keydown = 0;

    SetTargetFPS(TARGET_FPS);  

    unsigned char update = 0;

    IsKeyReleased(KEY_SPACE); 

    while (!WindowShouldClose()) {
        if (update == 1) {
            update = 0;
            regenTexture(scale);
        }
        if (IsKeyReleased(KEY_SPACE)) {
            scale = 1.0;
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
            open_simplex_noise_free(ctx);
            open_simplex_noise(1, &ctx);
            update = 1;
        }
        if (IsKeyPressed(KEY_TWO)) {
            open_simplex_noise_free(ctx);
            open_simplex_noise(2, &ctx);
            update = 1;
        }
        if (IsKeyPressed(KEY_THREE)) {
            open_simplex_noise_free(ctx);
            open_simplex_noise(3, &ctx);
            update = 1;
        }

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
            open_simplex_noise_free(ctx);
            int rand_seed = rand();
            printf("rand seed: %d\n", rand_seed);
            open_simplex_noise(rand_seed, &ctx);

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

void regenTexture(double scale) {
    UnloadTexture(tex);
    Image img = GenImageColor(W, H, BLACK);
    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            double n = noise2d(x * 0.01 * scale, y * 0.01 * scale); // your function
            unsigned char v = (unsigned char)(255 * n);
            ImageDrawPixel(&img, x, y, (Color){ v, v, v, 255 });
        }
    }
    tex = LoadTextureFromImage(img);
    UnloadImage(img);
}