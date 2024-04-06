/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * Binary image operations.
 */
#include "imlib.h"

#ifdef IMLIB_ENABLE_BINARY_OPS
void imlib_zero_line_op(int x, int x_end, int y_row, imlib_draw_row_data_t *data) {
    image_t *mask = data->callback_arg;

    switch (data->dst_img->pixfmt) {
        case PIXFORMAT_BINARY: {
            uint32_t *row = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(data->dst_img, y_row);

            if (!mask) {
                for (; x < x_end; x++) {
                    if (IMAGE_GET_BINARY_PIXEL_FAST(((uint32_t *) data->dst_row_override), x)) {
                        IMAGE_CLEAR_BINARY_PIXEL_FAST(row, x);
                    }
                }
            } else {
                for (; x < x_end; x++) {
                    if (image_get_mask_pixel(mask, x, y_row) &&
                        IMAGE_GET_BINARY_PIXEL_FAST(((uint32_t *) data->dst_row_override), x)) {
                        IMAGE_CLEAR_BINARY_PIXEL_FAST(row, x);
                    }
                }
            }
            break;
        }
        case PIXFORMAT_GRAYSCALE: {
            uint8_t *row = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(data->dst_img, y_row);

            if (!mask) {
                for (; x < x_end; x++) {
                    if (IMAGE_GET_GRAYSCALE_PIXEL_FAST(((uint8_t *) data->dst_row_override), x)) {
                        IMAGE_PUT_GRAYSCALE_PIXEL_FAST(row, x, 0);
                    }
                }
            } else {
                for (; x < x_end; x++) {
                    if (image_get_mask_pixel(mask, x, y_row) &&
                        IMAGE_GET_GRAYSCALE_PIXEL_FAST(((uint8_t *) data->dst_row_override), x)) {
                        IMAGE_PUT_GRAYSCALE_PIXEL_FAST(row, x, 0);
                    }
                }
            }
            break;
        }
        case PIXFORMAT_RGB565: {
            uint16_t *row = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(data->dst_img, y_row);

            if (!mask) {
                for (; x < x_end; x++) {
                    if (IMAGE_GET_RGB565_PIXEL_FAST(((uint16_t *) data->dst_row_override), x)) {
                        IMAGE_PUT_RGB565_PIXEL_FAST(row, x, 0);
                    }
                }
            } else {
                for (; x < x_end; x++) {
                    if (image_get_mask_pixel(mask, x, y_row) &&
                        IMAGE_GET_RGB565_PIXEL_FAST(((uint16_t *) data->dst_row_override), x)) {
                        IMAGE_PUT_RGB565_PIXEL_FAST(row, x, 0);
                    }
                }
            }
            break;
        }
        default: {
            break;
        }
    }
}

void imlib_mask_line_op(int x, int x_end, int y_row, imlib_draw_row_data_t *data) {
    image_t *mask = data->callback_arg;

    switch (data->dst_img->pixfmt) {
        case PIXFORMAT_BINARY: {
            uint32_t *row = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(data->dst_img, y_row);
            for (; x < x_end; x++) {
                if (image_get_mask_pixel(mask, x, y_row)) {
                    int otherPixel = IMAGE_GET_BINARY_PIXEL_FAST(((uint32_t *) data->dst_row_override), x);
                    IMAGE_PUT_BINARY_PIXEL_FAST(row, x, otherPixel);
                }
            }
            break;
        }
        case PIXFORMAT_GRAYSCALE: {
            uint8_t *row = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(data->dst_img, y_row);
            for (; x < x_end; x++) {
                if (image_get_mask_pixel(mask, x, y_row)) {
                    int otherPixel = IMAGE_GET_GRAYSCALE_PIXEL_FAST(((uint8_t *) data->dst_row_override), x);
                    IMAGE_PUT_GRAYSCALE_PIXEL_FAST(row, x, otherPixel);
                }
            }
            break;
        }
        case PIXFORMAT_RGB565: {
            uint16_t *row = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(data->dst_img, y_row);
            for (; x < x_end; x++) {
                if (image_get_mask_pixel(mask, x, y_row)) {
                    int otherPixel = IMAGE_GET_RGB565_PIXEL_FAST(((uint16_t *) data->dst_row_override), x);
                    IMAGE_PUT_RGB565_PIXEL_FAST(row, x, otherPixel);
                }
            }
            break;
        }
        default: {
            break;
        }
    }
}

void imlib_binary(image_t *out, image_t *img, list_t *thresholds, bool invert, bool zero, image_t *mask) {
    image_t bmp;
    bmp.w = img->w;
    bmp.h = img->h;
    bmp.pixfmt = PIXFORMAT_BINARY;
    bmp.data = fb_alloc0(image_size(&bmp), FB_ALLOC_NO_HINT);

    list_for_each(it, thresholds) {
        color_thresholds_list_lnk_data_t *lnk_data = list_get_data(it);

        switch (img->pixfmt) {
            case PIXFORMAT_BINARY: {
                for (int y = 0, yy = img->h; y < yy; y++) {
                    uint32_t *old_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img, y);
                    uint32_t *bmp_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(&bmp, y);
                    for (int x = 0, xx = img->w; x < xx; x++) {
                        if (COLOR_THRESHOLD_BINARY(IMAGE_GET_BINARY_PIXEL_FAST(old_row_ptr, x), lnk_data, invert)) {
                            IMAGE_SET_BINARY_PIXEL_FAST(bmp_row_ptr, x);
                        }
                    }
                }
                break;
            }
            case PIXFORMAT_GRAYSCALE: {
                for (int y = 0, yy = img->h; y < yy; y++) {
                    uint8_t *old_row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, y);
                    uint32_t *bmp_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(&bmp, y);
                    for (int x = 0, xx = img->w; x < xx; x++) {
                        if (COLOR_THRESHOLD_GRAYSCALE(IMAGE_GET_GRAYSCALE_PIXEL_FAST(old_row_ptr, x), lnk_data, invert)) {
                            IMAGE_SET_BINARY_PIXEL_FAST(bmp_row_ptr, x);
                        }
                    }
                }
                break;
            }
            case PIXFORMAT_RGB565: {
                for (int y = 0, yy = img->h; y < yy; y++) {
                    uint16_t *old_row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img, y);
                    uint32_t *bmp_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(&bmp, y);
                    for (int x = 0, xx = img->w; x < xx; x++) {
                        if (COLOR_THRESHOLD_RGB565(IMAGE_GET_RGB565_PIXEL_FAST(old_row_ptr, x), lnk_data, invert)) {
                            IMAGE_SET_BINARY_PIXEL_FAST(bmp_row_ptr, x);
                        }
                    }
                }
                break;
            }
            default: {
                break;
            }
        }
    }

    imlib_draw_row_callback_t callback = NULL;
    if (zero) {
        callback = imlib_zero_line_op;
    }

    if ((!callback) && mask) {
        callback = imlib_mask_line_op;
    }

    void *dst_row_override = NULL;
    if (callback) {
        dst_row_override = fb_alloc0(image_line_size(out), FB_ALLOC_CACHE_ALIGN);
    }

    imlib_draw_image(out, &bmp, 0, 0, 1.0f, 1.0f, NULL, -1, 256, NULL, NULL, 0, callback, mask, dst_row_override);

    if (dst_row_override) {
        fb_free(); // dst_row_override
    }

    fb_free(); // bmp.data
}

