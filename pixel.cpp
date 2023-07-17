#include "pixel.h"


Pixel::Pixel() {}

Pixel::Pixel(const uint8_t& B, const uint8_t& G, const uint8_t& R) {
    this->B = B;
    this->G = G;
    this->R = R;
}

Pixel::~Pixel() {}
