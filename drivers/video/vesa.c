#include "kernel/video.h"
#include "kernel/string.h"
#include "prefs.h"

/*
static int vesa_init(void) {
	vesa_driver.width = 1024;
	vesa_driver.height = 768;
	vesa_driver.bpp = 32;
	vesa_driver.pitch = vesa_driver.width * 4;
	// vesa_driver.framebuffer = // TODO: detect_vesa_framebuffer();

	return 0;
}

static void vesa_put_pixel(uint32_t x, uint32_t y, video_color_t color) {
	uint32_t offset = y * vesa_driver.pitch + x * 4;
	vesa_driver.framebuffer[offset + 0] = color.b;
	vesa_driver.framebuffer[offset + 1] = color.g;
	vesa_driver.framebuffer[offset + 2] = color.r;
}

struct video_driver vesa_driver = {
    .name = "VESA",
    .priority = VIDEO_PRIORITY_VESA,
    .init = vesa_init,
    .set_mode = vesa_set_mode,
    .put_pixel = vesa_put_pixel,
    .put_char = vesa_put_char,
    .clear = vesa_clear,
    .scroll = vesa_scroll
};
*/
//VIDEO_DRIVER(vesa_driver);
