#pragma once
#include "MemoryScanner.h"
#define ULONG unsigned long long

namespace deglitcher {
	namespace memory {
		class WindowsMemoryScanner : public MemoryScanner {
		private:
			void* openedProcess;
		public:
			WindowsMemoryScanner(void* process);
			int readValueFromAddress(ULONG address, void* output, size_t outputSize) override;
			int writeValueToAddress(ULONG address, void* input, size_t inputSize) override;
			std::vector<ULONG> findAddressesWithValue(void* value, size_t length) override;
			std::vector<ULONG> filterAddressesWithValue(const std::vector<ULONG>* addressesToFilter, void* value, size_t length) override;

		};
	}
}
#undef ULONG