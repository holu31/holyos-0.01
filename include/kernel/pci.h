#ifndef _KERNEL_PCI_H
#define _KERNEL_PCI_H

#include "stdint.h"

#define PCI_CONFIG_ADDRESS	0xCF8
#define PCI_CONFIG_DATA		0xCFC

#define PCI_VENDOR_ID		0x00
#define PCI_DEVICE_ID		0x02
#define PCI_COMMAND		0x04
#define PCI_STATUS		0x06
#define PCI_REVISION_ID		0x08
#define PCI_CLASS		0x0B
#define PCI_HEADER_TYPE		0x0E
#define PCI_CACHE_LINE_SIZE 	0x0C
#define PCI_BAR0		0x10

typedef struct {
	uint16_t vendor_id;
	uint16_t device_id;
	uint16_t command;
	uint16_t status;
	uint8_t revision_id;
	uint8_t prog_if;
	uint8_t subclass;
	uint8_t class_code;
	uint8_t cache_line_size;
	uint8_t latency_timer;
	uint8_t header_type;
	uint8_t bist;
	uint32_t bar[6];
	uint32_t cardbus_cis_ptr;
	uint16_t subsystem_vendor_id;
	uint16_t subsystem_id;
	uint32_t expansion_rom_base;
	uint8_t capabilities_ptr;
	uint8_t reserved[7];
	uint8_t interrupt_line;
	uint8_t interrupt_pin;
	uint8_t min_grant;
	uint8_t max_latency;
} pci_device_t;

uint32_t pci_read_config(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);
void pci_write_config(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint32_t value);
uint16_t pci_get_vendor_id(uint8_t bus, uint8_t slot, uint8_t func);
uint16_t pci_get_device_id(uint8_t bus, uint8_t slot, uint8_t func);
void pci_get_device_info(uint8_t bus, uint8_t device, uint8_t function, pci_device_t* dev_info);
const char* pci_get_class_name(uint8_t class_code);
void pci_init(void);

#endif // _KERNEL_PCI_H
