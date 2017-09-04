#ifndef __MY_PNG_H__
#define __MY_PNG_H__


#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <inttypes.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include "png.h"
#include "pngconf.h"
#include "pnglibconf.h"
/**
 * fatal:
 */
 #define fatal(format, ...) \
 do { \
     fprintf(stderr, "Fatal: "); \
     fprintf(stderr, format, ##__VA_ARGS__); \
     fprintf(stderr, "\n"); \
     exit(127); \
 } while (0)

/**
 * fb_t:
 */
typedef struct fb {
    int fd;
    uint8_t *pixels;
    char *device_path;
    struct fb_fix_screeninfo finfo;
    struct fb_var_screeninfo vinfo;

    int result_code;
    char *result_string;
    int system_error_code;

} fb_t;

/**
 * copy_fn_t:
 **/
 typedef bool (*copy_fn_t)(uint8_t *, uint8_t *, size_t);


void usage(int argc, char *argv[]);
bool addition_will_overflow(size_t a, size_t b);
bool multiplication_will_overflow(size_t a, size_t b);
bool fb_set_error(fb_t *fb, int n, char *str);
fb_t *fb_zero_structure(fb_t *fb);
bool fb_initialize(fb_t *fb, char *device_path);
bool fb_blend_argb32(uint8_t *dst, uint8_t *src, size_t n);
bool fb_copy_generic(uint8_t *dst, uint8_t *src, size_t n);
bool fb_write_line_generic(fb_t *fb, ssize_t x, ssize_t y, uint8_t *pixels, size_t length, copy_fn_t fn);
bool fb_write_line_argb32(fb_t *fb, ssize_t x, ssize_t y, uint8_t *pixels, size_t length);
unsigned int fb_get_width(fb_t *fb);
unsigned int fb_get_height(fb_t *fb);
bool fb_draw_image_png(fb_t *fb, ssize_t pos_x, ssize_t pos_y, const char *path);

#endif