#include "mypng.h"


/**
 * usage:
 */
void usage(int argc, char *argv[]) {

	fprintf(stderr, "Usage: %s device_path file.png\n", argv[0]);
	exit(1);
}

/**
 * addition_will_overflow:
 */
bool addition_will_overflow(size_t a, size_t b) {

	return (((size_t) -1 - a) < b);
}

/**
 * multiplication_will_overflow:
 */
bool multiplication_will_overflow(size_t a, size_t b) {

	if (a == 0 || b == 0) {
		return false;
	}

	return (((size_t) -1 / b) < a);
}

/**
 * fb_set_error:
 */
bool fb_set_error(fb_t *fb, int n, char *str) {

	fb->result_code = n;
	fb->result_string = str;
	fb->system_error_code = errno;

	return false;
}

/**
 * fb_zero_structure:
 */
fb_t *fb_zero_structure(fb_t *fb) {

	fb->fd = 0;
	fb->pixels = NULL;
	fb->device_path = NULL;

	memset(&fb->finfo, 0, sizeof(fb->finfo));
	memset(&fb->vinfo, 0, sizeof(fb->vinfo));

	fb->result_code = 0;
	fb->result_string = 0;
	fb->system_error_code = 0;

	return fb;
}

/**
 * fb_initialize:
 */
bool fb_initialize(fb_t *fb, char *device_path) {

	fb_zero_structure(fb);
	fb->device_path = device_path;

	fb->fd = open(device_path, O_RDWR);

	if (fb->fd < 0) {
		return fb_set_error(fb, 1, "Unable to open specified device");
	}

	if (ioctl(fb->fd, FBIOGET_FSCREENINFO, &fb->finfo)) {
		return fb_set_error(fb, 2, "Unable to read fixed screen information");
	}
	
	if (ioctl(fb->fd, FBIOGET_VSCREENINFO, &fb->vinfo)) {
		return fb_set_error(fb, 3, "Unable to read variable screen information");
	}

	fb->pixels = (uint8_t *) mmap(
		0, fb->finfo.smem_len,
			PROT_READ | PROT_WRITE, MAP_SHARED, fb->fd, 0
	);

	if ((intptr_t) fb->pixels == -1) {
		return fb_set_error(fb, 4, "Failed to map framebuffer in to memory");
	}

	fb->result_code = 0;
	fb->result_string = "Success";

	return true;
}

/**
 * fb_blend_argb32:
 *   Copy 32-bit ARGB pixels from `src` to `dst`, blending each
 *   pixel's color value if the data in `src` isn't fully-opaque.
 *   The value `n` is the size, in bytes, of the region to copy.
 */
bool fb_blend_argb32(uint8_t *dst, uint8_t *src, size_t n) {

	#define saturated_add_uint8(x, y) \
		(((x) > 0xff - (y)) ? 0xff : (x) + (y))

	#define blend_color(dst, src, alpha) \
		(dst) = saturated_add_uint8( \
			(src) * (alpha), (dst) * (1.0 - (alpha)) \
		)

	int i = 0;
	for (i = 0; i < n; i += 4) {

		int  n = i;
		float alpha = src[n + 3] / 255.0;

		blend_color(dst[n], src[n], alpha); n++;
		blend_color(dst[n], src[n], alpha); n++;
		blend_color(dst[n], src[n], alpha); n++;

		dst[n] = 0;
	}

	return true;
}

/**
 * fb_copy_generic:
 *   A generic `memcpy` wrapper for the `copy_fn_t` type. This
 *   is the default copy method for `fb_write_line_generic`.
 */
bool fb_copy_generic(uint8_t *dst, uint8_t *src, size_t n) {

	memcpy(dst, src, n);
	return true;
}

/**
 * fb_write_line_generic:
 *   Write a line of pixel data to the screen. No transformation
 *   whatsoever is performed, so the data must match the current
 *   format of the framebuffer. Overflow, either to the next line
 *   or off-screen, is automatically detected and avoided. If the
 *   entire line would be off-screen, this function returns false.
 *   If any pixels are drawn at all, this function returns true.
 */
bool fb_write_line_generic(fb_t *fb, ssize_t x, ssize_t y,
													 uint8_t *pixels, size_t length, copy_fn_t fn) {

	/* Framebuffer attributes */
	size_t line_length = fb->finfo.line_length;
	size_t bytes_per_pixel = fb->vinfo.bits_per_pixel / 8;

	/* Default */
	if (!fn) {
		fn = fb_copy_generic;
	}

	/* Error: entirely off-screen */
	if (y < 0) {
		return false;
	}

	/* Error: entirely off-screen */
	if (x < 0 && length < -x) {
		return false;
	}

	/* Handle negative offset */
	if (x < 0) {

		/* Calculate number of bytes to skip */
		size_t offscreen_bytes = (-x * bytes_per_pixel);
	 
		/* Adjust bounds */
		length -= offscreen_bytes;
		pixels += offscreen_bytes;
		x = 0;
	}

	/* Find target pixel offset */
	size_t offset = (
		(x + fb->vinfo.xoffset) * (fb->vinfo.bits_per_pixel / 8)
			+ (y + fb->vinfo.yoffset) * line_length
	);

	/* Error: entirely off-screen */
	if (offset >= fb->finfo.smem_len) {
		return false;
	}

	/* Compute remaining space */
	size_t available_length = line_length - (offset % line_length);

	/* Clamp to a single row */
	if (length > available_length) {
		length = available_length;
	}

	/* Write row data */
	(*fn)(&fb->pixels[offset], pixels, length);

	/* Success */
	return true;
}

