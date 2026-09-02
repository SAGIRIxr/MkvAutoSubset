#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    int i_width;
    int i_height;
    int i_fps_den;
    int i_fps_num;
} stream_info_t;

void zero_transparent(stream_info_t *s_info, char *img);

int main(void)
{
    stream_info_t info = {3, 1, 0, 0};
    uint8_t storage[20];
    size_t aligned_offset = (4 - ((uintptr_t)storage & 3)) & 3;
    uint8_t *pixels = storage + aligned_offset + 1;
    const uint8_t input[] = {
        11, 22, 33, 0,
        44, 55, 66, 255,
        77, 88, 99, 0,
    };
    const uint8_t expected[] = {
        0, 0, 0, 0,
        44, 55, 66, 255,
        0, 0, 0, 0,
    };

    memcpy(pixels, input, sizeof(input));
    zero_transparent(&info, (char *)pixels);
    for (size_t i = 0; i < sizeof(input); i++)
        assert(pixels[i] == expected[i]);

    puts("zero_transparent_test: PASS");
    return 0;
}
