#include "mat.h"

Mat::Mat() { }

Mat::Mat(int row, int column) {

    this->row = row;
    this->column = column;

    pixels = new Pixel* [row];

    for (int i = 0; i < row; i++) {

        pixels[i] = new Pixel[column];

    }
}

Mat::Mat(const Mat& mat) {

    row = mat.getHeight();
    column = mat.getWidth();

    pixels = new Pixel * [row];

    for (int i = 0; i < row; i++) {

        pixels[i] = new Pixel[column];
    }

    for (int i = 0; i < row; i++) {
        for (int j = 0; j < column; j++) {

            pixels[i][j] = mat.pixels[i][j];
        }
    }
}

int Mat::getHeight() const {

    return row;
}

int Mat::getWidth() const {

    return column;
}

Pixel& Mat::operator()(const int& row, const int& col) {

    return pixels[row][col];
}

const Pixel& Mat::operator()(const int& row, const int& col) const {

    return pixels[row][col];
}

Mat& Mat::operator=(const Mat image) {

    if (this != &image)
    {
        row = image.row;
        column = image.column;
        pixels           = image.pixels;
        histogram          = image.histogram;
        red_histogram   = image.red_histogram;
        green_histogram    = image.green_histogram;
    }

    return *this;
}
