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

float hue2rgb(float p, float q, float t) {
    if (t < 0.0f) t += 1;
    if (t > 1.0f) t -= 1;
    if (t < 1.0f/6) return p + (q - p) * 6.0f * t;
    if (t < 1.0f/2) return q;
    if (t < 2.0f/3) return p + (q - p) * (2.0f/3 - t) * 6.0f;
    return p;
}
Color hslToRgb(float h, float s, float l) {
    float r, g, b;

    if (s == 0) {
        r = g = b = l; // achromatic
    } else {
        float q = l < 0.5 ? l * (1.0f + s) : l + s - l * s;
        float p = 2 * l - q;

        r = hue2rgb(p, q, h + 1.0f/3);
        g = hue2rgb(p, q, h);
        b = hue2rgb(p, q, h - 1.0f/3);
    }

    return (Color){ round(r * 255), round(g * 255), round(b * 255), 255 };
}

void regenTexture(float scale);
void c_lsd_single(char);
void c_lsd();
void relTex();

unsigned short lsdit = 0;
char lsd = 0;

Texture2D tex;
Image img;
Image img2;
short int it_img[W][H] = {0};

int main(void) {
    float scale = 1.1f;

    InitWindow(W, H, "noise test");

    Perlin2DSetSeed(78374);
    regenTexture(scale);

    int frame_keydown = 0;
    uint8_t update = 0;

    SetTargetFPS(TARGET_FPS);  
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
        if (IsKeyPressed(KEY_M)) {
            lsd = !lsd;
        }
        if (lsd) {
            c_lsd();
            relTex();
        }
        if (IsKeyPressed(KEY_N)) {
            c_lsd();
            relTex();
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

short mint = 0, maxt = 0;
float min = 1.0f, max = 0.0f;
float lsd_modifier = 1.0f;
void regenTexture(float scale) {
    img = GenImageColor(W, H, BLACK);
    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            float n = noise2d(x * 0.01f * scale, y * 0.01f * scale); // your function
            it_img[x][y] = n * 360.0f;
            ImageDrawPixel(&img, x, y, hslToRgb(n, 1.0f, 0.5f));

            if (n < min) min = n;
            if (n > max) max = n;
        }
    }

    lsd_modifier = 1.0f/(max - min);

    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            it_img[x][y] = (it_img[x][y] - (min*360.0f)) * lsd_modifier;
            if (it_img[x][y] < 0) it_img[x][y] += 360.0f;
            if (it_img[x][y] < mint) mint = it_img[x][y];
            if (it_img[x][y] > maxt) maxt = it_img[x][y];
        }
    }
    // c_lsd_single(1);
    printf("mint: %d, maxt: %d\n", mint, maxt);

    relTex();
    // UnloadImage(img);
}

void c_lsd_single(char single) {
    // UnloadTexture(tex);
    img2 = ImageCopy(img);
    if (!single) {
        lsdit += 4;
        if (lsdit > 360) lsdit = 1;

    }
    // char disable = 1;

    short iterations = 0;
    // iterate image
    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            iterations = it_img[x][y] + lsdit;

            if (iterations > 360) {
                iterations -= 360;
            }

            ImageDrawPixel(&img, x, y, hslToRgb(iterations/360.0f, 1.0f, 0.5f));
        }
    }
    // if (disable) {
    //     keepGen = 0;
    // }
    relTex();
}

void c_lsd() {
    c_lsd_single(0);
}

void relTex() {
    UnloadTexture(tex);
    tex = LoadTextureFromImage(img);
}