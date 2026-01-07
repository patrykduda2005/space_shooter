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
}

void initAmmoCounter(){
    resources->add_component<AmmoCounter>({
        .maxAmmo = {0, 15, 15},
        .currentAmmo = {0, 15, 15},
    });
}



