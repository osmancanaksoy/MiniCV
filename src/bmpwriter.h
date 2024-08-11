#ifndef BMP_WRITER_H
#define BMP_WRITER_H

#include "mat.h"
#include "image.h"
#include <string>

using namespace std;

class BMPWriter {

public:

    BMPWriter();
    ~BMPWriter();

    static void write(const string &filename, Mat &mat);
};
#endif // BMP_WRITER_H
