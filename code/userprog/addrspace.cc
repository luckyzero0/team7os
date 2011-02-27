// addrspace.cc 
//	Routines to manage address spaces (executing user programs).
//
//	In order to run a user program, you must:
//
//	1. link with the -N -T 0 option 
//	2. run coff2noff to convert the object file to Nachos format
//		(Nachos object code format is essentially just a simpler
//		version of the UNIX executable object code format)
//	3. load the NOFF file into the Nachos file system
//		(if you haven't implemented the file system yet, you
//		don't need to do this last step)
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "addrspace.h"
#include "noff.h"
#include "table.h"
#include "synch.h"

extern "C" { int bzero(char *, int); };

Table::Table(int s) : map(s), table(0), lock(0), size(s) {
	table = new void *[size];
	lock = new Lock("TableLock");
}

Table::~Table() {
	if (table) {
		delete table;
		table = 0;
	}
	if (lock) {
		delete lock;
		lock = 0;
	}
}

void *Table::Get(int i) {
	// Return the element associated with the given if, or 0 if
	// there is none.

	return (i >=0 && i < size && map.Test(i)) ? table[i] : 0;
}

int Table::Put(void *f) {
	// Put the element in the table and return the slot it used.  Use a
	// lock so 2 files don't get the same space.
	int i;	// to find the next slot

	lock->Acquire();
	i = map.Find();
	lock->Release();
	if ( i != -1)
		table[i] = f;
	return i;
}

void *Table::Remove(int i) {
	// Remove the element associated with identifier i from the table,
	// and return it.

	void *f =0;

	if ( i >= 0 && i < size ) {
		lock->Acquire();
		if ( map.Test(i) ) {
			map.Clear(i);
			f = table[i];
			table[i] = 0;
		}
		lock->Release();
	}
	return f;
}

//----------------------------------------------------------------------
// SwapHeader
// 	Do little endian to big endian conversion on the bytes in the 
//	object file header, in case the file was generated on a little
//	endian machine, and we're now running on a big endian machine.
//----------------------------------------------------------------------

static void 
	SwapHeader (NoffHeader *noffH)
{
	noffH->noffMagic = WordToHost(noffH->noffMagic);
	noffH->code.size = WordToHost(noffH->code.size);
	noffH->code.virtualAddr = WordToHost(noffH->code.virtualAddr);
	noffH->code.inFileAddr = WordToHost(noffH->code.inFileAddr);
	noffH->initData.size = WordToHost(noffH->initData.size);
	noffH->initData.virtualAddr = WordToHost(noffH->initData.virtualAddr);
	noffH->initData.inFileAddr = WordToHost(noffH->initData.inFileAddr);
	noffH->uninitData.size = WordToHost(noffH->uninitData.size);
	noffH->uninitData.virtualAddr = WordToHost(noffH->uninitData.virtualAddr);
	noffH->uninitData.inFileAddr = WordToHost(noffH->uninitData.inFileAddr);
}

//----------------------------------------------------------------------
// AddrSpace::AddrSpace
// 	Create an address space to run a user program.
//	Load the program from a file "executable", and set everything
//	up so that we can start executing user instructions.
//
//	Assumes that the object code file is in NOFF format.
//
//	"executable" is the file containing the object code to load into memory
//
//      It's possible to fail to fully construct the address space for
//      several reasons, including being unable to allocate memory,
//      and being unable to read key parts of the executable.
//      Incompletely consretucted address spaces have the member
//      constructed set to false.
//----------------------------------------------------------------------

