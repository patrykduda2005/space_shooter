#ifndef ECS_H_
#define ECS_H_
#include <vector>

class Entity {
    std::vector<struct component_wrapper> components;
    public:
    Entity() {};
    template<typename T>
    void add_component(T comp);
    template<typename T>
    T* get_component();
};

class Entities {
    public:
    std::vector<Entity*> ents;
    Entities() {};
    Entity* new_entity();
};

static Entities *entities = new Entities();

#endif
