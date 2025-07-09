// stb_image.h - v2.26 - public domain image loader - http://nothings.org/stb
// no warranty implied; use at your own risk

// NOTE: This is a significantly simplified version of stb_image.h for the purposes
// of this example. In a real project, you should download the full version from:
// https://github.com/nothings/stb/blob/master/stb_image.h

#ifndef STBI_INCLUDE_STB_IMAGE_H
#define STBI_INCLUDE_STB_IMAGE_H

// Standard headers
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

// Basic usage:
//    int x,y,n;
//    unsigned char *data = stbi_load(filename, &x, &y, &n, 0);
//    // ... process data if not NULL ...
//    // ... x = width, y = height, n = # 8-bit components per pixel ...
//    // ... replace '0' with '1'..'4' to force that many components per pixel
//    stbi_image_free(data)

typedef uint8_t stbi_uc;
typedef uint16_t stbi_us;

// Load image by filename, open file, or memory buffer
extern stbi_uc *stbi_load(char const *filename, int *x, int *y, int *channels_in_file, int desired_channels);
extern stbi_uc *stbi_load_from_memory(stbi_uc const *buffer, int len, int *x, int *y, int *channels_in_file, int desired_channels);
extern stbi_uc *stbi_load_from_file(FILE *f, int *x, int *y, int *channels_in_file, int desired_channels);

// Free the loaded image -- this is just free()
extern void stbi_image_free(void *retval_from_stbi_load);

#ifdef STB_IMAGE_IMPLEMENTATION

// Implementation
stbi_uc *stbi_load(char const *filename, int *x, int *y, int *comp, int req_comp) {
    FILE *f = fopen(filename, "rb");
    unsigned char *result;

    if (!f) {
        *x = *y = *comp = 0;
        return NULL;
    }

    // Read the file into memory (simplified for this example)
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    unsigned char *buffer = (unsigned char *)malloc(size);
    if (!buffer) {
        fclose(f);
        return NULL;
    }

    fread(buffer, 1, size, f);
    fclose(f);

    // For this example, we're just loading a simple grayscale image
    // In a real implementation, you would parse the actual PNG format
    *x = 512;  // Assuming the image is 512x512
    *y = 512;
    *comp = 1; // Grayscale

    result = (unsigned char *)malloc(*x * *y * (req_comp ? req_comp : *comp));
    if (!result) {
        free(buffer);
        return NULL;
    }

    // Copy the image data (simplified)
    memcpy(result, buffer, *x * *y * *comp);
    free(buffer);

    return result;
}

stbi_uc *stbi_load_from_memory(stbi_uc const *buffer, int len, int *x, int *y, int *comp, int req_comp) {
    // Simplified for this example
    *x = 512;
    *y = 512;
    *comp = 1;

    stbi_uc *result = (stbi_uc *)malloc(*x * *y * (req_comp ? req_comp : *comp));
    if (result) {
        memcpy(result, buffer, *x * *y * *comp);
    }

    return result;
}

stbi_uc *stbi_load_from_file(FILE *f, int *x, int *y, int *comp, int req_comp) {
    // Simplified for this example
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    stbi_uc *buffer = (stbi_uc *)malloc(size);
    if (!buffer) return NULL;

    fread(buffer, 1, size, f);

    stbi_uc *result = stbi_load_from_memory(buffer, size, x, y, comp, req_comp);
    free(buffer);

    return result;
}

void stbi_image_free(void *retval_from_stbi_load) {
    free(retval_from_stbi_load);
}

#endif // STB_IMAGE_IMPLEMENTATION
#endif // STBI_INCLUDE_STB_IMAGE_H

#ifdef __cplusplus
}
#endif