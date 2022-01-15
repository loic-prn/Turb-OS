#pragma once

#include <system/CPU/GDT/gdt.hpp>
#include <stddef.h>

namespace turbo::smp {

	struct cpu_t {
		uint64_t cpuID;
		uint32_t lapicID;
		turbo::gdt::TSS* tss;
		size_t fpuStorageSize;
		void (*fpuSave)(void*);
		void (*fpuRestore)(void*);
		volatile bool isUp;
	};

	extern cpu_t *cpus;
	extern bool isInit;

	void init();

	#define thisCPU \
	({ \
		uint64_t cpuNumber; \
		asm volatile("movq %%gs:[0], %0" : "=r"(cpuNumber) : : "memory"); \
		&turbo::smp::cpus[cpuNumber]; \
	})
}