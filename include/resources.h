#pragma once
#include "ecs.h"
#include "raylib.h"

typedef struct {
    float x;
    float y;
    float width;
    float height;
} WorldBorder;

void initResources();

typedef struct {
    Sound shootingsfx;
} SoundResources;
void initSoundResources();

typedef struct {
    Texture2D bull;
}BulletTexture;
void initBulletTexture();

void unLoadResources();

typedef struct {
    int maxAmmo[3];
    int currentAmmo[3];
} AmmoCounter;
void initAmmoCounter();

extern Entity *resources;
