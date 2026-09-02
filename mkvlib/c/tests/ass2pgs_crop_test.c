#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

bool ass2pgs(char *ass, const char *resolution, const char *rate,
             char *fontdir, char *output);

static uint16_t read_be16(const uint8_t *p)
{
    return (uint16_t)((p[0] << 8) | p[1]);
}

static void write_ass(FILE *fh)
{
    const char *ass =
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
        "Dialogue: 0,0:00:00.00,0:00:01.00,Default,,0,0,0,,Crop test\n";

    assert(fwrite(ass, strlen(ass), 1, fh) == 1);
}

static void inspect_sup(const char *filename)
{
    FILE *fh = fopen(filename, "rb");
    uint8_t header[13];
    bool saw_pcs = false;
    bool saw_wds = false;
    bool saw_ods = false;

    assert(fh != NULL);
    while (fread(header, sizeof(header), 1, fh) == 1) {
        uint16_t length;
        uint8_t *payload;

        assert(header[0] == 'P' && header[1] == 'G');
        length = read_be16(header + 11);
        payload = malloc(length ? length : 1);
        assert(payload != NULL);
        assert(length == 0 || fread(payload, length, 1, fh) == 1);

        if (!saw_pcs && header[10] == 0x16 && length >= 19 && payload[10] > 0) {
            assert(read_be16(payload) == 1920);
            assert(read_be16(payload + 2) == 1080);
            assert(read_be16(payload + 15) > 0);
            assert(read_be16(payload + 17) > 0);
            saw_pcs = true;
        } else if (!saw_wds && header[10] == 0x17 && length >= 10 && payload[0] > 0) {
            assert(read_be16(payload + 2) > 0);
            assert(read_be16(payload + 4) > 0);
            assert(read_be16(payload + 6) < 1920);
            assert(read_be16(payload + 8) < 1080);
            saw_wds = true;
        } else if (!saw_ods && header[10] == 0x15 && length >= 11) {
            assert(read_be16(payload + 7) < 1920);
            assert(read_be16(payload + 9) < 1080);
            saw_ods = true;
        }

        free(payload);
    }
    fclose(fh);

    assert(saw_pcs);
    assert(saw_wds);
    assert(saw_ods);
}

int main(void)
{
    char ass_path[] = "/tmp/mkvautosubset-ass-XXXXXX";
    char sup_path[] = "/tmp/mkvautosubset-sup-XXXXXX.sup";
    int ass_fd = mkstemp(ass_path);
    int sup_fd = mkstemps(sup_path, 4);
    FILE *ass_fh;

    assert(ass_fd >= 0);
    assert(sup_fd >= 0);
    assert(close(sup_fd) == 0);
    ass_fh = fdopen(ass_fd, "wb");
    assert(ass_fh != NULL);
    write_ass(ass_fh);
    assert(fclose(ass_fh) == 0);

    assert(ass2pgs(ass_path, "1080p", "24", ".", sup_path));
    inspect_sup(sup_path);

    assert(unlink(ass_path) == 0);
    assert(unlink(sup_path) == 0);
    puts("ass2pgs_crop_test: PASS");
    return 0;
}
