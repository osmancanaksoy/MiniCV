#include "shape.h"


Shape::Shape(QString name) {
    this->name = name;
    invariant.resize(1, 7);
}