void imlib_invert(image_t *img) {
    uint32_t n = image_size(img);
    uint32_t *p32 = (uint32_t *) img->data;

    switch (img->pixfmt) {
        case PIXFORMAT_BINARY: {
            for (; n >= 4; n -= 4, p32++) {
                *p32 = ~*p32;
            }
            break;
        }
        case PIXFORMAT_GRAYSCALE: {
            #if (__ARM_ARCH > 6)
            for (; n >= 4; n -= 4, p32++) {
                *p32 = ~*p32;
            }
            #endif

            uint8_t *p8 = (uint8_t *) p32;

            for (; n >= 1; n -= 1, p8++) {
                *p8 = ~*p8;
            }
            break;
        }
        case PIXFORMAT_RGB565: {
            #if (__ARM_ARCH > 6)
            for (; n >= 4; n -= 4, p32++) {
                *p32 = ~*p32;
            }
            #endif

            uint16_t *p16 = (uint16_t *) p32;

            for (; n >= 2; n -= 2, p16++) {
                *p16 = ~*p16;
            }
            break;
        }
        default: {
            break;
        }
    }
}

void imlib_b_and_line_op(image_t *img, int line, void *other, void *data, bool vflipped) {
    image_t *mask = (image_t *) data;

    switch (img->pixfmt) {
        case PIXFORMAT_BINARY: {
            uint32_t *row0 = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img, line);
            uint32_t *row1 = (uint32_t *) other;

            if (!mask) {
                size_t x = 0;
                #if (__ARM_ARCH > 6)
                for (; (img->w - x) >= 32; x += 32) {
                    uint32_t p0 = row0[x / 32];
                    uint32_t p1 = ((uint32_t *) row1)[x / 32];
                    row0[x / 32] = p0 & p1;
                }
                #endif

                for (; x < img->w; x++) {
                    uint32_t p0 = IMAGE_GET_BINARY_PIXEL_FAST(row0, x);
                    uint32_t p1 = IMAGE_GET_BINARY_PIXEL_FAST(row1, x);
                    uint32_t p = p0 & p1;
                    IMAGE_PUT_BINARY_PIXEL_FAST(row0, x, p);
                }
            } else {
                for (size_t x = 0; x < img->w; x++) {
                    if (image_get_mask_pixel(mask, x, line)) {
                        uint32_t p0 = IMAGE_GET_BINARY_PIXEL_FAST(row0, x);
                        uint32_t p1 = IMAGE_GET_BINARY_PIXEL_FAST(row1, x);
                        uint32_t p = p0 & p1;
                        IMAGE_PUT_BINARY_PIXEL_FAST(row0, x, p);
                    }
                }
            }
            break;
        }
        case PIXFORMAT_GRAYSCALE: {
            uint8_t *row0 = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, line);
            uint8_t *row1 = (uint8_t *) other;

            if (!mask) {
                size_t x = 0;
                #if (__ARM_ARCH > 6)
                for (; (img->w - x) >= 4; x += 4) {
                    uint32_t p0 = *((uint32_t *) (row0 + x));
                    uint32_t p1 = *((uint32_t *) (row1 + x));
                    *((uint32_t *) (row0 + x)) = p0 & p1;
                }
                #endif

                for (; x < img->w; x++) {
                    uint32_t p0 = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row0, x);
                    uint32_t p1 = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row1, x);
                    uint32_t p = p0 & p1;
                    IMAGE_PUT_GRAYSCALE_PIXEL_FAST(row0, x, p);
                }
            } else {
                for (size_t x = 0; x < img->w; x++) {
                    if (image_get_mask_pixel(mask, x, line)) {
                        uint32_t p0 = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row0, x);
                        uint32_t p1 = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row1, x);
                        uint32_t p = p0 & p1;
                        IMAGE_PUT_GRAYSCALE_PIXEL_FAST(row0, x, p);
                    }
                }
            }
            break;
        }
        case PIXFORMAT_RGB565: {
            uint16_t *row0 = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img, line);
            uint16_t *row1 = (uint16_t *) other;

            if (!mask) {
                size_t x = 0;
                #if (__ARM_ARCH > 6)
                for (; (img->w - x) >= 2; x += 2) {
                    uint32_t p0 = *((uint32_t *) (row0 + x));
                    uint32_t p1 = *((uint32_t *) (row1 + x));
                    *((uint32_t *) (row0 + x)) = p0 & p1;
                }
                #endif

                for (; x < img->w; x++) {
                    uint32_t p0 = IMAGE_GET_RGB565_PIXEL_FAST(row0, x);
                    uint32_t p1 = IMAGE_GET_RGB565_PIXEL_FAST(row1, x);
                    uint32_t p = p0 & p1;
                    IMAGE_PUT_RGB565_PIXEL_FAST(row0, x, p);
                }
            } else {
                for (size_t x = 0; x < img->w; x++) {
                    if (image_get_mask_pixel(mask, x, line)) {
                        uint32_t p0 = IMAGE_GET_RGB565_PIXEL_FAST(row0, x);
                        uint32_t p1 = IMAGE_GET_RGB565_PIXEL_FAST(row1, x);
                        uint32_t p = p0 & p1;
                        IMAGE_PUT_RGB565_PIXEL_FAST(row0, x, p);
                    }
                }
            }
            break;
        }
        default: {
            break;
        }
    }
}

