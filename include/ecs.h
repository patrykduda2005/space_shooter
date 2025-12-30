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

typedef struct {
    comp_id_t id;
    void *comp; // pointer to component class
} ComponentHandle;

class Entity {
    std::vector<ComponentHandle> components;
    public:
    Entity() {};
    void add_component(ComponentHandle);
    template<typename T>
    void add_component(T comp);
    template<typename T>
    T* get_component();
    template<typename T>
    std::vector<T*> get_components();
    template<typename T>
    void remove_component();
};

class Entities {
    std::vector<Entity*> ents;
    public:
    Entities() {};
    Entity* new_entity();
    std::vector<Entity*> get();
    void kill_entity(Entity* ent);
};


extern Entities *entities;
extern _Registry *registry;

// Template functions implementation
template<typename T>
ComponentHandle create_component(T comp) {
    comp_id_t comp_id = registry->_get_id(typeid(comp).name());
    return {.id = comp_id, .comp = new _Component<T>(comp)};
}


template<typename T>
void Entity::add_component(T comp) {
    this->components.push_back(create_component(comp));
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

template<typename T>
std::vector<T*> Entity::get_components() {
    auto collected_components = std::vector<T*>();
    comp_id_t comp_id = registry->_get_id(typeid(T).name());
    for (int i = 0; i < this->components.size(); i++) {
        if (this->components[i].id == comp_id) 
            collected_components.push_back(
                    (T*)this->components[i].comp
                    );
    }
    return collected_components;
}

template<typename T>
void Entity::remove_component() {
    comp_id_t comp_id = registry->_get_id(typeid(T).name());
    for (int i = 0; i < this->components.size(); i++) {
        if (this->components[i].id == comp_id) {
            ComponentHandle last_comp = this->components[this->components.size() - 1];
            this->components[i] = last_comp;
            this->components.pop_back();
        }
    }
}

