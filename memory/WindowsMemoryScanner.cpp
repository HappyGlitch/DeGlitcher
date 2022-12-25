#include "WindowsMemoryScanner.h"
#include <Windows.h>
#define ULONG unsigned long long

deglitcher::memory::WindowsMemoryScanner::WindowsMemoryScanner(void* process) {
	openedProcess = process;
}

int deglitcher::memory::WindowsMemoryScanner::readValueFromAddress(ULONG address, void* output, size_t outputSize)
{
	return ReadProcessMemory(openedProcess, (LPCVOID)address, output, outputSize, NULL);
}

int deglitcher::memory::WindowsMemoryScanner::writeValueToAddress(ULONG address, void* input, size_t inputSize)
{
	return WriteProcessMemory(openedProcess, (LPVOID)address, input, inputSize, NULL);
}

ULONG incrementAddress(ULONG address, ULONG amount) {
	void* voidAddress = (void*)(address);
	char* charAddress = (char*)voidAddress + amount;
	voidAddress = (void*)charAddress;
	return (ULONG)voidAddress;
}

int areArraysEqual(void* array1, void* array2, size_t length) {
	char* array1Char = (char*)array1;
	char* array2Char = (char*)array2;
	for (int i = 0; i < length; i++) {
		if (array1Char[i] != array2Char[i])
			return 0;
	}
	return 1;
}

const unsigned int ALIGNMENT_IN_BYTES = 1;
void findAdressesWithValueInsideRegion(void* value, size_t length, void* process, std::vector<ULONG>* addressesFound, MEMORY_BASIC_INFORMATION region, ULONG address) {
	std::vector<char> memory(region.RegionSize);
	ReadProcessMemory(process, (LPCVOID)address, &memory[0], memory.size(), NULL);
	for (ULONG i = 0; i <= memory.size() - length; i += ALIGNMENT_IN_BYTES) {
		if (areArraysEqual(&memory[i], value, length)) {
			addressesFound->push_back(incrementAddress(address, i));
		}
	}
}

int isRegionWritable(MEMORY_BASIC_INFORMATION region) {
	return region.State == MEM_COMMIT && (region.Protect == PAGE_EXECUTE_READWRITE || region.Protect == PAGE_READWRITE);
}

void pause(void* process) {
	DebugActiveProcess(GetProcessId(process));
	DebugSetProcessKillOnExit(0);
}

void unpause(void* process) {
	DebugActiveProcessStop(GetProcessId(process));
}

std::vector<ULONG> deglitcher::memory::WindowsMemoryScanner::findAddressesWithValue(void* value, size_t length)
{
	pause(openedProcess);

	std::vector<ULONG> addressesFound;
	MEMORY_BASIC_INFORMATION region;
	for (ULONG address = 0; VirtualQueryEx(openedProcess, (LPCVOID)address, &region, sizeof(region)); address = incrementAddress(address, region.RegionSize)) {
		if (!isRegionWritable(region))
			continue;
		findAdressesWithValueInsideRegion(value, length, openedProcess, &addressesFound, region, address);
	}

	unpause(openedProcess);
	return addressesFound;
}

std::vector<ULONG> deglitcher::memory::WindowsMemoryScanner::filterAddressesWithValue(const std::vector<ULONG>* addressesToFilter, void* value, size_t length)
{
	pause(openedProcess);

	std::vector<ULONG> addressesFound;
	MEMORY_BASIC_INFORMATION region;
	std::vector<ULONG> memory;
	ULONG regionEnd = 0;
	for (ULONG i = 0; i < addressesToFilter->size(); i++) {
		ULONG address = (*addressesToFilter)[i];
		std::vector<char> read(length);
		if (ReadProcessMemory(openedProcess, (LPCVOID)address, &read[0], length, NULL)) {
			if (areArraysEqual(&read[0], value, length))
				addressesFound.push_back(address);
		}
	}

	unpause(openedProcess);
	return addressesFound;
}

#undef ULONG