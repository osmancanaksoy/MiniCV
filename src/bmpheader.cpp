#include "bmpheader.h"

#include <iostream>

using namespace std;

const uint16_t BMPHeader::header_size;

BMPHeader::BMPHeader() {
    p_data = (uint8_t*)& data;
}
BMPHeader::BMPHeader(const uint32_t& height, const uint32_t& width) {

    p_data = (uint8_t*)&data;

    uint8_t padding = 4 - (3 * width) % 4;

    if (padding == 4) {
        padding = 0;
    }

    uint64_t row_padded = (3 * width) + padding;

    *(uint16_t*)p_data = (uint16_t)19778;
    *(uint32_t*)(p_data + 2) = (uint32_t)(header_size + row_padded * height);
    *(uint16_t*)(p_data + 6) = (uint16_t)0;
    *(uint16_t*)(p_data + 8) = (uint16_t)0;
    *(uint32_t*)(p_data + 10) = (uint32_t)54;
    *(uint32_t*)(p_data + 14) = (uint32_t)40;
    *(uint32_t*)(p_data + 18) = (uint32_t)width;
    *(uint32_t*)(p_data + 22) = (uint32_t)height;
    *(uint16_t*)(p_data + 26) = (uint16_t)1;
    *(uint16_t*)(p_data + 28) = (uint16_t)24;
    *(uint32_t*)(p_data + 30) = (uint32_t)0;
    *(uint32_t*)(p_data + 34) = (uint32_t)(row_padded * height);
    *(uint32_t*)(p_data + 38) = (uint32_t)0;
    *(uint32_t*)(p_data + 42) = (uint32_t)0;
    *(uint32_t*)(p_data + 46) = (uint32_t)0;
    *(uint32_t*)(p_data + 50) = (uint32_t)0;
}

BMPHeader::BMPHeader(const BMPHeader& header) {

    p_data = (uint8_t*)&data;

    data = header.data;
}

const uint32_t& BMPHeader:: getHeight() const {
    return *(uint32_t*)(p_data + 22);
}

const uint32_t& BMPHeader:: getWidth() const {
    return *(uint32_t*)(p_data + 18);
}


void BMPHeader::printInfo() const {

    cout << "Signature: " << *(uint16_t *)p_data << endl;
    cout << "File size: " << *(uint32_t *)(p_data + 2) << " bytes" << endl;
    cout << "Reserved 1: " << *(uint16_t *)(p_data + 6) << endl;
    cout << "Reserved 2: " << *(uint16_t *)(p_data + 8) << endl;
    cout << "Data offset: " << *(uint32_t *)(p_data + 10) << endl;
    cout << "Size of BITMAPINFOHEADER: " << *(uint32_t *)(p_data + 14) << " bytes"
         << endl;
    cout << "Image width: " << *(uint32_t *)(p_data + 18) << " pixels" << endl;
    cout << "Image height: " << *(uint32_t *)(p_data + 22) << " pixels" << endl;
    cout << "Number of planes: " << *(uint16_t *)(p_data + 26) << endl;
    cout << "Number of bits per pixel: " << *(uint16_t *)(p_data + 28) << endl;
    cout << "Compression type: " << *(uint32_t *)(p_data + 30) << endl;
    cout << "Size of image data: " << *(uint32_t *)(p_data + 34) << " bytes"
         << endl;
    cout << "Horizontal resolution (pixels/meter): " << *(uint32_t *)(p_data + 38)
         << endl;
    cout << "Vertical resolution (pixels/meter): " << *(uint32_t *)(p_data + 42)
         << endl;
    cout << "Number of colors: " << *(uint32_t *)(p_data + 46) << endl;
    cout << "Number of important colors: " << *(uint32_t *)(p_data + 50) << endl;
}
