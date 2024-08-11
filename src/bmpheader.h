#ifndef BMP_HEADER_H
#define BMP_HEADER_H

#include <stdint.h>

class BMPHeader {

private:

    struct {
        uint8_t signature[2];
        uint8_t file_size[4];
        uint8_t reserved_1[2];
        uint8_t reserved_2[2];
        uint8_t data_offset[4];
        uint8_t size_of_info_header[4];
        uint8_t image_width[4];
        uint8_t image_height[4];
        uint8_t number_of_planes[2];
        uint8_t bits_per_pixel[2];
        uint8_t compression_type[4];
        uint8_t size_of_image_data[4];
        uint8_t horizontal_resolution[4];
        uint8_t vertical_resolution[4];
        uint8_t number_of_colors[4];
        uint8_t number_of_important_colors[4];
    } data;

public:

    static const uint16_t header_size = 54;
    uint8_t *p_data;

    BMPHeader();
    BMPHeader(const uint32_t &height, const uint32_t &width);
    BMPHeader(const BMPHeader& header);

    const uint32_t &getHeight() const;
    const uint32_t &getWidth() const;

    void printInfo() const;
};
#endif // BMP_HEADER_H
