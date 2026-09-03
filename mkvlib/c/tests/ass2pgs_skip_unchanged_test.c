#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static size_t full_frame_clear_count;

static void *tracking_memset(void *dest, int value, size_t size);

#define memset tracking_memset
#include "../ass2pgs.c"
#undef memset

static void *tracking_memset(void *dest, int value, size_t size)
{
    if (size == 1920u * 1080u * 4u)
        full_frame_clear_count++;
    return memset(dest, value, size);
}

static uint32_t read_be32(const uint8_t *p)
{
    return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) |
           ((uint32_t)p[2] << 8) | p[3];
}

static uint16_t read_be16(const uint8_t *p)
{
    return (uint16_t)((p[0] << 8) | p[1]);
}

static bool has_expected_end_timestamp(const char *filename)
{
    FILE *fh = fopen(filename, "rb");
    uint8_t header[13];
    bool found = false;

    if (fh == NULL)
        return false;
    while (fread(header, sizeof(header), 1, fh) == 1) {
        uint16_t length = read_be16(header + 11);

        if (header[0] != 'P' || header[1] != 'G')
            break;
        if (header[10] == 0x16 && length == 11) {
            found = read_be32(header + 2) == 446250;
            break;
        }
        if (fseek(fh, length, SEEK_CUR) != 0)
            break;
    }
    fclose(fh);
    return found;
}

int main(void)
{
    static const char ass[] =
        "[Script Info]\n"
        "ScriptType: v4.00+\n"
        "PlayResX: 1920\n"
        "PlayResY: 1080\n"
        "\n"
        "[V4+ Styles]\n"
        "Format: Name, Fontname, Fontsize, PrimaryColour, SecondaryColour, "
        "OutlineColour, BackColour, Bold, Italic, Underline, StrikeOut, "
        "ScaleX, ScaleY, Spacing, Angle, BorderStyle, Outline, Shadow, "
        "Alignment, MarginL, MarginR, MarginV, Encoding\n"
        "Style: Default,Arial,64,&H00FFFFFF,&H000000FF,&H00000000,"
        "&H00000000,0,0,0,0,100,100,0,0,1,3,0,2,20,20,40,1\n"
        "\n"
        "[Events]\n"
        "Format: Layer, Start, End, Style, Name, MarginL, MarginR, MarginV, "
        "Effect, Text\n"
        "Dialogue: 0,0:00:00.00,0:00:05.00,Default,,0,0,0,,Static\n";
    char ass_path[] = "/tmp/mkvautosubset-skip-ass-XXXXXX";
    char sup_path[] = "/tmp/mkvautosubset-skip-sup-XXXXXX.sup";
    int ass_fd = mkstemp(ass_path);
    int sup_fd = mkstemps(sup_path, 4);
    FILE *ass_fh;

    if (ass_fd < 0 || sup_fd < 0)
        return 2;
    close(sup_fd);
    ass_fh = fdopen(ass_fd, "wb");
    if (ass_fh == NULL || fwrite(ass, strlen(ass), 1, ass_fh) != 1 ||
        fclose(ass_fh) != 0)
        return 2;

    if (!ass2pgs(ass_path, "1080p", "24", ".", sup_path)) {
        unlink(ass_path);
        unlink(sup_path);
        return 2;
    }
    if (full_frame_clear_count != 1) {
        fprintf(stderr, "expected 1 full-frame clear, got %zu\n",
                full_frame_clear_count);
        unlink(ass_path);
        unlink(sup_path);
        return 1;
    }
    if (!has_expected_end_timestamp(sup_path)) {
        fprintf(stderr, "unexpected SUP end timestamp\n");
        unlink(ass_path);
        unlink(sup_path);
        return 1;
    }

    unlink(ass_path);
    unlink(sup_path);
    puts("ass2pgs_skip_unchanged_test: PASS");
    return 0;
}
