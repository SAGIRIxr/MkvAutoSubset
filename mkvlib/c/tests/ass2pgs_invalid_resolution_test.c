#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

bool ass2pgs(char *ass, const char *resolution, const char *rate,
             char *fontdir, char *output);

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
        "Dialogue: 0,0:00:00.00,0:00:01.00,Default,,0,0,0,,Too large\n";
    char ass_path[] = "/tmp/mkvautosubset-invalid-ass-XXXXXX";
    char sup_path[] = "/tmp/mkvautosubset-invalid-sup-XXXXXX.sup";
    int ass_fd = mkstemp(ass_path);
    int sup_fd = mkstemps(sup_path, 4);
    FILE *ass_fh;

    assert(ass_fd >= 0);
    assert(sup_fd >= 0);
    assert(close(sup_fd) == 0);
    ass_fh = fdopen(ass_fd, "wb");
    assert(ass_fh != NULL);
    assert(fwrite(ass, strlen(ass), 1, ass_fh) == 1);
    assert(fclose(ass_fh) == 0);

    assert(!ass2pgs(ass_path, "65535*4096", "24", ".", sup_path));

    assert(unlink(ass_path) == 0);
    assert(unlink(sup_path) == 0);
    puts("ass2pgs_invalid_resolution_test: PASS");
    return 0;
}
