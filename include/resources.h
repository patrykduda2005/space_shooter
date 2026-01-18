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
    Texture2D shopTexture;
    Texture2D blackhole;
    Texture2D sniper_bullet;
    Texture2D shater;
    Texture2D gameover;
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
    KeyboardKey type_shoot4;
    KeyboardKey type_shoot5;
} KeyBinds;
void initKeyBinds();

void unLoadResources();

typedef struct {
    //int maxAmmo[4];
    int currentAmmo[4];
} AmmoCounter;
void initAmmoCounter();

typedef struct{
    Music backgroundMusic;
    Music menuMusic;
} MusicResources;
void initMusicResources();

typedef struct {
    int fps_index;// = 1;
	int volume; // = 0.5f;
	int sfx_volume; // = 0.5f;
} SettingsComponent;
void initSettingsComponent();

typedef struct{
    int money;
} Money;


extern Entity *resources;
