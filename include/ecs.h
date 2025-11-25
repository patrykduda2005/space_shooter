#pragma once

//
#include "components.h"
#include <cstdint>
#include <vector>

using comp_id_t = uint16_t;
struct component_wrapper {
    comp_id_t id;
    void *comp;
};

class Registry {
    std::vector<const char*> seen;
    public:
    Registry() {};
    comp_id_t get_id(const char* name); 
};

template<typename T>
class Component {
    public:
    T comp;
    Component(T comp): comp(comp) {};
};


//
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
#include "ecs.ipp"
