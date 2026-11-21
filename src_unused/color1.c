#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define W 400
#define H 400


#define DOTS 107000000
#define TARGET_FPS 60

unsigned int dotn = 1;
unsigned int dotit = 0;

short dotx[DOTS] = {200};
short doty[DOTS] = {200};

short dotx_local[DOTS] = {0};
short doty_local[DOTS] = {0};


void regenTexture(float scale);
char regFreePixel(Image, int, int);

Texture2D tex;
Image img;

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

    // copy dotx to local array
    memcpy(dotx_local, dotx, sizeof(dotx_local));
    memcpy(doty_local, doty, sizeof(doty_local));

    int dotn_local = dotn;
    dotn = 0;
    dotit++;

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

        ImageDrawPixel(&img, x, y, (Color){ dotit*10, dotit*10, dotit*10, 255 });
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