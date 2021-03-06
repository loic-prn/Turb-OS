#include <drivers/display/terminal/terminal.hpp>
#include <drivers/display/serial/serial.hpp>
#include <system/ACPI/acpi.hpp>
#include <system/PCI/pci.hpp>
#include <kernel/kernel.hpp>
#include <lib/memory/memory.hpp>
#include <lib/string.hpp>
#include <lib/memory/mmio.hpp>
#include <lib/portIO.hpp>

using namespace turbo;

[[gnu::always_inline]] inline bool is_canonical(uint64_t addr){
    return ((addr <= 0x00007FFFFFFFFFFF) || ((addr >= 0xFFFF800000000000) && (addr <= 0xFFFFFFFFFFFFFFFF)));
}

namespace turbo::acpi {

	bool isInit = false;
	bool madt = false;

	bool useXSTD;
	RSDP *rsdp;

	MCFGHeader *mcfghdr;
	MADTHeader *madthdr;
	FADTHeader *fadthdr;
	HPETHeader *hpethdr;
	SDTHeader *rsdt;

	TurboVector<MADTLapic*> lapics;
	TurboVector<MADTIOApic*> ioapics;
	TurboVector<MADTIso*> isos;
	TurboVector<MADTnMaskInt*> nmis;

	uint32_t *SMI_CMD;
	uint8_t ACPI_ENABLE;
	uint8_t ACPI_DISABLE;
	uint32_t PM1a_CNT;
	uint32_t PM1b_CNT;
	uint16_t SLP_TYPa;
	uint16_t SLP_TYPb;
	uint16_t SLP_EN;
	uint16_t SCI_EN;
	uint8_t PM1_CNT_LEN;

	uintptr_t lapicAddress = 0;

	void madtInit(){
		serial::newline();

		lapics.init(1);
		ioapics.init(1);
		isos.init(1);
		nmis.init(1);

		lapicAddress = madthdr->localControllerAddress;
		
		for(uint8_t *madt_ptr = (uint8_t*)madthdr->entriesBegin; (uintptr_t)madt_ptr < (uintptr_t)madthdr + madthdr->sdt.length; madt_ptr += *(madt_ptr + 1)){
			switch(*(madt_ptr)){
				case 0:
					serial::log("ACPI/MADT: Found local APIC %ld", lapics.getLength());
					lapics.push_back((MADTLapic*)madt_ptr);
					break;

				case 1:
					serial::log("ACPI/MADT: Found I/O APIC %ld", ioapics.getLength());
					ioapics.push_back((MADTIOApic*)madt_ptr);
					break;

				case 2:
					serial::log("ACPI/MADT: Found ISO %ld", isos.getLength());
					isos.push_back((MADTIso*)madt_ptr);
					break;

				case 4:
					serial::log("ACPI/MADT: Found NMI %ld", nmis.getLength());
					nmis.push_back((MADTnMaskInt*)madt_ptr);
					break;

				case 5:
					lapicAddress = QWORD_PTR(madt_ptr + 4);
					break;
			}
		}
	}

	void dsdtInit(){
		uint64_t dsdtaddr = ((is_canonical(fadthdr->X_Dsdt) && useXSTD) ? fadthdr->X_Dsdt : fadthdr->Dsdt);
		uint8_t *S5Addr = (uint8_t*)dsdtaddr + 36;
		uint64_t dsdtlength = ((SDTHeader*)dsdtaddr)->length;

		dsdtaddr *= 2;
		while(dsdtlength-- > 0){
			if(!memcmp(S5Addr, "_S5_", 4)){
				break;
			}
			S5Addr++;
		}

		if(dsdtlength <= 0){
			serial::log("_S5 not present in ACPI");
			return;
		}

		if((*(S5Addr - 1) == 0x8 || (*(S5Addr - 2) == 0x8 && *(S5Addr - 1) == '\\')) && *(S5Addr + 4) == 0x12){
			S5Addr += 5;
			S5Addr += ((*S5Addr & 0xC0) >> 6) + 2;

			if(*S5Addr == 0xA){
				S5Addr++;
			}

			SLP_TYPb = *(S5Addr) << 10;
			SMI_CMD = (uint32_t*)((uintptr_t)fadthdr->SMI_CommandPort);

			ACPI_ENABLE = fadthdr->AcpiEnable;
			ACPI_DISABLE = fadthdr->AcpiDisable;

			PM1a_CNT = fadthdr->PM1aControlBlock;
			PM1b_CNT = fadthdr->PM1bControlBlock;

			PM1_CNT_LEN = fadthdr->PM1ControlLength;

			SLP_EN = 1 << 13;
			SCI_EN = 1;

			return;
		}

		serial::log("Failed to parse _S5 in ACPI");
		serial::log("ACPI shutdown may not be possible");
		SCI_EN = 0;
	}

