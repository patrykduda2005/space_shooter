#pragma once

#include <cstdint>
#include <vector>
#include <typeinfo>
using comp_id_t = uint16_t;
using entity_id_t = uint16_t;

class _Registry {
    std::vector<const char*> seen;
    public:
    _Registry() {};
    comp_id_t _get_id(const char* name); 
};

template<typename T>
class _Component {
    public:
    T comp;
    _Component(T component): comp(component) {};
};

struct _component_wrapper {
    comp_id_t id;
    void *comp;
};

class Entity {
    std::vector<struct _component_wrapper> components;
    public:
    Entity() {};
    template<typename T>
    void add_component(T comp);
    template<typename T>
    T* get_component();
};

class Entities {
    std::vector<Entity*> ents;
    public:
    Entities() {};
    Entity* new_entity();
    std::vector<Entity*> get();
};

extern Entities *entities;
extern _Registry *registry;

// Template functions implementation
template<typename T>
void Entity::add_component(T comp) {
    comp_id_t comp_id = registry->_get_id(typeid(comp).name());
    this->components.push_back({.id = comp_id, .comp = new _Component<T>(comp)});
}

template<typename T>
T* Entity::get_component() {
    comp_id_t comp_id = registry->_get_id(typeid(T).name());
    for (int i = 0; i < this->components.size(); i++) {
        if (this->components[i].id == comp_id) 
            return (T*)this->components[i].comp;
    }
    return NULL;
}


