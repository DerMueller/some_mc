#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#define W 1500
#define H 800

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


#define TARGET_FPS 60

unsigned int dotn = 1;
unsigned int dotit = 0;

char keepGen = 0;

void regenTexture();
char regFreePixel(Image, int, int);
void relTex();

Texture2D tex;
Image img;
Image img2;

int main(void) {

    InitWindow(W, H, "noise test");

    img = GenImageColor(W, H, WHITE);

    ImageDrawPixel(&img, W/2, H/2, BLACK);



    // regenTexture();
    relTex();


    int frame_keydown = 0;

    SetTargetFPS(TARGET_FPS);  

    IsKeyReleased(KEY_SPACE); 

    while (!WindowShouldClose()) {
        if (frame_keydown > TARGET_FPS/2) {
            frame_keydown = 0;
        }
        if (IsKeyPressed(KEY_R)) {
            // reset
            img = GenImageColor(W, H, WHITE);
            relTex();
        }       

        if (IsKeyPressed(KEY_SPACE)) {
            keepGen = 1;
        }
        if (keepGen) {
            regenTexture();
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            int x = GetMouseX();
            int y = GetMouseY();
            ImageDrawPixel(&img, x, y, BLACK);
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

void relTex() {
    UnloadTexture(tex);
    tex = LoadTextureFromImage(img);
}

void regenTexture() {
    // UnloadTexture(tex);
    img2 = ImageCopy(img);
    Color px;
    dotit++;
    dotit++;
    if (dotit > 360) dotit = 1;
    char disable = 1;

    Color itcol = hslToRgb(dotit/360.0f, 1.0f, 0.5f);
    printf("Iterat col: %d %d %d\n", itcol.r, itcol.g, itcol.b);

    // iterate image
    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            px = GetImageColor(img2, x, y);
            if (px.r == 0 && px.g == 0 && px.b == 0) {
                // target pixel
                disable = 0;
                ImageDrawPixel(&img, x, y, itcol);
                
                regFreePixel(img, x-1, y-2);
                regFreePixel(img, x+1, y-2);
                regFreePixel(img, x-1, y+2);
                regFreePixel(img, x+1, y+2);

                regFreePixel(img, x-2, y-1);
                regFreePixel(img, x-2, y+1);
                regFreePixel(img, x+2, y-1);
                regFreePixel(img, x+2, y+1);
            }


        }
    }
    if (disable) {
        keepGen = 0;
    }


    // int dotn_local = dotn;
    // dotn = 0;
    // dotit++;

    // for (int n = 0; n < dotn_local; n++) {

    //     regFreePixel(img, x-1, y-2);
    //     regFreePixel(img, x+1, y-2);
    //     regFreePixel(img, x-1, y+2);
    //     regFreePixel(img, x+1, y+2);

    //     regFreePixel(img, x-2, y-1);
    //     regFreePixel(img, x-2, y+1);
    //     regFreePixel(img, x+2, y-1);
    //     regFreePixel(img, x+2, y+1);

    //     ImageDrawPixel(&img, x, y, (Color){ dotit*10, dotit*10, dotit*10, 255 });
    // }

    // printf("dotn: %d\n", dotn);


    // for (int y = 0; y < H; y++) {
    //     for (int x = 0; x < W; x++) {
    //         unsigned char v = (unsigned char)(255 * n);
    //         ImageDrawPixel(&img, x, y, (Color){ v, v, v, 255 });
    //     }
    // }
    // tex = LoadTextureFromImage(img);
    // UnloadImage(img);
    relTex();
}

Color regPx;
char regFreePixel(Image img, int x, int y) {
    // printf("GetImageColor(img, %d, %d).r = %d\n",x,y, GetImageColor(img, x, y).r);
    if (x < 0 || x >= W || y < 0 || y >= H) return 0;
    regPx = GetImageColor(img, x, y);
    if (regPx.r == 255 && regPx.g == 255 && regPx.b == 255) {
        ImageDrawPixel(&img, x, y, BLACK);
        return 1;
    } else {
        return 0;
    }
}