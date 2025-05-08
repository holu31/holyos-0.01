#include "kernel/cpu/io.h"
#include "kernel/kprintf.h"
#include "pci.h"

uint32_t pci_read_config(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
	uint32_t address = (uint32_t)((bus << 16) | (slot << 11) | (func << 8) | (offset & 0xFC) | 0x80000000);
	outl(PCI_CONFIG_ADDRESS, address);
	return inl(PCI_CONFIG_DATA);
}

void pci_write_config(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint32_t value) {
	uint32_t address = (uint32_t)((bus << 16) | (slot << 11) | (func << 8) | (offset & 0xFC) | 0x80000000);
	outl(PCI_CONFIG_ADDRESS, address);
	outl(PCI_CONFIG_DATA, value);
}

uint16_t pci_get_vendor_id(uint8_t bus, uint8_t slot, uint8_t func) {
	return (uint16_t)(pci_read_config(bus, slot, func, PCI_VENDOR_ID) & 0xFFFF);
}

uint16_t pci_get_device_id(uint8_t bus, uint8_t slot, uint8_t func) {
	return (uint16_t)(pci_read_config(bus, slot, func, PCI_VENDOR_ID) >> 16);
}

void pci_get_device_info(uint8_t bus, uint8_t device, uint8_t function, pci_device_t* dev_info) {
	uint32_t data;

	data = pci_read_config(bus, device, function, PCI_VENDOR_ID);
	dev_info->vendor_id = (uint16_t)(data & 0xFFFF);
	dev_info->device_id = (uint16_t)(data >> 16);

	data = pci_read_config(bus, device, function, PCI_COMMAND);
	dev_info->command = (uint16_t)(data & 0xFFFF);
	dev_info->status = (uint16_t)(data >> 16);

	data = pci_read_config(bus, device, function, PCI_REVISION_ID);
	dev_info->revision_id = (uint8_t)(data & 0xFF);
	dev_info->prog_if = (uint8_t)((data >> 8) & 0xFF);
	dev_info->subclass = (uint8_t)((data >> 16) & 0xFF);
	dev_info->class_code = (uint8_t)((data >> 24) & 0xFF);

	data = pci_read_config(bus, device, function, PCI_CACHE_LINE_SIZE);
	dev_info->cache_line_size = (uint8_t)(data & 0xFF);
	dev_info->latency_timer = (uint8_t)((data >> 8) & 0xFF);
	dev_info->header_type = (uint8_t)((data >> 16) & 0xFF);
	dev_info->bist = (uint8_t)((data >> 24) & 0xFF);

	// 0x10-0x24
	for (int i = 0; i < 6; i++) {
		dev_info->bar[i] = pci_read_config(bus, device, function, PCI_BAR0 + (i * 4));
	}

	dev_info->cardbus_cis_ptr = pci_read_config(bus, device, function, 0x28);

	data = pci_read_config(bus, device, function, 0x2C);
	dev_info->subsystem_vendor_id = (uint16_t)(data & 0xFFFF);
	dev_info->subsystem_id = (uint16_t)(data >> 16);

	dev_info->expansion_rom_base = pci_read_config(bus, device, function, 0x30);

	data = pci_read_config(bus, device, function, 0x34);
	dev_info->capabilities_ptr = (uint8_t)(data & 0xFF);

	data = pci_read_config(bus, device, function, 0x3C);
	dev_info->interrupt_line = (uint8_t)(data & 0xFF);
	dev_info->interrupt_pin = (uint8_t)((data >> 8) & 0xFF);
	dev_info->min_grant = (uint8_t)((data >> 16) & 0xFF);
	dev_info->max_latency = (uint8_t)((data >> 24) & 0xFF);
}

void pci_scan_function(uint8_t bus, uint8_t device, uint8_t function) {
	uint16_t vendor_id = pci_get_vendor_id(bus, device, function);
	uint16_t device_id = pci_get_device_id(bus, device, function);
	uint32_t data = pci_read_config(bus, device, function, PCI_REVISION_ID);
	const char* class_name = pci_get_class_name((uint8_t)((data >> 24) & 0xFF));

	log_debug("[PCI] - %x:%x.%x: %s (vendor=%x, device=%x)\n",
			bus, device, function, class_name, vendor_id, device_id);

	uint8_t class_code = (uint8_t)((pci_read_config(bus, device, function, PCI_CLASS) >> 24) & 0xFF);
	uint8_t subclass = (uint8_t)((pci_read_config(bus, device, function, PCI_CLASS) >> 16) & 0xFF);

	if (class_code == PCI_CLASS_BRIDGE && subclass == 0x04) {
		uint8_t secondary_bus = (uint8_t)((pci_read_config(bus, device, function, 0x18) >> 8) & 0xFF);
		pci_scan_bus(secondary_bus);
	}
}

void pci_scan_device(uint8_t bus, uint8_t device) {
	uint8_t function = 0;
	uint16_t vendor_id = pci_get_vendor_id(bus, device, function);
    
	if (vendor_id == 0xFFFF) return;
	
	uint8_t header_type = (uint8_t)((pci_read_config(bus, device, function, PCI_HEADER_TYPE) >> 16) & 0xFF);
	
	if ((header_type & 0x80) != 0) {
		for (function = 0; function < 8; function++) {
			if (pci_get_vendor_id(bus, device, function) != 0xFFFF) {
				pci_scan_function(bus, device, function);
			}
		}
	} else {
		pci_scan_function(bus, device, function);
	}
}

void pci_scan_bus(uint8_t bus) {
	for (uint8_t device = 0; device < 32; device++) {
		pci_scan_device(bus, device);
	}
}

const char* pci_get_class_name(uint8_t class_code) {
	switch(class_code) {
		case 0x00: return "Pre-2.0 device";
		case 0x01: return "Mass storage controller";
		case 0x02: return "Network controller";
		case 0x03: return "Display controller";
		case 0x04: return "Multimedia controller";
		case 0x05: return "Memory controller";
		case 0x06: return "Bridge device";
		case 0x07: return "Simple comm. controller";
		case 0x08: return "Base system peripheral";
		case 0x09: return "Input device";
		case 0x0A: return "Docking station";
		case 0x0B: return "Processor";
		case 0x0C: return "Serial bus controller";
		case 0x0D: return "Wireless controller";
		case 0x0E: return "Intelligent controller";
		case 0x0F: return "Satellite comm. controller";
		case 0x10: return "Encryption controller";
		case 0x11: return "Signal processing controller";
		case 0x12: return "Processing accelerator";
		case 0x13: return "Non-essential instrumentation";
		default:   return "Unknown device";
    }
}

void pci_init(void) {
	uint8_t header_type = (uint8_t)((pci_read_config(0, 0, 0, PCI_HEADER_TYPE) >> 16) & 0xFF);

	log_info("[PCI] Scanning PCI bus...\n");
	if ((header_type & 0x80) == 0) {
		log_ok("[PCI] Single PCI host controller detected\n");
		log_debug("[PCI] List of detected devices:\n");
		pci_scan_bus(0);
	} else {
		log_ok("[PCI] Multiple PCI host controllers detected\n");
		for (uint8_t function = 0; function < 8; function++) {
			if (pci_get_vendor_id(0, 0, function) != 0xFFFF) {
				log_debug("[PCI] List of detected devices (bus=%d)\n", function);
				pci_scan_bus(function);
			} else {
				break;
			}
		}
	}
}
