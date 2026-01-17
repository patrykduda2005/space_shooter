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
    Effects = (1 << 28),
};

typedef struct {
} RemoveHitbox;
typedef struct {
    std::int32_t layer;
    std::int32_t interactsWith;
    Area collisionBox; // Cordinates are RELATIVE to entity owning that collisionBox, NOT absolute.
    std::vector<ComponentHandle> receives;
    std::vector<ComponentHandle> applies;
} Hitbox;
void detectCollision();
void outlineColliders();
void removeHitbox();

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
typedef struct {
} SuckedToBlack;
void suckToBlack(float d);

typedef struct {
    int currentWave;
    int enemiesInWave;      
    int enemiesSpawned;     
    int enemiesKilled;
    bool waveActive;
    float waveTimer;
    float spawnTimer;
} WaveManager;

void updateWaveManager(float d);
void spawnEnemyInWave(int waveNumber);
void spawnBasicEnemy(Position pos);
void spawnTankEnemy(Position pos);
void spawnShootingEnemy(Position pos);
Entity* createEnemyBullet(Position pos);

typedef struct {
    float bounceStrength;
} BounceOffWalls;

void bounceOffWalls();

typedef struct {
    float timer;
} Invulnerable;

void updateInvulnerability(float d);

typedef struct {
} ShootingEnemyTag;

typedef struct {
    float cooldown;
} EnemyShooting;

void updateEnemyShooting(float d);

typedef struct {
    int type;
    int value;
} DropItem;

typedef struct {
    Texture txt;
} ItemRender;

void spawnDropItem(Position pos, int type);
void updateDropItems(float d);
void collectDropItems();

typedef struct {
    int dropType;
} CollectDrop;