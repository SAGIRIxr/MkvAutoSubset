#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../ass2bdnxml/auto_split.h"

uint32_t *palletize_crops(const uint8_t *image, uint8_t *indexed,
                          int w, int h, int num_crop,
                          const crop_t *crops);

static uint32_t rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    return (uint32_t)r | ((uint32_t)g << 8) |
           ((uint32_t)b << 16) | ((uint32_t)a << 24);
}

int main(void)
{
    enum { width = 6, height = 3 };
    uint32_t image[width * height];
    uint8_t indexed[width * height];
    crop_t crops[] = {
        {1, 0, 2, 1},
        {4, 2, 1, 1},
    };
    const uint32_t outside = rgba(0, 255, 0, 255);
    const uint32_t red = rgba(255, 0, 0, 255);
    const uint32_t blue = rgba(0, 0, 255, 255);
    uint32_t *palette;
    uint8_t red_index;
    uint8_t blue_index;

    for (size_t i = 0; i < width * height; i++)
        image[i] = outside;
    image[1] = red;
    image[2] = 0;
    image[2 * width + 4] = blue;
    memset(indexed, 0xa5, sizeof(indexed));

    palette = palletize_crops((const uint8_t *)image, indexed,
                              width, height, 2, crops);
    assert(palette != NULL);

    red_index = indexed[1];
    blue_index = indexed[2 * width + 4];
    assert(red_index != 0);
    assert(blue_index != 0);
    assert(red_index != blue_index);
    assert(palette[red_index] == red);
    assert(palette[blue_index] == blue);
    assert(indexed[2] == 0);

    assert(indexed[0] == 0xa5);
    assert(indexed[3] == 0xa5);
    assert(indexed[2 * width + 3] == 0xa5);
    assert(indexed[2 * width + 5] == 0xa5);

    for (size_t i = 0; i < 256 && palette[i] != 0xc0decafe; i++)
        assert(palette[i] != outside);

    free(palette);
    puts("palletize_crops_test: PASS");
    return 0;
}
