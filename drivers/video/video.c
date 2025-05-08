#include "kernel/video.h"
#include "kernel/kprintf.h"
#include "prefs.h"

#define MAX_VIDEO_DRIVERS 5
static struct video_driver* registered_drivers[MAX_VIDEO_DRIVERS];
static struct video_driver *current_driver = NULL;

void video_register_driver(struct video_driver *driver) {
	 for (int i = 0; i < MAX_VIDEO_DRIVERS; i++) {
		 if (!registered_drivers[i]) {
			 registered_drivers[i] = driver;
			 log_info("[VIDEO] Registered driver '%s' (priority=%d)\n", 
					 driver->name, driver->priority);
			 return;
		 }
	 }
	 log_err("[VIDEO] Cannot register driver '%s', no slots left\n", driver->name);
}

void video_init(void) {
	log_info("[VIDEO] Starting video subsystem initialization...\n");
	
	struct video_driver* best = NULL;
	for (int i = 0; registered_drivers[i]; i++) {
		if (registered_drivers[i]->priority < best->priority) continue;
		if (registered_drivers[i]->probe()) {
			best = registered_drivers[i];	
			break;
		}
	}

	if (best) {
		current_driver = best;
		if (current_driver->init)
			current_driver->init();
		
		log_ok("[VIDEO] Initializated default video: %s\n", current_driver->name);
	}
	else log_err("[VIDEO] No suitable video driver found!\n");
}

struct video_driver* video_get_current_driver(void) {
	return current_driver;
}

int video_set_mode(uint32_t width, uint32_t height, uint32_t bpp) {
	if (current_driver && current_driver->set_mode)
		return current_driver->set_mode(width, height, bpp);
	return -1;
}

void video_put_pixel(uint32_t x, uint32_t y, video_color_t color) {
	if (current_driver && current_driver->put_pixel)
		current_driver->put_pixel(x, y, color);
}

void video_write(char c, uint32_t x, uint32_t y, video_color_t fg, video_color_t bg) {
	if (current_driver && current_driver->write)
		current_driver->write(c, x, y, fg, bg);

}

void video_clear(video_color_t color) {
	if (current_driver && current_driver->clear)
		current_driver->clear(color);

}

void video_scroll(void) {
	if (current_driver && current_driver->scroll)
		current_driver->scroll();

}
