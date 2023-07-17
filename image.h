#ifndef IMAGE_H
#define IMAGE_H

#include "bmpheader.h"
#include "mat.h"

class Image {

public:

    BMPHeader header;
    Mat data;

    Image();
    Image(const BMPHeader& header, const Mat& data);
    ~Image();
};

#endif // IMAGE_H
