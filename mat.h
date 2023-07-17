#ifndef MAT_H
#define MAT_H

#include "pixel.h"

class Mat {

private:

    int row;
    int column;

public:

    Pixel** pixels;
    long int* histogram;
    long int* red_histogram;
    long int* green_histogram;
    long int* blue_histogram;

    Mat();
    Mat(int row, int column);
    Mat(const Mat& mat);

    int getHeight() const;
    int getWidth() const;

    Pixel& operator()(const int& row, const int& col);
    const Pixel& operator()(const int& row, const int& col) const;

    Mat& operator=(const Mat image);
};

#endif // MAT_H
