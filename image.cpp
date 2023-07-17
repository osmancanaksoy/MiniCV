#include "image.h"

Image::Image() { }

Image::Image(const BMPHeader& header, const Mat& data) {

    this->header = header;
    this->data = data;
}

Image::~Image() { }
