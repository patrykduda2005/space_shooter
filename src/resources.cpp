#include "resources.h"
Entity *resources = new Entity();

void initResources() {
    resources->add_component<WorldBorder>({
            .x = 0,
            .y = 0,
            .width = 1000,
            .height = 800,
            });
}