void imlib_b_and(image_t *img, const char *path, image_t *other, int scalar, image_t *mask) {
    imlib_image_operation(img, path, other, scalar, imlib_b_and_line_op, mask);
}

static void imlib_b_nand_line_op(image_t *img, int line, void *other, void *data, bool vflipped) {
    image_t *mask = (image_t *) data;

    switch (img->pixfmt) {
        case PIXFORMAT_BINARY: {
            uint32_t *row0 = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img, line);
            uint32_t *row1 = (uint32_t *) other;

            if (!mask) {
                size_t x = 0;
                #if (__ARM_ARCH > 6)
                for (; (img->w - x) >= 32; x += 32) {
                    uint32_t p0 = row0[x / 32];
                    uint32_t p1 = row1[x / 32];
                    row0[x / 32] = ~(p0 & p1);
                }
                #endif

                for (; x < img->w; x++) {
                    uint32_t p0 = IMAGE_GET_BINARY_PIXEL_FAST(row0, x);
                    uint32_t p1 = IMAGE_GET_BINARY_PIXEL_FAST(row1, x);
                    uint32_t p = ~(p0 & p1);
                    IMAGE_PUT_BINARY_PIXEL_FAST(row0, x, p);
                }
            } else {
                for (size_t x = 0; x < img->w; x++) {
                    if (image_get_mask_pixel(mask, x, line)) {
                        uint32_t p0 = IMAGE_GET_BINARY_PIXEL_FAST(row0, x);
                        uint32_t p1 = IMAGE_GET_BINARY_PIXEL_FAST(row1, x);
                        uint32_t p = ~(p0 & p1);
                        IMAGE_PUT_BINARY_PIXEL_FAST(row0, x, p);
                    }
                }
            }
            break;
        }
        case PIXFORMAT_GRAYSCALE: {
            uint8_t *row0 = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, line);
            uint8_t *row1 = (uint8_t *) other;

            if (!mask) {
                size_t x = 0;
                #if (__ARM_ARCH > 6)
                for (; (img->w - x) >= 4; x += 4) {
                    uint32_t p0 = *((uint32_t *) (row0 + x));
                    uint32_t p1 = *((uint32_t *) (row1 + x));
                    *((uint32_t *) (row0 + x)) = ~(p0 & p1);
                }
                #endif

                for (; x < img->w; x++) {
                    uint32_t p0 = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row0, x);
                    uint32_t p1 = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row1, x);
                    uint32_t p = ~(p0 & p1);
                    IMAGE_PUT_GRAYSCALE_PIXEL_FAST(row0, x, p);
                }
            } else {
                for (size_t x = 0; x < img->w; x++) {
                    if (image_get_mask_pixel(mask, x, line)) {
                        uint32_t p0 = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row0, x);
                        uint32_t p1 = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row1, x);
                        uint32_t p = ~(p0 & p1);
                        IMAGE_PUT_GRAYSCALE_PIXEL_FAST(row0, x, p);
                    }
                }
            }
            break;
        }
        case PIXFORMAT_RGB565: {
            uint16_t *row0 = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img, line);
            uint16_t *row1 = (uint16_t *) other;

            if (!mask) {
                size_t x = 0;
                #if (__ARM_ARCH > 6)
                for (; (img->w - x) >= 2; x += 2) {
                    uint32_t p0 = *((uint32_t *) (row0 + x));
                    uint32_t p1 = *((uint32_t *) (row1 + x));
                    *((uint32_t *) (row0 + x)) = ~(p0 & p1);
                }
                #endif

                for (; x < img->w; x++) {
                    uint32_t p0 = IMAGE_GET_RGB565_PIXEL_FAST(row0, x);
                    uint32_t p1 = IMAGE_GET_RGB565_PIXEL_FAST(row1, x);
                    uint32_t p = ~(p0 & p1);
                    IMAGE_PUT_RGB565_PIXEL_FAST(row0, x, p);
                }
            } else {
                for (size_t x = 0; x < img->w; x++) {
                    if (image_get_mask_pixel(mask, x, line)) {
                        uint32_t p0 = IMAGE_GET_RGB565_PIXEL_FAST(row0, x);
                        uint32_t p1 = IMAGE_GET_RGB565_PIXEL_FAST(row1, x);
                        uint32_t p = ~(p0 & p1);
                        IMAGE_PUT_RGB565_PIXEL_FAST(row0, x, p);
                    }
                }
            }
            break;
        }
        default: {
            break;
        }
    }
}

void imlib_b_nand(image_t *img, const char *path, image_t *other, int scalar, image_t *mask) {
    imlib_image_operation(img, path, other, scalar, imlib_b_nand_line_op, mask);
}

