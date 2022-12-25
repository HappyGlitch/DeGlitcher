#include "deglitcher/memory/WindowsMemoryScanner.h"
#include <windows.h>

using namespace std;

deglitcher::memory::MemoryScanner* scanner;
std::vector<unsigned long long> scan;

void found() {
	int i;
	std::cin >> i;
	int currentValue;
	scanner->readValueFromAddress(scan[0], &currentValue, sizeof(currentValue));
	std::cout << "Current value at address: " << currentValue << "\n>>";
	scanner->writeValueToAddress(scan[0], &i, sizeof(i));
	found();
}

void input() {
	std::string i;
	std::cout << "Found " << scan.size() << "results.\n>>";
	if (scan.size() == 1)
		found();
	else {
		std::cin >> i;
		if (i == "a") {
			int index;
			std::cin >> index;
			unsigned long long address = scan[index];
			scan = std::vector<unsigned long long>();
			scan.push_back(address);
			found();
		}
		int value = atoi(i.c_str());
		scan = scanner->filterAddressesWithValue(&scan, &value, sizeof(value));
		input();
	}
}

int main()
{
	std::cout << "Enter debugee id:\n>>";
	int i;
	std::cin >> i;
	scanner = new deglitcher::memory::WindowsMemoryScanner(OpenProcess(PROCESS_ALL_ACCESS, 0, i));
	std::cout << ">>";
	std::cin >> i;
	scan = scanner->findAddressesWithValue(&i, sizeof(i));
	input();
	delete scanner;
	return 0;
}