	void shutdown(){
		if (SCI_EN == 1){
			outw(fadthdr->PM1aControlBlock, (inw(fadthdr->PM1aControlBlock) & 0xE3FF) | ((SLP_TYPa << 10) | ACPI_SLEEP));
			
			if(fadthdr->PM1bControlBlock){
				outw(fadthdr->PM1bControlBlock, (inw(fadthdr->PM1bControlBlock) & 0xE3FF) | ((SLP_TYPb << 10) | ACPI_SLEEP));
			}
			
			outw(PM1a_CNT, SLP_TYPa | SLP_EN);
			
			if(PM1b_CNT){
				outw(PM1b_CNT, SLP_TYPb | SLP_EN);
			}
		}
	}

	void reboot(){
		switch(fadthdr->ResetReg.addressSpace){
			case ACPI_GAS_MMIO:
				*((uint8_t*)((uintptr_t)fadthdr->ResetReg.address)) = fadthdr->ResetValue;
				break;

			case ACPI_GAS_IO:
				outb(fadthdr->ResetReg.address, fadthdr->ResetValue);
				break;

			case ACPI_GAS_PCI:
				pci::writeb(0, (fadthdr->ResetReg.address >> 32) & 0xFFFF, (fadthdr->ResetReg.address >> 16) & 0xFFFF, fadthdr->ResetReg.address & 0xFFFF, fadthdr->ResetValue);
				break;
		}
	}

	void *findtable(const char *signature, size_t skip){
		if(skip < 0){
			skip = 0;
		}
		
		size_t entries = (rsdt->length - sizeof(SDTHeader)) / (useXSTD ? 8 : 4);
		
		for(size_t i = 0; i < entries; i++){
			SDTHeader *newsdthdr;

			if(useXSTD){
				newsdthdr = (SDTHeader*)*(uint64_t*)((uint64_t)rsdt + sizeof(SDTHeader) + (i * 8));
			}
			else{
				newsdthdr = reinterpret_cast<SDTHeader*>(
					*reinterpret_cast<uint32_t*>(
						reinterpret_cast<uint64_t>(rsdt) + sizeof(SDTHeader) + (i * 4)
					)
				);
			}
			
			if(!newsdthdr || !strcmp((const char*)newsdthdr->signature, "")){
				continue;
			}

			if(!strncmp((const char*)newsdthdr->signature, signature, 4)){
				if(!skip){
					return newsdthdr;
				}
				else{
					skip--;
				}
			}
			else{
				continue;
			}
		}
		return 0;
	}

	void init(){
		serial::log("Initialising ACPI");

		if(isInit){
			serial::log("ACPI has already been isInit!\n");
			return;
		}

		rsdp = (RSDP*)rsdp_tag->rsdp;

		if(rsdp->revision >= 2 && rsdp->xsdtaddress){
			useXSTD = true;
			rsdt = (SDTHeader*)rsdp->xsdtaddress;
			serial::log("Found XSDT at: 0x%X", rsdp->xsdtaddress);
		}
		else{
			useXSTD = false;
			rsdt = (SDTHeader*)((uintptr_t)rsdp->rsdtaddress);
			serial::log("Found RSDT at: 0x%X", rsdp->rsdtaddress);
		}

		mcfghdr = (MCFGHeader*)findtable("MCFG", 0);
		madthdr = (MADTHeader*)findtable("APIC", 0);
		
		if(madthdr){
			madt = true;
		}
		
		fadthdr = (FADTHeader*)findtable("FACP", 0);
		hpethdr = (HPETHeader*)findtable("HPET", 0);

		outb(fadthdr->SMI_CommandPort, fadthdr->AcpiEnable);
		while(!(inw(fadthdr->PM1aControlBlock) & 1));

		if(madt){
			madtInit();
		}
		
		dsdtInit();

		serial::newline();
		isInit = true;
	}
}