AddrSpace::AddrSpace(OpenFile *executable) : fileTable(MaxOpenFiles) {
	NoffHeader noffH;
	unsigned int i, size;

	constructedSuccessfully = false;

	// Don't allocate the input or output to disk files
	fileTable.Put(0);
	fileTable.Put(0);

	executable->ReadAt((char *)&noffH, sizeof(noffH), 0);
	if ((noffH.noffMagic != NOFFMAGIC) && 
		(WordToHost(noffH.noffMagic) == NOFFMAGIC))
		SwapHeader(&noffH);
	if(noffH.noffMagic != NOFFMAGIC) {
		printf("Tried to construct an AddressSpace with a non NOFF file.\n");
		return;
	}

	size = noffH.code.size + noffH.initData.size + noffH.uninitData.size ;
	numPages = divRoundUp(size, PageSize) + divRoundUp(UserStackSize,PageSize);
	mainThreadStartVPN = divRoundUp(size,PageSize);
	// we need to increase the size
	// to leave room for the stack
	size = numPages * PageSize;
	codeSize = noffH.code.size;

	if(numPages > NumPhysPages) {
		printf("Not enough physical pages in total for this AddressSpace to allocate.\n");
		return;
	}		// check we're not trying
	// to run anything too big --
	// at least until we have
	// virtual memory

	numThreads = 0;

	

	DEBUG('a', "Initializing address space, num pages %d, size %d\n", 
		numPages, size);
	// first, set up the translation 
	pageTable = new TranslationEntry[numPages];

	int startPPN = getContiguousPhysicalPages(numPages);
	if (startPPN == -1) {
		printf("We failed to allocate %d contiguous physical pages, so the process did not create correctly!\n", numPages);
		return;
	}
	for (i = 0; i < numPages; i++) {
		pageTable[i].virtualPage = i;	// for now, virtual page # = phys page #
		pageTable[i].physicalPage = startPPN + i;
		pageTable[i].valid = TRUE;
		pageTable[i].use = FALSE;
		pageTable[i].dirty = FALSE;
		pageTable[i].readOnly = FALSE;  // if the code segment was entirely on 
		// a separate page, we could set its 
		// pages to be read-only
	}

	// zero out the entire address space, to zero the unitialized data segment 
	// and the stack segment
	//   bzero(machine->mainMemory, size);

	// then, copy in the code and data segments into memory
	if (noffH.code.size > 0) {
		DEBUG('a', "Initializing code segment, at 0x%x, size %d\n", 
			noffH.code.virtualAddr, noffH.code.size);
		executable->ReadAt(&(machine->mainMemory[pageTable[0].physicalPage * PageSize]),
			noffH.code.size, noffH.code.inFileAddr);
	}
	if (noffH.initData.size > 0) {
		DEBUG('a', "Initializing data segment, at 0x%x, size %d\n", 
			noffH.initData.virtualAddr, noffH.initData.size);
		executable->ReadAt(&(machine->mainMemory[pageTable[0].physicalPage * PageSize + noffH.code.size]),
			noffH.initData.size, noffH.initData.inFileAddr);
	}

	constructedSuccessfully = true;

}

//----------------------------------------------------------------------
// AddrSpace::~AddrSpace
//
// 	Dealloate an address space.  release pages, page tables, files
// 	and file tables
//----------------------------------------------------------------------

AddrSpace::~AddrSpace()
{
	for (int i = 0; i < numPages; i++) {
		giveUpPhysicalPage(i);
	}
	delete pageTable;
}

int AddrSpace::getMainThreadStartVPN() {
	return mainThreadStartVPN;
}

int AddrSpace::getNumPages() {
	return numPages;
}

int AddrSpace::getCodeSize(){
	return codeSize;
}

bool AddrSpace::didConstructSuccessfully() {
	return constructedSuccessfully;
}

