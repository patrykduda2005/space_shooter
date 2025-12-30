#include "resources.h"
#include "raylib.h"
Entity *resources = new Entity();

void initResources() {
    resources->add_component<WorldBorder>({
            .x = 0,
            .y = 0,
            .width = 1000,
            .height = 800,
            });
}

void initSoundResources(){
    resources->add_component<SoundResources>({
        .shootingsfx = LoadSound("resources/shoot.wav"),
    });
}

void initBulletTexture(){
    resources->add_component<BulletTexture>({
        .bull = LoadTexture("resources/surowka.png"),
    });
}

void unLoadResources(){
    auto soundComp = resources->get_component<SoundResources>();
    if(soundComp){
        UnloadSound(soundComp->shootingsfx);
    }
    auto bulletTexComp = resources->get_component<BulletTexture>();
    if(bulletTexComp){
        UnloadTexture(bulletTexComp->bull);
    }
}

