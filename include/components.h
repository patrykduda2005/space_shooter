#pragma once

#include "ecs.h"
#include "raylib.h"
#include "utils.h"
#include <cstdint>

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
void shoot(int tab);//, int *ammoPointer);

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

enum HitboxLayer {
    Nothing = 0,
    Players = (1 << 31),
    Bullets = (1 << 30),
    Enemies = (1 << 29),
};

typedef struct {
    std::int32_t layer;
    std::int32_t interactsWith;
    Area collisionBox; // Cordinates are RELATIVE to entity owning that collisionBox, NOT absolute.
    std::vector<ComponentHandle> receives;
    std::vector<ComponentHandle> applies;
} Hitbox;
void detectCollision();
void outlineColliders();

typedef struct {
} Destroy;
void destroy();

typedef struct {
    int hp;
} Hp;
typedef struct {
    int dmg;
} Damage;
typedef struct {
    bool global;
    Vec2 vec;
} HpOffset;
void damage();
void displayhp();
void die();

void ammoCounter(int type); //, int *ammoPointer);

const char* GetKeyText(int key);

typedef struct {
    std::vector<Entity*> comps;
} Spawn;
void spawn();

typedef struct {
    std::vector<ComponentHandle> comps;
    double timestamp;
    double delay;
} Delay;
void delay();

typedef struct {
    int g;
} BlackHole;
void suckToBlack(float d);
