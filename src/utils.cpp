#include "utils.h"
#include <cassert>

Vec2::Vec2(): x(0.0), y(0.0) {};
Vec2::Vec2(float x, float y): x(x), y(y) {};
Vec2 Vec2::operator+(Vec2 rhs) {
    this->x += rhs.x;
    this->y += rhs.y;
    return *this;
}

Vec2 Vec2::operator-(Vec2 rhs) {
    this->x -= rhs.x;
    this->y -= rhs.y;
    return *this;
}

Area::Area(Vec2 left_up_corner, Vec2 right_down_corner) {
    assert(left_up_corner.x < right_down_corner.x);
    assert(left_up_corner.y < right_down_corner.y);
    this->left_up_corner = left_up_corner;
    this->right_down_corner = right_down_corner;
}

Vec2 Area::get_middle() {
    Vec2 width_and_height = this->right_down_corner - this->left_up_corner;
    float width = width_and_height.x;
    float height = width_and_height.y;
    return Vec2(
            this->right_down_corner.x - (width/2),
            this->right_down_corner.y - (height/2)
            );
}

enum CONTAINS Area::contains(Vec2 v) {
    if (v.x > this->right_down_corner.x) return CONTAINS::OUTSIDE_RIGHT;
    if (v.y > this->right_down_corner.y) return CONTAINS::OUTSIDE_DOWN;
    if (v.x < this->left_up_corner.x) return CONTAINS::OUTSIDE_LEFT;
    if (v.y < this->left_up_corner.y) return CONTAINS::OUTSIDE_UP;
    return CONTAINS::CONTAINS;
}

// Bazujac na: https://www.geeksforgeeks.org/dsa/find-two-rectangles-overlap/
bool Area::overlaps(Area a) {
    if (this->left_up_corner.x > a.right_down_corner.x ||
            a.left_up_corner.x > this->right_down_corner.x)
        return false;
    if (this->right_down_corner.y > a.left_up_corner.y ||
            a.right_down_corner.y > this->left_up_corner.y)
        return false;
    return true;
}
