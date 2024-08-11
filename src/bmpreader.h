#ifndef BMP_READER_H
#define BMP_READER_H

#include "mat.h"
#include "image.h"
#include <string>

using namespace std;

class BMPReader {

public:

    BMPReader();
    ~BMPReader();

    static Mat read(const string &filename);
};
#endif // BMP_READER_H
