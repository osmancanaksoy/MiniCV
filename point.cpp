#include "point.h"

Point::Point() {}

Point::Point(int x, int y) {
    this->x = x;
    this->y = y;
}

Point::~Point() {}

void Point::set_x_point(int x) {
    this->x = x;
}

int Point::get_x_point() {
    return x;
}

void Point::set_y_point(int y) {
    this->y = y;
}

int Point::get_y_point() {
    return y;
}
