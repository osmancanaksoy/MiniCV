#include "imio.h"

Mat imread(const string& path) {

    return BMPReader::read(path);
}

void imwrite(const string& path, Mat& mat) {

    BMPWriter::write(path, mat);
}
