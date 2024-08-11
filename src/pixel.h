#ifndef PIXEL_H
#define PIXEL_H

#include <stdint.h>

class Pixel {

public:

    Pixel();
    Pixel(const uint8_t& B, const uint8_t& G, const uint8_t& R);
    ~Pixel();

    uint8_t B;
    uint8_t G;
    uint8_t R;
};

#endif
