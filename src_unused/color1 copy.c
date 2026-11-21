#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#define W 1280
#define H 720


#define DOTS 1070000
#define TARGET_FPS 60

unsigned int dotn = 1;
unsigned int dotit = 0;

short dotx[DOTS] = {100};
short doty[DOTS] = {100};

void regenTexture(float scale);
char regFreePixel(Image, int, int);

Texture2D tex;
Image img;

float hueToRgb(float p, float q, float t);

Color hslToRgb(float h, float s, float l) {
  float r, g, b;

  if (s == 0) {
    r = g = b = l; // achromatic
  } else {
    float q = l < 0.5 ? l * (1 + s) : l + s - l * s;
    float p = 2 * l - q;
    r = hueToRgb(p, q, h + 1/3);
    g = hueToRgb(p, q, h);
    b = hueToRgb(p, q, h - 1/3);
  }

  return (Color){ round(r * 255), round(g * 255), round(b * 255), 255 };
}

float hueToRgb(float p, float q, float t) {
  if (t < 0) t += 1;
  if (t > 1) t -= 1;
  if (t < 1/6) return p + (q - p) * 6 * t;
  if (t < 1/2) return q;
  if (t < 2/3) return p + (q - p) * (2/3 - t) * 6;
  return p;
}

int main(void) {

    float scale = 2.0f;

    InitWindow(W, H, "noise test");

    img = GenImageColor(W, H, WHITE);

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

        if (IsKeyPressed(KEY_R)) {
            regenTexture(1.1f);



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

    printf("here");

    // copy dotx to local array
    short dotx_local[DOTS] = {0};
    memcpy(dotx_local, dotx, sizeof(dotx_local));

    short doty_local[DOTS] = {0};
    memcpy(doty_local, doty, sizeof(doty_local));

    int dotn_local = dotn;
    dotn = 0;
    dotit++;
    if (dotit == 360) dotit = 1;

    printf("dotit: %d\n", dotit);


    for (int n = 0; n < dotn_local; n++) {
        int x = dotx_local[n];
        int y = doty_local[n];

        regFreePixel(img, x-1, y-2);
        regFreePixel(img, x+1, y-2);
        regFreePixel(img, x-1, y+2);
        regFreePixel(img, x+1, y+2);

        regFreePixel(img, x-2, y-1);
        regFreePixel(img, x-2, y+1);
        regFreePixel(img, x+2, y-1);
        regFreePixel(img, x+2, y+1);

        // ImageDrawPixel(&img, x, y, hslToRgb(dotit/360.0f, 0.5f, 1.0f));
        ImageDrawPixel(&img, x, y, (Color){ dotit, dotit+30, dotit, 255 });
    }

    printf("dotn: %d\n", dotn);


    // for (int y = 0; y < H; y++) {
    //     for (int x = 0; x < W; x++) {
    //         unsigned char v = (unsigned char)(255 * n);
    //         ImageDrawPixel(&img, x, y, (Color){ v, v, v, 255 });
    //     }
    // }
    tex = LoadTextureFromImage(img);
    // UnloadImage(img);
}

char regFreePixel(Image img, int x, int y) {
    // printf("GetImageColor(img, %d, %d).r = %d\n",x,y, GetImageColor(img, x, y).r);
    if (GetImageColor(img, x, y).r == 255) {
        dotx[dotn] = x;
        doty[dotn] = y;
        dotn++;
        return 1;
    } else {
        return 0;
    }
}