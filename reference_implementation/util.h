#ifndef _UTIL_H
#define _UTIL_H

#include <pam.h>

#define PAM_MEMBER_OFFSET(mbrname)                    \
  ((unsigned long int)(char*)&((struct pam *)0)->mbrname)
#define PAM_MEMBER_SIZE(mbrname) \
  sizeof(((struct pam *)0)->mbrname)
#define PAM_STRUCT_SIZE(mbrname) \
(PAM_MEMBER_OFFSET(mbrname) + PAM_MEMBER_SIZE(mbrname))

#define PI (3.14159265358979f)


/**
 * These quantization tables are UNZIGGED!!
 */
const extern int quant_table_high_quality[64];
const extern int quant_table_low_quality[64];

typedef struct image {
    int width;
    int height;
    int depth;

    // supports images with up to 4 components.
    int (*components)[4];
} image_t;


/**
 * Given a
 */
image_t* image_create_from_pam(const char* filepath, char* argv0);
void image_destroy(image_t* image);

/**
 * level-shifts a grayscale image so that its values are signed and centered about 0.
 */
void image_level_shift(image_t* im);

/**
 * rounds up an image to have full MCUs by first duplicating copying the rightmost pixels and then
 * duplicating the bottommost pixels. Because of this duplication order, the bottom right corner is
 * well-defined.
 *
 * Because our encoder operates on 320x320 images, we probably won't implement this function.
 */
void image_pad_out(image_t* im);

/**
 * Allocates a new block and snips out an 8x8 section of the given image, putting it in the newly
 * allocated block.
 */
int* image_copy_mcu(image_t* im, int xstart, int ystart);

/**
 * slow, O(n^2) forward DCT on an 8x8 block.
 *
 * The name doesn't mean that the input and output data are floats, we just use floats as an
 * intermediate representation here.
 *
 * @param[in]     data_in     an 8x8 block of pixels consisting of values in [-128, 127]
 * @param[out]    data_out    an 8x8 block of pixels with values in [-32768, 32767].
 */
void mcu_fdct_floats(int* data_in, int* data_out);

#endif