void imlib_b_or_line_op(image_t *img, int line, void *other, void *data, bool vflipped) {
    image_t *mask = (image_t *) data;

    switch (img->pixfmt) {
        case PIXFORMAT_BINARY: {
            uint32_t *row0 = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img, line);
            uint32_t *row1 = (uint32_t *) other;

            if (!mask) {
                size_t x = 0;
                #if (__ARM_ARCH > 6)
                for (; (img->w - x) >= 32; x += 32) {
                    uint32_t p0 = row0[x / 32];
                    uint32_t p1 = row1[x / 32];
                    row0[x / 32] = p0 | p1;
                }
                #endif

                for (; x < img->w; x++) {
                    uint32_t p0 = IMAGE_GET_BINARY_PIXEL_FAST(row0, x);
                    uint32_t p1 = IMAGE_GET_BINARY_PIXEL_FAST(row1, x);
                    uint32_t p = p0 | p1;
                    IMAGE_PUT_BINARY_PIXEL_FAST(row0, x, p);
                }
            } else {
                for (size_t x = 0; x < img->w; x++) {
                    if (image_get_mask_pixel(mask, x, line)) {
                        uint32_t p0 = IMAGE_GET_BINARY_PIXEL_FAST(row0, x);
                        uint32_t p1 = IMAGE_GET_BINARY_PIXEL_FAST(row1, x);
                        uint32_t p = p0 | p1;
                        IMAGE_PUT_BINARY_PIXEL_FAST(row0, x, p);
                    }
                }
            }
            break;
        }
        case PIXFORMAT_GRAYSCALE: {
            uint8_t *row0 = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, line);
            uint8_t *row1 = (uint8_t *) other;

            if (!mask) {
                size_t x = 0;
                #if (__ARM_ARCH > 6)
                for (; (img->w - x) >= 4; x += 4) {
                    uint32_t p0 = *((uint32_t *) (row0 + x));
                    uint32_t p1 = *((uint32_t *) (row1 + x));
                    *((uint32_t *) (row0 + x)) = p0 | p1;
                }
                #endif

                for (; x < img->w; x++) {
                    uint32_t p0 = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row0, x);
                    uint32_t p1 = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row1, x);
                    uint32_t p = p0 | p1;
                    IMAGE_PUT_GRAYSCALE_PIXEL_FAST(row0, x, p);
                }
            } else {
                for (size_t x = 0; x < img->w; x++) {
                    if (image_get_mask_pixel(mask, x, line)) {
                        uint32_t p0 = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row0, x);
                        uint32_t p1 = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row1, x);
                        uint32_t p = p0 | p1;
                        IMAGE_PUT_GRAYSCALE_PIXEL_FAST(row0, x, p);
                    }
                }
            }
            break;
        }
        case PIXFORMAT_RGB565: {
            uint16_t *row0 = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img, line);
            uint16_t *row1 = (uint16_t *) other;

            if (!mask) {
                size_t x = 0;
                #if (__ARM_ARCH > 6)
                for (; (img->w - x) >= 2; x += 2) {
                    uint32_t p0 = *((uint32_t *) (row0 + x));
                    uint32_t p1 = *((uint32_t *) (row1 + x));
                    *((uint32_t *) (row0 + x)) = p0 | p1;
                }
                #endif

                for (; x < img->w; x++) {
                    uint32_t p0 = IMAGE_GET_RGB565_PIXEL_FAST(row0, x);
                    uint32_t p1 = IMAGE_GET_RGB565_PIXEL_FAST(row1, x);
                    uint32_t p = p0 | p1;
                    IMAGE_PUT_RGB565_PIXEL_FAST(row0, x, p);
                }
            } else {
                for (size_t x = 0; x < img->w; x++) {
                    if (image_get_mask_pixel(mask, x, line)) {
                        uint32_t p0 = IMAGE_GET_RGB565_PIXEL_FAST(row0, x);
                        uint32_t p1 = IMAGE_GET_RGB565_PIXEL_FAST(row1, x);
                        uint32_t p = p0 | p1;
                        IMAGE_PUT_RGB565_PIXEL_FAST(row0, x, p);
                    }
                }
            }
            break;
        }
        default: {
            break;
        }
    }
}

void imlib_b_or(image_t *img, const char *path, image_t *other, int scalar, image_t *mask) {
    imlib_image_operation(img, path, other, scalar, imlib_b_or_line_op, mask);
}

static void imlib_b_nor_line_op(image_t *img, int line, void *other, void *data, bool vflipped) {
    image_t *mask = (image_t *) data;

    switch (img->pixfmt) {
        case PIXFORMAT_BINARY: {
            uint32_t *row0 = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img, line);
            uint32_t *row1 = (uint32_t *) other;

            if (!mask) {
                size_t x = 0;
                #if (__ARM_ARCH > 6)
                for (; (img->w - x) >= 32; x += 32) {
                    uint32_t p0 = row0[x / 32];
                    uint32_t p1 = row1[x / 32];
                    row0[x / 32] = ~(p0 | p1);
                }
                #endif

                for (; x < img->w; x++) {
                    uint32_t p0 = IMAGE_GET_BINARY_PIXEL_FAST(row0, x);
                    uint32_t p1 = IMAGE_GET_BINARY_PIXEL_FAST(row1, x);
                    uint32_t p = ~(p0 | p1);
                    IMAGE_PUT_BINARY_PIXEL_FAST(row0, x, p);
                }
            } else {
                for (size_t x = 0; x < img->w; x++) {
                    if (image_get_mask_pixel(mask, x, line)) {
                        uint32_t p0 = IMAGE_GET_BINARY_PIXEL_FAST(row0, x);
                        uint32_t p1 = IMAGE_GET_BINARY_PIXEL_FAST(row1, x);
                        uint32_t p = ~(p0 | p1);
                        IMAGE_PUT_BINARY_PIXEL_FAST(row0, x, p);
                    }
                }
            }
            break;
        }
        case PIXFORMAT_GRAYSCALE: {
            uint8_t *row0 = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, line);
            uint8_t *row1 = (uint8_t *) other;

            if (!mask) {
                size_t x = 0;
                #if (__ARM_ARCH > 6)
                for (; (img->w - x) >= 4; x += 4) {
                    uint32_t p0 = *((uint32_t *) (row0 + x));
                    uint32_t p1 = *((uint32_t *) (row1 + x));
                    *((uint32_t *) (row0 + x)) = ~(p0 | p1);
                }
                #endif

                for (; x < img->w; x++) {
                    uint32_t p0 = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row0, x);
                    uint32_t p1 = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row1, x);
                    uint32_t p = ~(p0 | p1);
                    IMAGE_PUT_GRAYSCALE_PIXEL_FAST(row0, x, p);
                }
            } else {
                for (size_t x = 0; x < img->w; x++) {
                    if (image_get_mask_pixel(mask, x, line)) {
                        uint32_t p0 = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row0, x);
                        uint32_t p1 = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row1, x);
                        uint32_t p = ~(p0 | p1);
                        IMAGE_PUT_GRAYSCALE_PIXEL_FAST(row0, x, p);
                    }
                }
            }
            break;
        }
        case PIXFORMAT_RGB565: {
            uint16_t *row0 = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img, line);
            uint16_t *row1 = (uint16_t *) other;

            if (!mask) {
                size_t x = 0;
                #if (__ARM_ARCH > 6)
                for (; (img->w - x) >= 2; x += 2) {
                    uint32_t p0 = *((uint32_t *) (row0 + x));
                    uint32_t p1 = *((uint32_t *) (row1 + x));
                    *((uint32_t *) (row0 + x)) = ~(p0 | p1);
                }
                #endif

                for (; x < img->w; x++) {
                    uint32_t p0 = IMAGE_GET_RGB565_PIXEL_FAST(row0, x);
                    uint32_t p1 = IMAGE_GET_RGB565_PIXEL_FAST(row1, x);
                    uint32_t p = ~(p0 | p1);
                    IMAGE_PUT_RGB565_PIXEL_FAST(row0, x, p);
                }
            } else {
                for (size_t x = 0; x < img->w; x++) {
                    if (image_get_mask_pixel(mask, x, line)) {
                        uint32_t p0 = IMAGE_GET_RGB565_PIXEL_FAST(row0, x);
                        uint32_t p1 = IMAGE_GET_RGB565_PIXEL_FAST(row1, x);
                        uint32_t p = ~(p0 | p1);
                        IMAGE_PUT_RGB565_PIXEL_FAST(row0, x, p);
                    }
                }
            }
            break;
        }
        default: {
            break;
        }
    }
}

