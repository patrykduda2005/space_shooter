#pragma once
#include "ecs.h"

typedef struct {
    float x;
    float y;
    float width;
    float height;
} WorldBorder;

void initResources();

extern Entity *resources;
