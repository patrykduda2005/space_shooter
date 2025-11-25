#include "components.h"
#include <typeinfo>

//
comp_id_t Registry::get_id(const char* name) {
    bool isseen = false;
    int i = 0;
    for (; i < this->seen.size(); i++) {
        if (this->seen[i] == name) {
            isseen = true;
            break;
        }
    }
    if (isseen) return i;
    else {
        this->seen.push_back(name);
        return this->seen.size() - 1;
    }
}
//

using entity_id_t = uint16_t;

static Registry* registry = new Registry();

template<typename T>
void Entity::add_component(T comp) {
    comp_id_t comp_id = registry->get_id(typeid(comp).name());
    this->components.push_back({.id = comp_id, .comp = new Component<T>(comp)});
}

template<typename T>
T* Entity::get_component() {
    comp_id_t comp_id = registry->get_id(typeid(T).name());
    for (int i = 0; i < this->components.size(); i++) {
        if (this->components[i].id == comp_id) 
            return (T*)this->components[i].comp;
    }
    return NULL;
}

Entity* Entities::new_entity() {
    Entity* ent = new Entity();
    this->ents.push_back(ent);
    return ent;
}

