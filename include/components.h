#pragma once

#include "ecs.h"
#include "raylib.h"
typedef struct {
    float g;
} Gravity;
void updateGravity();

typedef struct {
    float x;
    float y;
} Position;

typedef struct {
    Texture txt;
} Render;
void renderThings();
