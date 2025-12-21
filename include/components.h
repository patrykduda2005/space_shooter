#pragma once

#include "ecs.h"
#include "raylib.h"
typedef struct {
    float g;
} Gravity;
void updateGravity(float d);

typedef struct {
    float x;
    float y;
} Position;

typedef struct {
    Texture txt;
} Render;
void renderThings(float d);

typedef struct{
    float cooldown;
} Shooting;
void shoot(int tab, int *ammoPointer, Sound shootingsfx);

typedef struct{
    float x;
    float y;
} Velocity;
void updateVelocity(float d);

typedef struct {
    float northSpeed;
    float eastSpeed;
    float southSpeed;
    float westSpeed;
} ArrowMovement;
void arrowMovement(float d);

typedef struct {
} RestrictToWorld;
void restrictToWorld(float d);

typedef struct {
} DestroyBeyondWorld;
void destroyBeyondWorld();
