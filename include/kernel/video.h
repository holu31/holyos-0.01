#ifndef _KERNEL_VIDEO_H
#define _KERNEL_VIDEO_H

#include "stdint.h"
#include "stddef.h"

typedef struct {
	uint8_t r;
	uint8_t g;
	uint8_t b;
} video_color_t;

struct video_driver {
	const char *name;
	int priority;

	int (*probe)(void);
	int (*init)(void);

	int (*set_mode)(uint32_t width, uint32_t height, uint32_t bpp);
	void (*put_pixel)(uint32_t x, uint32_t y, video_color_t color);
	void (*write)(char c, uint32_t x, uint32_t y, video_color_t fg, video_color_t bg);
	void (*clear)(video_color_t color);
	void (*scroll)(void);

	uint32_t width;
	uint32_t height;
	uint32_t bpp;
	uint32_t pitch;
	uint8_t* framebuffer;
	uint32_t framebuffer_size;
};

#define VIDEO_DRIVER(drv) \
	static void __attribute__((constructor)) __register_##drv(void) { \
		video_register_driver(&drv); \
	}

void video_register_driver(struct video_driver* driver);
void video_init(void);
struct video_driver* video_get_current_driver(void);

int video_set_mode(uint32_t width, uint32_t height, uint32_t bpp);
void video_put_pixel(uint32_t x, uint32_t y, video_color_t color);
void video_write(char c, uint32_t x, uint32_t y, video_color_t fg, video_color_t bg);
void video_clear(video_color_t color);
void video_scroll(void);

#endif // _KERNEL_VIDEO_H
