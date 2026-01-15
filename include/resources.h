#pragma once
#include "ecs.h"
#include "raylib.h"

typedef struct {
    float x;
    float y;
    float width;
    float height;
} WorldBorder;

void initWorldSize();

typedef struct {
    Sound shootingsfx;
    Texture2D bull;
    Texture2D background;
} soundTextureResources;

void initResources();

typedef struct{
    KeyboardKey up;
    KeyboardKey down;
    KeyboardKey left;  
    KeyboardKey right;
    KeyboardKey shoot;
    KeyboardKey type_shoot1;
    KeyboardKey type_shoot2;
    KeyboardKey type_shoot3;
} KeyBinds;
void initKeyBinds();

// typedef struct {
//     Sound shootingsfx;
// } SoundResources;
// //void initSoundResources();

// typedef struct {
//     Texture2D bull;
// }BulletTexture;
// //void initBulletTexture();

void unLoadResources();

typedef struct {
    int maxAmmo[3];
    int currentAmmo[3];
} AmmoCounter;
void initAmmoCounter();

typedef struct{
    Music backgroundMusic;
    Music menuMusic;
} MusicResources;
void initMusicResources();

typedef struct {
    int fps_index;// = 1;
	float volume; // = 0.5f;
	float sfx_volume; // = 0.5f;
} SettingsComponent;
void initSettingsComponent();

extern Entity *resources;