void imlib_b_nor(image_t *img, const char *path, image_t *other, int scalar, image_t *mask) {
    imlib_image_operation(img, path, other, scalar, imlib_b_nor_line_op,  mask);
}

void imlib_b_xor_line_op(image_t *img, int line, void *other, void *data, bool vflipped) {
    image_t *mask = (image_t *) data;

    switch (img->pixfmt) {
        case PIXFORMAT_BINARY: {
            uint32_t *row0 = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img, line);
            uint32_t *row1 = (uint32_t *) other;

            if (!mask) {
                size_t x = 0;
                #if (__ARM_ARCH > 6)
                for (; (img->w - x) >= 32; x += 32) {
                    uint32_t p0 = row0[x / 32];
                    uint32_t p1 = row1[x / 32];
                    row0[x / 32] = p0 ^ p1;
                }
                #endif

                for (; x < img->w; x++) {
                    uint32_t p0 = IMAGE_GET_BINARY_PIXEL_FAST(row0, x);
                    uint32_t p1 = IMAGE_GET_BINARY_PIXEL_FAST(row1, x);
                    uint32_t p = p0 ^ p1;
                    IMAGE_PUT_BINARY_PIXEL_FAST(row0, x, p);
                }
            } else {
                for (size_t x = 0; x < img->w; x++) {
                    if (image_get_mask_pixel(mask, x, line)) {
                        uint32_t p0 = IMAGE_GET_BINARY_PIXEL_FAST(row0, x);
                        uint32_t p1 = IMAGE_GET_BINARY_PIXEL_FAST(row1, x);
                        uint32_t p = p0 ^ p1;
                        IMAGE_PUT_BINARY_PIXEL_FAST(row0, x, p);
                    }
                }
            }
            break;
        }
        case PIXFORMAT_GRAYSCALE: {
            uint8_t *row0 = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, line);
            uint8_t *row1 = (uint8_t *) other;

            if (!mask) {
                size_t x = 0;
                #if (__ARM_ARCH > 6)
                for (; (img->w - x) >= 4; x += 4) {
                    uint32_t p0 = *((uint32_t *) (row0 + x));
                    uint32_t p1 = *((uint32_t *) (row1 + x));
                    *((uint32_t *) (row0 + x)) = p0 ^ p1;
                }
                #endif

                for (; x < img->w; x++) {
                    uint32_t p0 = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row0, x);
                    uint32_t p1 = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row1, x);
                    uint32_t p = p0 ^ p1;
                    IMAGE_PUT_GRAYSCALE_PIXEL_FAST(row0, x, p);
                }
            } else {
                for (size_t x = 0; x < img->w; x++) {
                    if (image_get_mask_pixel(mask, x, line)) {
                        uint32_t p0 = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row0, x);
                        uint32_t p1 = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row1, x);
                        uint32_t p = p0 ^ p1;
                        IMAGE_PUT_GRAYSCALE_PIXEL_FAST(row0, x, p);
                    }
                }
            }
            break;
        }
        case PIXFORMAT_RGB565: {
            uint16_t *row0 = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img, line);
            uint16_t *row1 = (uint16_t *) other;

            if (!mask) {
                size_t x = 0;
                #if (__ARM_ARCH > 6)
                for (; (img->w - x) >= 2; x += 2) {
                    uint32_t p0 = *((uint32_t *) (row0 + x));
                    uint32_t p1 = *((uint32_t *) (row1 + x));
                    *((uint32_t *) (row0 + x)) = p0 ^ p1;
                }
                #endif

                for (; x < img->w; x++) {
                    uint32_t p0 = IMAGE_GET_RGB565_PIXEL_FAST(row0, x);
                    uint32_t p1 = IMAGE_GET_RGB565_PIXEL_FAST(row1, x);
                    uint32_t p = p0 ^ p1;
                    IMAGE_PUT_RGB565_PIXEL_FAST(row0, x, p);
                }
            } else {
                for (size_t x = 0; x < img->w; x++) {
                    if (image_get_mask_pixel(mask, x, line)) {
                        uint32_t p0 = IMAGE_GET_RGB565_PIXEL_FAST(row0, x);
                        uint32_t p1 = IMAGE_GET_RGB565_PIXEL_FAST(row1, x);
                        uint32_t p = p0 ^ p1;
                        IMAGE_PUT_RGB565_PIXEL_FAST(row0, x, p);
                    }
                }
            }
            break;
        }
        default: {
            break;
        }
    }
}

void imlib_b_xor(image_t *img, const char *path, image_t *other, int scalar, image_t *mask) {
    imlib_image_operation(img, path, other, scalar, imlib_b_xor_line_op, mask);
}

