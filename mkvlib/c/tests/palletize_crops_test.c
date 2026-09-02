#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../ass2bdnxml/palletize.h"

static uint32_t rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    return (uint32_t)r | ((uint32_t)g << 8) |
           ((uint32_t)b << 16) | ((uint32_t)a << 24);
}

int main(void)
{
    enum { width = 6, height = 3 };
    uint32_t image[width * height];
    uint32_t original[width * height];
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
    memcpy(original, image, sizeof(image));
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

    assert(memcmp(image, original, sizeof(image)) == 0);
    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++)
            if (!((y == 0 && x >= 1 && x <= 2) || (y == 2 && x == 4)))
                assert(indexed[y * width + x] == 0xa5);

    for (size_t i = 0; i < 256 && palette[i] != 0xc0decafe; i++)
        assert(palette[i] != outside);

    free(palette);

    {
        uint32_t legacy_image[] = {red, blue};
        uint8_t *legacy_indexed = (uint8_t *)legacy_image;
        uint32_t *legacy_palette = palletize(legacy_indexed, 2, 1);

        assert(legacy_palette != NULL);
        assert(legacy_indexed[0] != 0);
        assert(legacy_indexed[1] != 0);
        assert(legacy_palette[legacy_indexed[0]] == red);
        assert(legacy_palette[legacy_indexed[1]] == blue);
        free(legacy_palette);
    }

    puts("palletize_crops_test: PASS");
    return 0;
}
