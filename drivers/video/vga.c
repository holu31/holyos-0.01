#include "kernel/string.h"
#include "kernel/video.h"
#include "kernel/string.h"
#include "kernel/pci.h"
#include "kernel/kprintf.h"
#include "prefs.h"

#include "stddef.h"
#include "stdint.h"

#define VGA_BUFFER ((uint16_t*) 0xB8000)

uint16_t* vga_buffer = VGA_BUFFER;
static const size_t vga_xres = 80;
static const size_t vga_yres = 25;

static void vga_write(char c, uint32_t x, uint32_t y, video_color_t fg, video_color_t bg) {	
	if (x >= vga_xres || y >= vga_yres) return;

	uint8_t color = (bg.r & 0x1) << 4 | (fg.r & 0x7);
	vga_buffer[y * vga_xres + x] = (color << 8) | c;
}

static void vga_clear(video_color_t color) {
	uint16_t blank = (' ' & 0xff) | ((color.r & 0x7) << 8);
	memset16(vga_buffer, blank, vga_xres * vga_yres);
}

static void vga_scroll(void) {
	memcpy(vga_buffer, vga_buffer + vga_xres, vga_xres * (vga_yres - 1) * 2);
	memset16(vga_buffer + vga_xres * (vga_yres - 1), 
			(' ' & 0xff) | (0x07 << 8), vga_xres);
}

static int is_vga_pci_present() {
	log_debug("[PCI] Scanning for VGA-compatible controllers...\n");
	for (uint32_t bus = 0; bus < 256; bus++) {
		for (uint32_t slot = 0; slot < 32; slot++) {
			uint32_t id = pci_read_config(bus, slot, 0, 0x00);
			if (id == 0xFFFFFFFF) continue;

			uint16_t vendor_id = id & 0xFFFF;
			uint16_t device_id = (id >> 16) & 0xFFFF;

			uint32_t class = pci_read_config(bus, slot, 0, 0x08);			
			if ((class >> 16) == 0x0300) {
				log_debug("[PCI] Found VGA controller (vendor=0x%x, device=0x%x)\n",
						vendor_id, device_id);
				return 1;
			}
		}
	}
	return 0;
}

static int vga_probe() {
	return is_vga_pci_present();
}

struct video_driver vga_driver = {
	.name = "VGA Text",
	.priority = VIDEO_PRIORITY_VGA,
	.probe = vga_probe,
	.write = vga_write,
	.clear = vga_clear,
	.scroll = vga_scroll,
	.width = vga_xres,
	.height = vga_yres,
	.bpp = 4
};

VIDEO_DRIVER(vga_driver);
