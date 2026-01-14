#pragma once
#include <ostream>
enum CONTAINS {
    CONTAINS = 0,
    OUTSIDE_UP,
    OUTSIDE_RIGHT,
    OUTSIDE_DOWN,
    OUTSIDE_LEFT
};

class Vec2 {
    public:
    float x;
    float y;
    Vec2();
    Vec2(float x, float y);
    Vec2 operator+(Vec2 rhs);
    Vec2 operator-(Vec2 rhs);
    Vec2 operator*(float rhs);
    friend std::ostream& operator<<(std::ostream& os, Vec2 v);
};

class Area {
    public:
    Vec2 left_up_corner;
    Vec2 right_down_corner;
    Area(Vec2 left_up_corner, Vec2 right_down_corner);
    Area operator+(Vec2 new_reference_point);
    Area operator-(Vec2 new_reference_point);
    friend std::ostream& operator<<(std::ostream& os, Area a);
    Vec2 get_middle();
    float getWidth();
    float getHeight();
    enum CONTAINS contains(Vec2 v);
    bool overlaps(Area a);
};

