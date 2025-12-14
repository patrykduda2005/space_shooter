#pragma once

#include "ecs.h"
#include "raylib.h"
#include "utils.h"
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
void shoot();

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

enum Event {
    Die = (1 << 31),
    Kill = (1 << 30),
};

typedef struct {
    int layer;
    Area collisionBox; // Cordinates are RELATIVE to entity owning that collisionBox, NOT absolute.
    int events;
} Collider;
void detectCollision();
void outlineColliders();
