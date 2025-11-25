#include "components.h"

void updateGravity() {
    for (int i = 0; i < entities->get().size(); i++) {
        auto ent = entities->get()[i];
        auto grav = ent->get_component<Gravity>();
        auto pos = ent->get_component<Position>();
        if (pos && grav) {
            pos->y += grav->g;
        }
    }
}

void renderThings() {
    for (int i = 0; i < entities->get().size(); i++) {
        auto ent = entities->get()[i];
        auto ren = ent->get_component<Render>();
        auto pos = ent->get_component<Position>();
        if (ren && pos) {
            DrawTexture(ren->txt, pos->x, pos->y, WHITE);
        }
    }
}

