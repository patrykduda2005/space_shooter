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
Area Area::operator+(Vec2 new_reference_point) {
    Area copy = Area(this->left_up_corner, this->right_down_corner);
    copy.left_up_corner = copy.left_up_corner + new_reference_point;
    copy.right_down_corner = copy.right_down_corner + new_reference_point;
    return copy;
}

// Bazujac na: https://www.youtube.com/watch?v=TwW2Nl45FvE
std::ostream& operator<<(std::ostream& os, Area a) {
    os << "Area: (" << a.left_up_corner.x << ", " << a.left_up_corner.y << "), ("
        << a.right_down_corner.x << ", " << a.right_down_corner.y << ")";
    return os;
}

Vec2 Area::get_middle() {
    float width = this->getWidth();
    float height = this->getHeight();
    return Vec2(
            this->right_down_corner.x - (width/2),
            this->right_down_corner.y - (height/2)
            );
}

float Area::getWidth() {
    return this->right_down_corner.x - this->left_up_corner.x;
}
float Area::getHeight() {
    return this->right_down_corner.y - this->left_up_corner.y;
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
    if (this->right_down_corner.y < a.left_up_corner.y ||
            a.right_down_corner.y < this->left_up_corner.y)
        return false;
    return true;
}