/**
 * fb_write_line_argb32:
 *   Write a row of 32-bit ARGB pixels to the framebuffer `fb`.
 *   The data is converted to the framebuffer's current pixel format
 *   if necessary, and then written via `fb_write_line_generic`.
 */
bool fb_write_line_argb32(fb_t *fb, ssize_t x, ssize_t y,
													uint8_t *pixels, size_t length) {

	switch (fb->vinfo.bits_per_pixel) {
		case 32:
			return fb_write_line_generic(
				fb, x, y, pixels, length, fb_blend_argb32
			);
		case 24:
			return fb_write_line_generic(
				fb, x, y, pixels, length, fb_blend_argb32
			);
		case 16:
			return fb_write_line_generic(
				fb, x, y, pixels, length, fb_blend_argb32
			);
		case 8:
			return fb_write_line_generic(
				fb, x, y, pixels, length, fb_blend_argb32
			);
		default:
			return false;
	}
}

/**
 * fb_get_width:
 */
unsigned int fb_get_width(fb_t *fb) {
	
	return fb->vinfo.xres;
}

/**
 * fb_get_height:
 */
unsigned int fb_get_height(fb_t *fb) {

	return fb->vinfo.yres;
}

/**
 * fb_draw_image_png:
 */
bool fb_draw_image_png(fb_t *fb, ssize_t pos_x, ssize_t pos_y, const char *path) {

	bool rv = false;
	FILE *fp = fopen(path, "rb");

	if (!fp) {
		return fb_set_error(fb, 1, "Cannot open file");
	}

	const size_t png_header_size = 8;
	unsigned char png_header[png_header_size];

	memset(png_header, 0, png_header_size);
	fread(png_header, 0, png_header_size, fp);

	if (png_sig_cmp(png_header, 0, png_header_size) == 0) {
		fb_set_error(fb, 2, "File format not recognized");
		goto cleanup_fp;
	}

	png_structp png = png_create_read_struct(
		PNG_LIBPNG_VER_STRING, NULL, NULL, NULL
	);

	if (!png) {
		fb_set_error(fb, 3, "Can't create libpng read structure");
		goto cleanup_fp;
	}

	png_infop png_info = png_create_info_struct(png);

	if (!png_info) {
		fb_set_error(fb, 4, "Can't create libpng info structure");
		goto cleanup_read;
	}

	if (setjmp(png_jmpbuf(png))) {
		fb_set_error(fb, 5, "Error initializing libpng's I/O subsystem");
		goto cleanup_info;
	}

	png_init_io(png, fp);
	png_set_sig_bytes(png, 0);
	png_read_info(png, png_info);

	int width = png_get_image_width(png, png_info);
	int height = png_get_image_height(png, png_info);

	png_byte color_type = png_get_color_type(png, png_info);
	png_byte bit_depth = png_get_bit_depth(png, png_info);

	/* Use BGRA */
	png_set_bgr(png);

	/* One-byte channels */
	if (bit_depth == 16) {
		png_set_strip_16(png);
	}

	/* Convert palette images to RGB */
	if (color_type == PNG_COLOR_TYPE_PALETTE) {
		png_set_palette_to_rgb(png);
	}
 
	/* Add alpha channel, if needed */
	if (color_type == PNG_COLOR_TYPE_RGB) {
		png_set_filler(png, 0xff, PNG_FILLER_AFTER);
	}

	png_read_update_info(png, png_info);

	if (setjmp(png_jmpbuf(png))) {
		fb_set_error(fb, 6, "Error while reading image data");
		goto cleanup_info;
	}

	if (multiplication_will_overflow(width, 4)) {
		fb_set_error(fb, 7, "Image width is too large");
		goto cleanup_info;
	}

	png_byte *row = (png_byte *) malloc(width * 4);

	unsigned int y = 0;
	for (y = 0; y < height; y++) {
		png_read_row(png, row, NULL);
		fb_write_line_argb32(fb, pos_x, pos_y+y, row, width * 4);
	}

	/* Success */
	rv = true;

	cleanup_info:
		png_destroy_info_struct(png, &png_info);

	cleanup_read:
		png_destroy_read_struct(&png, NULL, NULL);

	cleanup_fp:
		fclose(fp);

	return rv;
}