static void imlib_b_xnor_line_op(image_t *img, int line, void *other, void *data, bool vflipped) {
    image_t *mask = (image_t *) data;

    switch (img->pixfmt) {
        case PIXFORMAT_BINARY: {
            uint32_t *row0 = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img, line);
            uint32_t *row1 = (uint32_t *) other;

            if (!mask) {
                size_t x = 0;
                #if (__ARM_ARCH > 6)
                for (; (img->w - x) >= 32; x += 32) {
                    uint32_t p0 = row0[x / 32];
                    uint32_t p1 = row1[x / 32];
                    row0[x / 32] = ~(p0 ^ p1);
                }
                #endif

                for (; x < img->w; x++) {
                    uint32_t p0 = IMAGE_GET_BINARY_PIXEL_FAST(row0, x);
                    uint32_t p1 = IMAGE_GET_BINARY_PIXEL_FAST(row1, x);
                    uint32_t p = ~(p0 ^ p1);
                    IMAGE_PUT_BINARY_PIXEL_FAST(row0, x, p);
                }
            } else {
                for (size_t x = 0; x < img->w; x++) {
                    if (image_get_mask_pixel(mask, x, line)) {
                        uint32_t p0 = IMAGE_GET_BINARY_PIXEL_FAST(row0, x);
                        uint32_t p1 = IMAGE_GET_BINARY_PIXEL_FAST(row1, x);
                        uint32_t p = ~(p0 ^ p1);
                        IMAGE_PUT_BINARY_PIXEL_FAST(row0, x, p);
                    }
                }
            }
            break;
        }
        case PIXFORMAT_GRAYSCALE: {
            uint8_t *row0 = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, line);
            uint8_t *row1 = (uint8_t *) other;

            if (!mask) {
                size_t x = 0;
                #if (__ARM_ARCH > 6)
                for (; (img->w - x) >= 4; x += 4) {
                    uint32_t p0 = *((uint32_t *) (row0 + x));
                    uint32_t p1 = *((uint32_t *) (row1 + x));
                    *((uint32_t *) (row0 + x)) = ~(p0 ^ p1);
                }
                #endif

                for (; x < img->w; x++) {
                    uint32_t p0 = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row0, x);
                    uint32_t p1 = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row1, x);
                    uint32_t p = ~(p0 ^ p1);
                    IMAGE_PUT_GRAYSCALE_PIXEL_FAST(row0, x, p);
                }
            } else {
                for (size_t x = 0; x < img->w; x++) {
                    if (image_get_mask_pixel(mask, x, line)) {
                        uint32_t p0 = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row0, x);
                        uint32_t p1 = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row1, x);
                        uint32_t p = ~(p0 ^ p1);
                        IMAGE_PUT_GRAYSCALE_PIXEL_FAST(row0, x, p);
                    }
                }
            }
            break;
        }
        case PIXFORMAT_RGB565: {
            uint16_t *row0 = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img, line);
            uint16_t *row1 = (uint16_t *) other;

            if (!mask) {
                size_t x = 0;
                #if (__ARM_ARCH > 6)
                for (; (img->w - x) >= 2; x += 2) {
                    uint32_t p0 = *((uint32_t *) (row0 + x));
                    uint32_t p1 = *((uint32_t *) (row1 + x));
                    *((uint32_t *) (row0 + x)) = ~(p0 ^ p1);
                }
                #endif

                for (; x < img->w; x++) {
                    uint32_t p0 = IMAGE_GET_RGB565_PIXEL_FAST(row0, x);
                    uint32_t p1 = IMAGE_GET_RGB565_PIXEL_FAST(row1, x);
                    uint32_t p = ~(p0 ^ p1);
                    IMAGE_PUT_RGB565_PIXEL_FAST(row0, x, p);
                }
            } else {
                for (size_t x = 0; x < img->w; x++) {
                    if (image_get_mask_pixel(mask, x, line)) {
                        uint32_t p0 = IMAGE_GET_RGB565_PIXEL_FAST(row0, x);
                        uint32_t p1 = IMAGE_GET_RGB565_PIXEL_FAST(row1, x);
                        uint32_t p = ~(p0 ^ p1);
                        IMAGE_PUT_RGB565_PIXEL_FAST(row0, x, p);
                    }
                }
            }
            break;
        }
        default: {
            break;
        }
    }
}

void imlib_b_xnor(image_t *img, const char *path, image_t *other, int scalar, image_t *mask) {
    imlib_image_operation(img, path, other, scalar, imlib_b_xnor_line_op, mask);
}

