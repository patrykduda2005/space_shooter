#include "ecs.h"

Entities *entities = new Entities();
_Registry* registry = new _Registry();

comp_id_t _Registry::_get_id(const char* name) {
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

Entity* Entities::new_entity() {
    Entity* ent = new Entity();
    this->ents.push_back(ent);
    return ent;
}



std::vector<Entity*> Entities::get() {
    return this->ents;
}
