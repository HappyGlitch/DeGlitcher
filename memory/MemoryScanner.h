#pragma once
#include <vector>
#define ULONG unsigned long long

namespace deglitcher {
	namespace memory {
		class MemoryScanner {
		public:
			~MemoryScanner() {}
			virtual int readValueFromAddress(ULONG address, void* output, size_t outputSize) = 0;
			virtual int writeValueToAddress(ULONG address, void* input, size_t inputSize) = 0;
			virtual std::vector<ULONG> findAddressesWithValue(void* value, size_t length) = 0;
			virtual std::vector<ULONG> filterAddressesWithValue(const std::vector<ULONG>* addressesToFilter, void* value, size_t length) = 0;
		};
	}
}

#undef ULONG