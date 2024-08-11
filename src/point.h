#ifndef POINT_H
#define POINT_H
#include <cstdint>
class Point {

public:
    int x;
    int y;

    Point();
    Point(int x, int y);
    ~Point();
    void set_x_point(int x);
    int get_x_point();
    void set_y_point(int y);
    int get_y_point();
};

#endif

