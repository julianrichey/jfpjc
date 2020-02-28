/**
 * Copyright February 2020, John Mamish
 */


#include <math.h>
#include <pam.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "jmcujc.h"
#include "jmcujc_image_util.h"

#include "util.h"

static jmcujc_bytearray_t* jmcujc_bytearray_create(int size)
{
    jmcujc_bytearray_t* ret = calloc(1, sizeof(jmcujc_bytearray_t));
    ret->base               = calloc(1, size);
    ret->index              = 0;

    return ret;
}

static void jmcujc_bytearray_destroy(jmcujc_bytearray_t* bytearray)
{
    free(bytearray->base);
    free(bytearray);
}

#if 0
static jmcujc_component_t* jmcujc_component_create(int width, int height)
{
    jmcujc_component_t* component = calloc(1, sizeof(jmcujc_component_t));
    const int size = width * height;
    component->samples = calloc(size, sizeof(float));
    component->width = width;
    component->height = height;
    component->subsampling_factors.horizontal_sampling_factor = 1;
    component->subsampling_factors.vertical_sampling_factor = 1;
    component->_dirty = true;
}
#endif

int main(int argc, char** argv)
{
    if (argc != 3) {
        printf("Usage: %s <image name> <output file name>\r\n", argv[0]);
        return -1;
    }

    jmcujc_source_image_slice_t* image_slice = grayscale_source_image_from_pam(argv[1], argv[0]);


    jmcujc_bytearray_t* data = jmcujc_bytearray_create((1 << 18));
    jmcujc_component_t component;
    const int component_size = image_slice->width * image_slice->height;
    float* component_buf = calloc(component_size, sizeof(float));
    jmcujc_component_initialize_from_source_image_slice(&component,
                                                        image_slice,
                                                        component_buf,
                                                        0,
                                                        image_slice->height);

    jmcujc_jpeg_params_t bw_params;
    memcpy(&bw_params, &bw_defaults, sizeof(bw_params));
    bw_params.width = image_slice->width;
    bw_params.height = image_slice->height;
    jmcujc_write_headers(&component, 1, &bw_params, data);


    FILE* outfile = fopen(argv[2], "wb");
    if (outfile == NULL) {
        printf("failed to open %s for writing\n", argv[2]);
        return -1;
    }
    printf("writing %i bytes to file %s... ", data->index, argv[2]);
    if (fwrite(data->base, data->index, 1, outfile) != 1) {
        printf("failed.\n");
        return -1;
    } else {
        printf("done.\n");
    }

    fclose(outfile);




    jmcujc_bytearray_destroy(data);

    return -1;
}
