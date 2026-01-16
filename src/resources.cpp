#include "resources.h"
#include "raylib.h"
#include <fstream>
#include <iostream>
using namespace std;
Entity *resources = new Entity();

void initWorldSize() {
    resources->add_component<WorldBorder>({ //świat gry
            .x = 0,
            .y = 0,
            .width = 1000,
            .height = 800,
            });
}

void initResources(){
    resources->add_component<soundTextureResources>({
        .shootingsfx = LoadSound("resources/shoot.wav"),
        .bull = LoadTexture("resources/surowka.png"),
        .background = LoadTexture("resources/tlo.png"),
    });
}

void initKeyBinds(){
    int up = KEY_W;
    int down = KEY_S;
    int left = KEY_A;
    int right = KEY_D;
    int shoot = KEY_SPACE;
    int type_shoot1 = KEY_ONE;
    int type_shoot2 = KEY_TWO;
    int type_shoot3 = KEY_THREE;

    ifstream ustawienia("resources/settings.txt");
    //ustawienia.open();

    if(ustawienia.is_open()){
        string key;
        while (ustawienia>>key){
            if(key == "KEY_UP") ustawienia>>up;
            if(key == "KEY_DOWN") ustawienia>>down;
            if(key == "KEY_LEFT") ustawienia>>left;
            if(key == "KEY_RIGHT") ustawienia>>right;
            if(key == "KEY_SHOOT") ustawienia>>shoot;
            if(key == "KEY_TYPE_SHOOT1") ustawienia>>type_shoot1;
            if(key == "KEY_TYPE_SHOOT2") ustawienia>>type_shoot2;
            if(key == "KEY_TYPE_SHOOT3") ustawienia>>type_shoot3;
        }
    }

    resources->add_component<KeyBinds>({
        .up = (KeyboardKey)up,
        .down = (KeyboardKey)down,
        .left = (KeyboardKey)left,
        .right = (KeyboardKey)right,
        .shoot = (KeyboardKey)shoot,
        .type_shoot1 = (KeyboardKey)type_shoot1,
        .type_shoot2 = (KeyboardKey)type_shoot2,
        .type_shoot3 = (KeyboardKey)type_shoot3,
    });
}

void unLoadResources(){
    auto soundComp = resources->get_component<soundTextureResources>();
    if(soundComp){
        UnloadSound(soundComp->shootingsfx);
    }
    auto bulletTexComp = resources->get_component<soundTextureResources>();
    if(bulletTexComp){
        UnloadTexture(bulletTexComp->bull);
    }
    auto musicComp = resources->get_component<MusicResources>();
    if(musicComp){
        UnloadMusicStream(musicComp->backgroundMusic);
        UnloadMusicStream(musicComp->menuMusic);
    }

}

void initAmmoCounter(){
    resources->add_component<AmmoCounter>({
        .maxAmmo = {0, 15, 15},
        .currentAmmo = {0, 15, 15},
    });
}

void initMusicResources(){
    resources->add_component<MusicResources>({
        .backgroundMusic = LoadMusicStream("resources/gamemusic.wav"),
        .menuMusic = LoadMusicStream("resources/menumusic.wav"),
    });
}

void initSettingsComponent(){
    int fps_i = 1;
    int vol = 5;//0.5f;
    int sfx_vol = 5;//0.5f;

    ifstream ustawienia("resources/settings.txt");

    //ustawienia.open();

    if(ustawienia.is_open()){
        string key;
        while (ustawienia>>key){
            if(key == "FPS_INDEX") ustawienia>>fps_i;
            if(key == "VOLUME") ustawienia>>vol;
            if(key == "SFX_VOLUME") ustawienia>>sfx_vol;
        }
        ustawienia.close();
    }

    resources->add_component<SettingsComponent>({
        .fps_index = fps_i,
        .volume = vol,
        .sfx_volume = sfx_vol,
    });
}

