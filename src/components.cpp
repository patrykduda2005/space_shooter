#include "privat_ecs.h"
#include "ecs.h"

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


