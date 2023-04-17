#include <stdint.h>
#include <string.h>

static void
bin2(uint8_t* im_, int w, int h)
{
    const uint8_t* end = im_ + w * h;

    // horizontal
    for (uint8_t* row = im_; row < end; row += w) {
        const uint8_t* row_end = im_ + w;
        for (uint8_t* p = row; p < row_end; p += 2) {
            p[0] = (uint8_t)(0.5f * p[0] + 0.5f * p[1]);
        }
        for (uint8_t *p = row, *s = row; s < row_end; ++p, s += 2) {
            *p = *s;
        }
    }

    // vertical
    for (uint8_t* row = im_ + w; row < end; row += 2 * w) {
        const uint8_t* row_end = im_ + 2 * w;
        for (uint8_t* p = row; p < row_end; ++p) {
            p[-w] = (uint8_t)(0.5f * p[-w] + 0.5f * p[0]);
        }
    }
    for (uint8_t *src_row = im_, *dst_row = im_; src_row < end;
         src_row += 2 * w, dst_row += w) {
        memcpy(dst_row, src_row, w);
    }
}
