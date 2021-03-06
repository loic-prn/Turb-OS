#pragma once

#include <lib/cpu/cpu.hpp>
#include <stdint.h>

namespace turbo::idt {

	#define SYSCALL 0x69

	enum IRQS{
		IRQ0 = 32,
		IRQ1 = 33,
		IRQ2 = 34,
		IRQ3 = 35,
		IRQ4 = 36,
		IRQ5 = 37,
		IRQ6 = 38,
		IRQ7 = 39,
		IRQ8 = 40,
		IRQ9 = 41,
		IRQ10 = 42,
		IRQ11 = 43,
		IRQ12 = 44,
		IRQ13 = 45,
		IRQ14 = 46,
		IRQ15 = 47,
	};

	struct [[gnu::packed]] IDTEntry{
		uint16_t Offset1;
		uint16_t Selector;
		uint8_t IST;
		uint8_t TypeAttr;
		uint16_t Offset2;
		uint32_t Offset3;
		uint32_t Zero;
	};

	struct [[gnu::packed]] IDTPtr{
		uint16_t Limit;
		uint64_t Base;
	};

	using intHandler_t = void (*)(registers_t *);

	extern IDTEntry idt[];
	extern IDTPtr idtr;

	extern intHandler_t interrupt_handlers[];

	extern bool isInit;

	void idtSetDescriptor(uint8_t vector, void *isr, uint8_t typeattr = 0x8E, uint8_t ist = 0);
	void reload();

	extern "C" void *int_table[];
	void init();

	uint8_t allocVector();
	void registerInterruptHandler(uint8_t vector, intHandler_t handler, bool ioapic = true);
}