#ifndef SHAPE_H
#define SHAPE_H
#include <iostream>
#include "matrix.h"
#include <QString>

using namespace std;

class Shape {
public:
    QString name;
    int detected_count = 0;;

    Matrix invariant;

    Shape(QString name);

};
#endif //SHAPE_H



