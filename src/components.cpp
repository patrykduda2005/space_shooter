#include "components.h"
#include "resources.h"

void updateGravity(float d) {
    for (int i = 0; i < entities->get().size(); i++) {
        auto ent = entities->get()[i];
        auto grav = ent->get_component<Gravity>();
        auto pos = ent->get_component<Position>();
        if (pos && grav) {
            pos->y += grav->g * d;
        }
    }
}

void renderThings(float d) {
    for (int i = 0; i < entities->get().size(); i++) {
        auto ent = entities->get()[i];
        auto ren = ent->get_component<Render>();
        auto pos = ent->get_component<Position>();
        if (ren && pos) {
            DrawTexture(ren->txt, pos->x, pos->y, WHITE);
        }
    }
}

void arrowMovement(float d) {
    for (int i = 0; i < entities->get().size(); i++) {
        auto ent = entities->get()[i];
        auto arr = ent->get_component<ArrowMovement>();
        auto pos = ent->get_component<Position>();
        if (arr && pos) {
            if (IsKeyDown(KEY_RIGHT)) pos->x += arr->eastSpeed * d;
            if (IsKeyDown(KEY_LEFT)) pos->x -= arr->westSpeed * d;
            if (IsKeyDown(KEY_UP)) pos->y -= arr->northSpeed * d;
            if (IsKeyDown(KEY_DOWN)) pos->y += arr->southSpeed * d; 
        }
    }
}

void restrictToWorld(float d) {
    for (int i = 0; i < entities->get().size(); i++) {
        auto ent = entities->get()[i];
        auto pos = ent->get_component<Position>();
        auto wb = resources->get_component<WorldBorder>();
        if (wb && pos) {
            if (pos->x > wb->x + wb->width) pos->x = wb->x + wb->width;
            if (pos->y > wb->y + wb->height) pos->y = wb->x + wb->height;
            if (pos->x < wb->x) pos->x = wb->x;
            if (pos->y < wb->y) pos->y = wb->y;
        }
    }
}
