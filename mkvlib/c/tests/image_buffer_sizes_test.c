#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

bool calculate_image_buffer_sizes(int width, int height,
                                  size_t *pixel_count,
                                  size_t *rgba_buffer_size);

int main(void)
{
    size_t pixel_count = 0;
    size_t rgba_buffer_size = 0;

    assert(calculate_image_buffer_sizes(1920, 1080,
                                        &pixel_count, &rgba_buffer_size));
    assert(pixel_count == 2073600);
    assert(rgba_buffer_size == 8294432);

    assert(!calculate_image_buffer_sizes(65535, 4096,
                                         &pixel_count, &rgba_buffer_size));
    assert(!calculate_image_buffer_sizes(7, 1080,
                                         &pixel_count, &rgba_buffer_size));
    assert(!calculate_image_buffer_sizes(1920, 65536,
                                         &pixel_count, &rgba_buffer_size));

    puts("image_buffer_sizes_test: PASS");
    return 0;
}