static void imlib_erode_dilate(image_t *img, int ksize, int threshold, int e_or_d, image_t *mask) {
    int brows = ksize + 1;
    image_t buf;
    buf.w = img->w;
    buf.h = brows;
    buf.pixfmt = img->pixfmt;

    switch (img->pixfmt) {
        case PIXFORMAT_BINARY: {
            buf.data = fb_alloc(IMAGE_BINARY_LINE_LEN_BYTES(img) * brows, FB_ALLOC_NO_HINT);

            for (int y = 0; y < img->h; y++) {
                uint32_t *row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img, y);
                uint32_t *buf_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(&buf, (y % brows));
                int acc = 0;

                for (int x = 0; x < img->w; x++) {
                    int pixel = IMAGE_GET_BINARY_PIXEL_FAST(row_ptr, x);
                    IMAGE_PUT_BINARY_PIXEL_FAST(buf_row_ptr, x, pixel);

                    if (mask && (!image_get_mask_pixel(mask, x, y))) {
                        continue; // Short circuit.
                    }

                    if (x > ksize && x < img->w - ksize && y >= ksize && y < img->h - ksize) {
                        // faster
                        for (int j = -ksize; j <= ksize; j++) {
                            uint32_t *k_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img, y + j);
                            // subtract old left column and add new right column
                            acc -= IMAGE_GET_BINARY_PIXEL_FAST(k_row_ptr, x - ksize - 1);
                            acc += IMAGE_GET_BINARY_PIXEL_FAST(k_row_ptr, x + ksize);
                        }
                    } else {
                        // slower (checks boundaries per pixel)
                        acc = e_or_d ? 0 : -1; // Don't count center pixel...
                        for (int j = -ksize; j <= ksize; j++) {
                            int y_j = IM_CLAMP(y + j, 0, (img->h - 1));
                            uint32_t *k_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img, y_j);

                            for (int k = -ksize; k <= ksize; k++) {
                                int x_k = IM_CLAMP(x + k, 0, (img->w - 1));
                                acc += IMAGE_GET_BINARY_PIXEL_FAST(k_row_ptr, x_k);
                            }
                        }
                    }

                    if (!e_or_d) {
                        // Preserve original pixel value... or clear it.
                        if (acc < threshold) {
                            IMAGE_CLEAR_BINARY_PIXEL_FAST(buf_row_ptr, x);
                        }
                    } else {
                        // Preserve original pixel value... or set it.
                        if (acc > threshold) {
                            IMAGE_SET_BINARY_PIXEL_FAST(buf_row_ptr, x);
                        }
                    }
                }

                if (y >= ksize) {
                    // Transfer buffer lines...
                    memcpy(IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img, (y - ksize)),
                           IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(&buf, ((y - ksize) % brows)),
                           IMAGE_BINARY_LINE_LEN_BYTES(img));
                }
            }

            // Copy any remaining lines from the buffer image...
            for (int y = IM_MAX(img->h - ksize, 0); y < img->h; y++) {
                memcpy(IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img, y),
                       IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(&buf, (y % brows)),
                       IMAGE_BINARY_LINE_LEN_BYTES(img));
            }

            fb_free();
            break;
        }
        case PIXFORMAT_GRAYSCALE: {
            buf.data = fb_alloc(IMAGE_GRAYSCALE_LINE_LEN_BYTES(img) * brows, FB_ALLOC_NO_HINT);

            #if defined(ARM_MATH_DSP)
            int32_t threshold_x4 = __USAT(threshold, 7) * 0x01010101;
            uint32_t e_or_d_mask_x4 = e_or_d ? 0xFFFFFFFF : 0x00000000;
            #endif

            for (int y = 0; y < img->h; y++) {
                uint8_t *row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, y);
                uint8_t *buf_row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(&buf, (y % brows));
                int acc = 0;

                for (int x = 0; x < img->w; x++) {
                    int pixel = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row_ptr, x);
                    IMAGE_PUT_GRAYSCALE_PIXEL_FAST(buf_row_ptr, x, pixel);

                    if (mask && (!image_get_mask_pixel(mask, x, y))) {
                        continue; // Short circuit.
                    }

                    if (x > ksize && x < img->w - ksize && y >= ksize && y < img->h - ksize) {
                        if (0) {
                        #if defined(ARM_MATH_DSP)
                            // acc will never be larger than 121 (<= 127) for ksize <= 5.
                        } else if ((x < img->w - ksize - 3) && (ksize <= 5) && (!mask)) {
                            int32_t acc_x4 = acc & 0xFF;

                            for (int j = -ksize; j <= ksize; j++) {
                                uint8_t *k_row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, y + j);
                                // subtract old left edge 4x
                                __USUB8(*((uint32_t *) (k_row_ptr + x - ksize - 1)), 0x01010101);
                                acc_x4 = __SSUB8(acc_x4, __SEL(0x01010101, 0));
                                // add new right edge to sum 4x
                                __USUB8(*((uint32_t *) (k_row_ptr + x + ksize)), 0x01010101);
                                acc_x4 = __SADD8(acc_x4, __SEL(0x01010101, 0));
                            }

                            // After the loop the lowest 8-bits contain a valid acc value. We
                            // compute the next 3 acc values by adding the sums up.

                            acc_x4 = __SADD8(acc_x4, acc_x4 << 8);
                            acc_x4 = __SADD8(acc_x4, acc_x4 << 16);
                            acc = acc_x4 >> 24;

                            // Compute all erode/dilate thresholds at once.

                            uint32_t pixel_x4 = *((uint32_t *) (row_ptr + x));
                            __SSUB8(e_or_d ? threshold_x4 : acc_x4, e_or_d ? acc_x4 : threshold_x4);
                            *((uint32_t *) (buf_row_ptr + x)) = __SEL(pixel_x4, e_or_d_mask_x4);

                            x += 3;
                            continue;
                        #endif
                        } else {
                            // faster
                            for (int j = -ksize; j <= ksize; j++) {
                                uint8_t *k_row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, y + j);
                                // subtract old left edge and add new right edge to sum
                                acc -= (IMAGE_GET_GRAYSCALE_PIXEL_FAST(k_row_ptr, x - ksize - 1) > 0);
                                acc += (IMAGE_GET_GRAYSCALE_PIXEL_FAST(k_row_ptr, x + ksize) > 0);
                            }
                        }
                    } else {
                        // slower way which checks boundaries per pixel
                        acc = e_or_d ? 0 : -1; // Don't count center pixel...
                        for (int j = -ksize; j <= ksize; j++) {
                            int y_j = IM_CLAMP(y + j, 0, (img->h - 1));
                            uint8_t *k_row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, y_j);

                            for (int k = -ksize; k <= ksize; k++) {
                                int x_k = IM_CLAMP(x + k, 0, (img->w - 1));
                                acc += IMAGE_GET_GRAYSCALE_PIXEL_FAST(k_row_ptr, x_k) > 0;
                            }
                        }
                    }

                    if (!e_or_d) {
                        // Preserve original pixel value... or clear it.
                        if (acc < threshold) {
                            IMAGE_PUT_GRAYSCALE_PIXEL_FAST(buf_row_ptr, x, COLOR_GRAYSCALE_BINARY_MIN);
                        }
                    } else {
                        // Preserve original pixel value... or set it.
                        if (acc > threshold) {
                            IMAGE_PUT_GRAYSCALE_PIXEL_FAST(buf_row_ptr, x, COLOR_GRAYSCALE_BINARY_MAX);
                        }
                    }
                }

                if (y >= ksize) {
                    // Transfer buffer lines...
                    memcpy(IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, (y - ksize)),
                           IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(&buf, ((y - ksize) % brows)),
                           IMAGE_GRAYSCALE_LINE_LEN_BYTES(img));
                }
            }

            // Copy any remaining lines from the buffer image...
            for (int y = IM_MAX(img->h - ksize, 0); y < img->h; y++) {
                memcpy(IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, y),
                       IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(&buf, (y % brows)),
                       IMAGE_GRAYSCALE_LINE_LEN_BYTES(img));
            }

            fb_free();
            break;
        }
        case PIXFORMAT_RGB565: {
            buf.data = fb_alloc(IMAGE_RGB565_LINE_LEN_BYTES(img) * brows, FB_ALLOC_NO_HINT);

            #if defined(ARM_MATH_DSP)
            int32_t threshold_x2 = __USAT(threshold, 15) * 0x00010001;
            uint32_t e_or_d_mask_x2 = e_or_d ? 0xFFFFFFFF : 0x00000000;
            #endif

            for (int y = 0; y < img->h; y++) {
                uint16_t *row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img, y);
                uint16_t *buf_row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(&buf, (y % brows));
                int acc = 0;

                for (int x = 0; x < img->w; x++) {
                    int pixel = IMAGE_GET_RGB565_PIXEL_FAST(row_ptr, x);
                    IMAGE_PUT_RGB565_PIXEL_FAST(buf_row_ptr, x, pixel);

                    if (mask && (!image_get_mask_pixel(mask, x, y))) {
                        continue; // Short circuit.
                    }

                    if (x > ksize && x < img->w - ksize && y >= ksize && y < img->h - ksize) {
                        if (0) {
                        #if defined(ARM_MATH_DSP)
                            // acc will never be larger than 32,761 (<= 32767) for ksize <= 90.
                        } else if ((x < img->w - ksize - 1) && (ksize <= 90) && (!mask)) {
                            int32_t acc_x2 = acc & 0xFFFF;

                            for (int j = -ksize; j <= ksize; j++) {
                                uint16_t *k_row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img, y + j);
                                // subtract old left edge 2x
                                __USUB16(*((uint32_t *) (k_row_ptr + x - ksize - 1)), 0x00010001);
                                acc_x2 = __SSUB16(acc_x2, __SEL(0x00010001, 0));
                                // add new right edge to sum 2x
                                __USUB16(*((uint32_t *) (k_row_ptr + x + ksize)), 0x00010001);
                                acc_x2 = __SADD16(acc_x2, __SEL(0x00010001, 0));
                            }

                            // After the loop the lowest 16-bits contain a valid acc value. We
                            // compute the next acc value by adding the sums up.

                            acc_x2 = __SADD16(acc_x2, acc_x2 << 16);
                            acc = acc_x2 >> 16;

                            // Compute all erode/dilate thresholds at once.

                            uint32_t pixel_x2 = *((uint32_t *) (row_ptr + x));
                            __SSUB16(e_or_d ? threshold_x2 : acc_x2, e_or_d ? acc_x2 : threshold_x2);
                            *((uint32_t *) (buf_row_ptr + x)) = __SEL(pixel_x2, e_or_d_mask_x2);

                            x += 1;
                            continue;
                        #endif
                        } else {
                            // faster
                            for (int j = -ksize; j <= ksize; j++) {
                                uint16_t *k_row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img, y + j);
                                // subtract old left column and add new right column
                                acc -= IMAGE_GET_RGB565_PIXEL_FAST(k_row_ptr, x - ksize - 1) > 0;
                                acc += IMAGE_GET_RGB565_PIXEL_FAST(k_row_ptr, x + ksize) > 0;
                            }
                        }
                    } else {
                        // need to check boundary conditions for each pixel
                        acc = e_or_d ? 0 : -1; // Don't count center pixel...
                        for (int j = -ksize; j <= ksize; j++) {
                            int y_j = IM_CLAMP(y + j, 0, (img->h - 1));
                            uint16_t *k_row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img, y_j);

                            for (int k = -ksize; k <= ksize; k++) {
                                int x_k = IM_CLAMP(x + k, 0, (img->w - 1));
                                acc += IMAGE_GET_RGB565_PIXEL_FAST(k_row_ptr, x_k) > 0;
                            }
                        }
                    }

                    if (!e_or_d) {
                        // Preserve original pixel value... or clear it.
                        if (acc < threshold) {
                            IMAGE_PUT_RGB565_PIXEL_FAST(buf_row_ptr, x, COLOR_RGB565_BINARY_MIN);
                        }
                    } else {
                        // Preserve original pixel value... or set it.
                        if (acc > threshold) {
                            IMAGE_PUT_RGB565_PIXEL_FAST(buf_row_ptr, x, COLOR_RGB565_BINARY_MAX);
                        }
                    }
                }

                if (y >= ksize) {
                    // Transfer buffer lines...
                    memcpy(IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img, (y - ksize)),
                           IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(&buf, ((y - ksize) % brows)),
                           IMAGE_RGB565_LINE_LEN_BYTES(img));
                }
            }

            // Copy any remaining lines from the buffer image...
            for (int y = IM_MAX(img->h - ksize, 0); y < img->h; y++) {
                memcpy(IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img, y),
                       IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(&buf, (y % brows)),
                       IMAGE_RGB565_LINE_LEN_BYTES(img));
            }

            fb_free();
            break;
        }
        default: {
            break;
        }
    }
}

