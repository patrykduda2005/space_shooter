#ifndef PRIVAT_ECS_H_
#define PRIVAT_ECS_H_

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

#endif
