#include "kernel/multiboot.h"
#include "kernel/cpu/gdt.h"
#include "kernel/cpu/idt.h"
#include "kernel/cpu/cpu.h"
#include "kernel/cpu/pic.h"
#include "kernel/mm/pmm.h"
#include "kernel/mm/vmm.h"
#include "kernel/mm/heap.h"
#include "kernel/input.h"
#include "kernel/video.h"
#include "kernel/pci.h"
#include "kernel/string.h"
#include "kernel/serial.h"
#include "kernel/kprintf.h"
#include "kernel/tty/tty.h"

extern void init_ctors();

void kmain(multiboot_info_t *mb_info) {
	cpu_disable_interrupts();
	serial8086_init();
	gdt_init();
	idt_init();
	pic_init();
	pmm_init(mb_info);
	vmm_init();
	heap_init();
	pci_init();
	init_ctors(); // this is a temporary solution for drivers,
		      // but you don't need to remove it (when there are modules)
	input_init();
	video_init();
	tty_init();
	cpu_enable_interrupts();

	log_ok("Kernel successfully launched\n");

	kprintf("\n"
"                    .-=====-.\n"
"                    | .\"\"\". |\n"
"                    | |   | |\n"
"                    | |   | |\n"
"                    | '---' |\n"
"                    |       |\n"
"                    |       |\n"
" .-================-'       '-================-.\n"
"j|  _                                          |\n"
"g|.'o\\                                    __   |\n"
"s| '-.'.                                .'o.`  |\n"
" '-==='.'.=========-.       .-========.'.-'===-'\n"
"        '.`'._    .===,     |     _.-' /\n"
"          '._ '-./  ,//\\   _| _.-'  _.'\n"
"             '-.| ,//'  \\-'  `   .-'\n"
"                `//'_`--;    ;.-'\n"
"                  `\\._ ;|    |\n"
"                     \\`-'  . |\n"
"                     |_.-'-._|\n"
"                     \\  _'_  /\n"
"                     |; -:- | \n"
"                     || -.- \\ \n"
"                     |;     .\\ \n"
"                     / `'\\'`\\-;\n"
"                    ;`   '. `_/\n"
"                    |,`-._;  .;\n"
"                    `;\\  `.-'-;\n"
"                     | \\   \\  |\n"
"                     |  `\\  \\ |\n"
"                     |   )  | |\n"
"                     |  /  /` /\n"
"                     | |  /|  |\n"
"                     | | / | /\n"
"                     | / |/ /|\n"
"                     | \\ / / |\n"
"                     |  /o | |\n"
"                     |  |_/  |	\n"
"                     |       | \n"
"                     |       |	\n"
"                     |       |	\n"
"                     |       |	\n"
"                     |       |	\n"
"                     |       |	\n"
"                     |       |	\n"
"                     '-=====-'\n"
"                                           \n"
"  +---------------------------------------+\n"
"  | MY OS LOADED                          |\n"
"  | x86 Operating System                  |\n"
"  | Version: 0.1                          |\n"
"  | By: holu31                            |\n"
"  +---------------------------------------+\n"
"                                           \n"
"           \"Ad Maiorem Dei Gloriam\"            \n"
"   \"In hoc signo vinces\" • \"In God We Trust\"  \n");

	while (true) {
		if (input_has_events()) {
			key_event e = input_pop_event();						
			tty_handle_keypress(e);
		}
	}


}
