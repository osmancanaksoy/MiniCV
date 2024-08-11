#include "bmpwriter.h"
#include "bmpheader.h"
#include <fstream>
#include <iostream>
#include <math.h>

BMPWriter::BMPWriter() {}

BMPWriter::~BMPWriter() {}

void BMPWriter::write(const string &filename, Mat &mat) {

    BMPHeader header(mat.getHeight(), mat.getWidth());
    Image image(header, mat);

    ofstream file(filename, ios::binary);
    file.write((char*)image.header.p_data, BMPHeader::header_size);

    int padding = 4 - (3 * mat.getWidth()) % 4;
    if (padding == 4) {
        padding = 0;
    }

    int padding_value = 0;

    for (int i = mat.getHeight() - 1; i >= 0; i--) {
        for (int j = 0; j < mat.getWidth(); j++) {
            file.write((char*)&image.data.pixels[i][j], 3);
        }
        file.write((char*)&padding_value, (int)padding);
    }
    file.close();
}
