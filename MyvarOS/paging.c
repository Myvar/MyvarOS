#include "paging.h"

extern unsigned int Page_Table1[1024 * 1024];	// # of entries/page table * total # of page tables 
												//		actual size = 4194304 bytes = 4MiB, represents 4GiB in physical memory (size of unsigned int = 4 bytes)
												//		ie. each 4 byte entry represent 4 KiB in physical memory
extern unsigned int Page_Directory[1024 * 1];  	// # of pages tables * # of directory (4096 bytes = 4 KiB)

#define KERNEL_VIRTUAL_BASE 0xC0000000					// Constant declaring base of Higher-half kernel (from Kernel.asm)		 
#define KERNEL_PAGE_TABLE (KERNEL_VIRTUAL_BASE >> 22)	// Constant declaring Page Table index in virtual memory (from Kernel.asm)

void Init_Paging()
{
	unsigned int PhysicalAddressAndFlags = 7; 	// ; 0b111 - Setting Page Table flags (Present: ON, Read/Write: ON, User/Supervisor: ON)
	unsigned int NoOfPageTables = 4; 			// 4 is arbitrary to cover 16MiB
	unsigned int EntriesPerPageTable = 1024;	// There is always 1024 pages (4KiB/Page)
	unsigned int StartPageTableEntryIndex = 0;
	unsigned int SizeOfPageTables = NoOfPageTables * EntriesPerPageTable;
	unsigned int index = 0;
	
	unsigned int* Page_Table1_Physical = (unsigned int*)((unsigned int)Page_Table1 - KERNEL_VIRTUAL_BASE);
	unsigned int* Page_Directory_Physical = (unsigned int*)((unsigned int)Page_Directory - KERNEL_VIRTUAL_BASE);
	
	// Setting up identity mapping for the full address range. -kmcg
	while(index < 1024 * 1024)
	{
		Page_Table1_Physical[index] = PhysicalAddressAndFlags;
		index = index + 1;
		PhysicalAddressAndFlags = PhysicalAddressAndFlags + 4096;
	}
	
	PhysicalAddressAndFlags = 7;
	index = (KERNEL_PAGE_TABLE * 1024);
	StartPageTableEntryIndex = (KERNEL_PAGE_TABLE * 1024);

	// Overwrite the portion for the kernel.
	while(index < (SizeOfPageTables + StartPageTableEntryIndex))
	{
		Page_Table1_Physical[index] = PhysicalAddressAndFlags;
		index = index + 1;
		PhysicalAddressAndFlags = PhysicalAddressAndFlags + 4096;
	}
	
	PhysicalAddressAndFlags = (unsigned int)&Page_Table1_Physical[0];
	PhysicalAddressAndFlags = PhysicalAddressAndFlags | 7;	//0b111 - Setting Page Table flags (Present: ON, Read/Write: ON, User/Supervisor: ON)
	
	unsigned int EntriesOfPageDirectory = 1024;
	index = 0; 	// Setting index to 0 to be used with Page Directory
	
	while(index < EntriesOfPageDirectory)
	{	
		Page_Directory_Physical[index] = PhysicalAddressAndFlags;
		index = index + 1;	// Move to next entry in Page Directory (4 bytes down)
		PhysicalAddressAndFlags = PhysicalAddressAndFlags + 4096; 	// Update physical address to which to set the next Page Directory entry to (4 KiB down)
	}
	   
	   
	// Set virtual addressing via control register CR3 
	// high 20 bits is Page directory Base Register i.e physical address of first page directory entry
	__asm__ 
	(
		"lea ECX, [Page_Directory - 0xC0000000]\n" // 0xC0000000 = KERNEL_VIRTUAL_BASE
		"mov CR3, ECX\n"
	);	
	
	// Switch on paging via control register CR0
	__asm__
	(
		"mov ECX, CR0\n"
		"or ECX, 0x80000000\n"	// Set PG bit in CR0 to enable paging.
		"mov CR0, ECX\n"
	);

	// At only this point we are in physical higher-half mode
}