void imlib_erode(image_t *img, int ksize, int threshold, image_t *mask) {
    // Threshold should be equal to 0
    // for normal operation. E.g. for ksize==3 -> threshold==0
    // Basically you're adjusting the number of data that
    // must be set in the kernel (besides the center) for the output to be 1.
    // Erode normally requires all data to be 1.
    imlib_erode_dilate(img, ksize, imlib_ksize_to_n(ksize) - 1 - threshold, 0, mask);
}

void imlib_dilate(image_t *img, int ksize, int threshold, image_t *mask) {
    // Threshold should be equal to 0
    // for normal operation. E.g. for ksize==3 -> threshold==0
    // Basically you're adjusting the number of data that
    // must be set in the kernel (besides the center) for the output to be 1.
    // Dilate normally requires one pixel to be 1.
    imlib_erode_dilate(img, ksize, threshold, 1, mask);
}

void imlib_open(image_t *img, int ksize, int threshold, image_t *mask) {
    imlib_erode(img, ksize, threshold, mask);
    imlib_dilate(img, ksize, threshold, mask);
}

void imlib_close(image_t *img, int ksize, int threshold, image_t *mask) {
    imlib_dilate(img, ksize, threshold, mask);
    imlib_erode(img, ksize, threshold, mask);
}

void imlib_top_hat(image_t *img, int ksize, int threshold, image_t *mask) {
    image_t temp;
    temp.w = img->w;
    temp.h = img->h;
    temp.pixfmt = img->pixfmt;
    temp.data = fb_alloc(image_size(img), FB_ALLOC_NO_HINT);
    memcpy(temp.data, img->data, image_size(img));
    imlib_open(&temp, ksize, threshold, mask);
    imlib_difference(img, NULL, &temp, 0, mask);
    fb_free();
}

void imlib_black_hat(image_t *img, int ksize, int threshold, image_t *mask) {
    image_t temp;
    temp.w = img->w;
    temp.h = img->h;
    temp.pixfmt = img->pixfmt;
    temp.data = fb_alloc(image_size(img), FB_ALLOC_NO_HINT);
    memcpy(temp.data, img->data, image_size(img));
    imlib_close(&temp, ksize, threshold, mask);
    imlib_difference(img, NULL, &temp, 0, mask);
    fb_free();
}
#endif
