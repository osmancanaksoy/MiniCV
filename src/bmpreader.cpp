#include "bmpreader.h"
#include "bmpheader.h"
#include <fstream>
#include <iostream>

using namespace std;

BMPReader::BMPReader() {}

BMPReader::~BMPReader() {}

Mat BMPReader::read(const string &filename) {

    ifstream file;
    file.open(filename, ios::binary);

    if (!file.is_open()) {

        cerr << "File could not be opened..! (" << filename << ")" << endl;

        system("PAUSE");

        exit(EXIT_FAILURE);
    }

    BMPHeader header;
    file.read((char *)header.p_data, BMPHeader::header_size);
    header.printInfo();

    Mat data(header.getHeight(), header.getWidth());

    int padding = 4 - (3 * header.getWidth()) % 4;

    if (padding == 4) {
        padding = 0;
    }

    for (int j = header.getHeight() - 1; j >= 0; j--) {
        for (int i = 0; i < header.getWidth(); i++) {
            file.read((char *)&data.pixels[j][i], 3);
        }
        file.seekg(padding, ios::cur);
    }

    file.close();

    return data;
}
