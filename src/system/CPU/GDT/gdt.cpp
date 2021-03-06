#include <drivers/display/serial/serial.hpp>
#include <system/memory/heap/heap.hpp>
#include <system/CPU/GDT/gdt.hpp>
#include <kernel/main.hpp>
#include <lib/memory/memory.hpp>
#include <lib/lock.hpp>
#include <system/CPU/SMP/smp.hpp>
#include <kernel/kernel.hpp>

namespace turbo::gdt {

	[[gnu::aligned(0x1000)]]
	GDT DefaultGDT = {
		{0x0000, 0, 0, 0x00, 0x00, 0}, // NULL
		{0xFFFF, 0, 0, 0x9A, 0x00, 0}, // 16 Bit code
		{0xFFFF, 0, 0, 0x92, 0x00, 0}, // 16 Bit data
		{0xFFFF, 0, 0, 0x9A, 0xCF, 0}, // 32 Bit code
		{0xFFFF, 0, 0, 0x92, 0xCF, 0}, // 32 Bit data
		{0x0000, 0, 0, 0x9A, 0x20, 0}, // 64 Bit code
		{0x0000, 0, 0, 0x92, 0x00, 0}, // 64 Bit data
		{0x0000, 0, 0, 0xF2, 0x00, 0}, // User data
		{0x0000, 0, 0, 0xFA, 0x20, 0}, // User code
		{0x0000, 0, 0, 0x89, 0x00, 0, 0, 0} // TSS
};


	DEFINE_LOCK(gdt_lock);
	bool isInit = false;
	GDTDescriptor gdtDescriptor;
	TSS *tss;

	void reloadGDT(){
		loadGDT(&gdtDescriptor);
	}

	void reloadTSS(){
		loadTSS();
	}

	void reloadAll(int cpu){
		gdt_lock.lock();

		uintptr_t base = (uintptr_t)&tss[cpu];
		uintptr_t limit = base + sizeof(tss[cpu]);

		DefaultGDT.Tss.length = limit;
		DefaultGDT.Tss.base0 = base;
		DefaultGDT.Tss.base1 = base >> 16;
		DefaultGDT.Tss.flag1 = 0x89;
		DefaultGDT.Tss.flag2 = 0x00;
		DefaultGDT.Tss.base2 = base >> 24;
		DefaultGDT.Tss.base3 = base >> 32;
		DefaultGDT.Tss.Reserved = 0x00;

		reloadGDT();
		reloadTSS();

		gdt_lock.unlock();
	}

	void init(){
		serial::log("[+] Initialising GDT");

		if (isInit){
			serial::log("[!!] already init: GDT\n");
			return;
		}

		tss = (TSS*)calloc(smp_tag->cpu_count, sizeof(TSS));

		gdtDescriptor.size = sizeof(GDT) - 1;
		gdtDescriptor.offset = (uint64_t)&DefaultGDT;

		reloadAll(smp_tag->bsp_lapic_id);
		tss[0].RSP[0] = (uint64_t)kernelStack;

		serial::newline();
		isInit = true;
	}

	void setStack(uint64_t cpu, uintptr_t stack){
		tss[cpu].RSP[0] = stack;
	}

	uint64_t getStack(uint64_t cpu){
		return tss[cpu].RSP[0];
	}

	void setStack(uintptr_t stack){
		tss[thisCPU->lapicID].RSP[0] = stack;
	}

	uint64_t getStack(){
		return tss[thisCPU->lapicID].RSP[0];
	}
}
