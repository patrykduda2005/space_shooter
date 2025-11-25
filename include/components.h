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

typedef struct {
    float northSpeed;
    float eastSpeed;
    float southSpeed;
    float westSpeed;
} ArrowMovement;
void arrowMovement(float d);
