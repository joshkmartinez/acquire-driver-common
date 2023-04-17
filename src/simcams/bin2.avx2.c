#ifdef __AVX2__
#include <immintrin.h>
#include <stdint.h>

#define LANES (32)
#define CEIL_BLOCKS(n) ((n + LANES - 1) / LANES)
#define FLOOR_BLOCKS(n) (n / LANES)

static void
bin2(uint8_t* im_, int w, int h)
{
    __m256i* const im = (__m256i*)im_;
    const int dy = w / LANES;

    for (int y = 0; y < h / 2; ++y) {
        __m256i* const row = im + 2 * y * dy;
        for (int x = 0; x < CEIL_BLOCKS(w); ++x) {
            __m256i* const col = row + x;
            im[x + y * dy] = _mm256_avg_epu8(col[0], col[dy]);
        }
    }

    const __m256i mask = _mm256_set1_epi16(0x00ff);
    for (int x = 0; x < FLOOR_BLOCKS(w * h / 2); ++x) {
        const __m256i b = _mm256_srli_epi16(im[x], 8);
        const __m256i v = _mm256_avg_epu8(im[x], b);
        im[x] = _mm256_and_si256(v, mask);
    }
    for (int x = 0; x < FLOOR_BLOCKS(w * h / 4); ++x) {
        const __m256i v = _mm256_packus_epi16(im[2 * x], im[2 * x + 1]);
        im[x] = _mm256_permute4x64_epi64(v, (3 << 6) | (1 << 4) | (2 << 2));
    }
}
#endif

/*
AVX2 2x2 averaging of an image with size (w,h).

This happens in three phases:
(endian is ignored below)

1. Rows are averaged together in blocks of size LANES.
   The result is stored in the first h/2 rows.
2. Columns are averaged together (now in the first h/2 rows).
   This is done by reading in a block, shifting it left by 1 lane and averaging.
   Something like:

        [x(1) x(2) x(3) 0]
      + [x(0) x(1) x(2) x(3)]

3. The last step leaves the averages in the even lanes.
   Now we iterate through blocks, packing to get rid of unused lanes and
   combining blocks. The main trick here is to use the packus instruction
   which packs 32 16-bit lanes into 32 8-bit lanes - that requires masking
   off the odd lanes from the previous step.

   Looking at the two blocks involved:
   (masked elements from the previous step are set to 0, x's have partial avg)

                block 0               block 1
        pack    [x(0)    0 x(2)    0] [x(4) 0 x(6) 0]
              = [x(0) x(2) x(4) x(6)] [x(4) 0 x(6) 0]

   The pack interleaves results from each block in 64-bit chunks.
   The final permutation fixes the order of the output bytes.

        before [x(0:7) y(0:7)  x(8:15) y(8:15)]
        after  [x(0:7) x(8:15) y(0:7)  y(8:15)]
*/