void AddrSpace::AddNewThread(Thread* newThread) {
	numThreads++;
	int startVPN = getStartVPN();
	newThread->startVPN = startVPN;
	if (startVPN >= numPages) {
		// copy and remake our pageTable
		TranslationEntry* newPageTable = new TranslationEntry[numPages + UserStackSize / PageSize];
		for (int i = 0; i < numPages; i++) { //copy the old entries
			newPageTable[i] = pageTable[i];
		}
		delete pageTable;
		pageTable = newPageTable;
		numPages += UserStackSize / PageSize;
		// restoreState (since we changed numPages and pageTable pointer)
	}

	for (int i = 0; i < UserStackSize / PageSize; i++) {
		pageTable[startVPN + i].virtualPage = startVPN + i;
		int physPage = getPhysicalPage();
		if (physPage == -1) {
			printf("Ran out of physical pages!");
			return;
		}
		pageTable[startVPN + i].physicalPage = physPage;
		pageTable[startVPN + i].valid = TRUE;
		pageTable[startVPN + i].use = FALSE;
		pageTable[startVPN + i].dirty = FALSE;
		pageTable[startVPN + i].readOnly = FALSE;
	}
	
	for(int i = 0; i <numPages; i++)
	{
		DEBUG('a',"pageTable[%d]. Physical Page = [%d]. Valid = [%d]\n",i,pageTable[i].physicalPage,pageTable[i].valid);
	}
}

int AddrSpace::getStartVPN() {
	int NUM_STACK_PAGES = UserStackSize / PageSize;

	//try to get NUM_PAGES contiguous virtual pages
	int startVPN = numPages;
	bool done = false;
	for (int i = 0; i < numPages; i++) {
		if (!pageTable[i].valid) { //if we find some invalid Vaddr try to reuse
			for (int j = 0; j < NUM_STACK_PAGES; j++) {
				if (pageTable[i + j].valid) { // try to find NUM_STACK_PAGES contiguous invalid VPNs
					break;
				} else if (j == NUM_STACK_PAGES - 1) {
					done = true; //we found 8 pages in a row
				}
			}
			if (done) {
				startVPN = i;
				break;
			}
		}
	}
	return startVPN;
}


void AddrSpace::RemoveCurrentThread() {
	numThreads--;
	int vpnStart = currentThread->startVPN;
	DEBUG('a', "Removing thread with startVPN = %d.\n", vpnStart);
	for (int i = 0; i < UserStackSize / PageSize; i++) { //mark the physical pages as free and the virtual pages as invalid
		int physPage = pageTable[vpnStart + i].physicalPage;
		DEBUG('a', "Giving up physical page = %d\n", physPage);
		giveUpPhysicalPage(physPage);
		pageTable[vpnStart + i].physicalPage = -1;
		pageTable[vpnStart + i].valid = FALSE;
	}
}
//----------------------------------------------------------------------
// AddrSpace::InitRegisters
// 	Set the initial values for the user-level register set.
//
// 	We write these directly into the "machine" registers, so
//	that we can immediately jump to user code.  Note that these
//	will be saved/restored into the currentThread->userRegisters
//	when this thread is context switched out.
//----------------------------------------------------------------------

void
	AddrSpace::InitRegisters()
{
	int i;

	for (i = 0; i < NumTotalRegs; i++)
		machine->WriteRegister(i, 0);

	// Initial program counter -- must be location of "Start"
	machine->WriteRegister(PCReg, 0);	

	// Need to also tell MIPS where next instruction is, because
	// of branch delay possibility
	machine->WriteRegister(NextPCReg, 4);

	// Set the stack register to the end of the address space, where we
	// allocated the stack; but subtract off a bit, to make sure we don't
	// accidentally reference off the end!
	machine->WriteRegister(StackReg, numPages * PageSize - 16);
	DEBUG('a', "Initializing stack register to %x\n", numPages * PageSize - 16);
}

//----------------------------------------------------------------------
// AddrSpace::SaveState
// 	On a context switch, save any machine state, specific
//	to this address space, that needs saving.
//
//	For now, nothing!
//----------------------------------------------------------------------

void AddrSpace::SaveState() 
{}

//----------------------------------------------------------------------
// AddrSpace::RestoreState
// 	On a context switch, restore the machine state so that
//	this address space can run.
//
//      For now, tell the machine where to find the page table.
//----------------------------------------------------------------------

void AddrSpace::RestoreState() 
{
	machine->pageTable = pageTable;
	machine->pageTableSize = numPages;
}
