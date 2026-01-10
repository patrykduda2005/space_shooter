#include "resources.h"
#include "raylib.h"
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
    resources->add_component<KeyBinds>({
        .up = KEY_W,
        .down = KEY_S,
        .left = KEY_A,
        .right = KEY_D,
        .shoot = KEY_SPACE,
        .type_shoot1 = KEY_ONE,
        .type_shoot2 = KEY_TWO,
        .type_shoot3 = KEY_THREE,
    });
}
/*
void initBulletTexture(){
    resources->add_component<BulletTexture>({
        .bull = LoadTexture("resources/surowka.png"),
    });
}
*/
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

