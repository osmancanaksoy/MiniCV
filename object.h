#ifndef OBJECT_H
#define OBJECT_H
#include "mat.h"
#include "matrix.h"
#include "point.h"

class Object {
public:
    int label_id;
    int height;
    int width;

    Point p1;
    Point p2;
    Point p3;
    Point p4;

    //Mat mat;
    long int** matrix;
    Matrix invariants;

    long double centroid_row;
    long double centroid_col;

    Object();
    Object(int label_id, long int** matrix,Point p1,
           Point p2, Point p3, Point p4);
    ~Object();

    long double m(long double p, long double q);
    long double mu(long double p, long double q);
    long double nu(long double p, long double q);
    void calculate_centroid();
    void calculate_invariants();
    long double calculate_invariants_avg();
    void draw_bounding_box(Mat& mat);
    void cut_object(Mat mat);
    void save_invariants();

};

#endif // OBJECT_H


