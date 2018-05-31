#include "ALACTRACTOR.h"
#include <iostream>
#include <algorithm>
#include <memory>
constexpr int real_sz = std::max(sizeof(max_align_t), sizeof(uintptr_t));
constexpr int r = sizeof(max_align_t);
constexpr int tmp = ((real_sz / r) * r);
constexpr int pointer_sz = (tmp < real_sz ? tmp + r : tmp);
IMemoryManager *CMemoryManagerSwitcher::now_ = nullptr;
void *operator new(size_t size) {
	auto need = pointer_sz + size;
	need /= alignof(max_align_t);
	need *= alignof(max_align_t);
	if (need < size + sizeof(std::uintptr_t))
		need += alignof(max_align_t);
	auto tmp = ((!CMemoryManagerSwitcher::now_) ? (std::malloc(need)) : (CMemoryManagerSwitcher::now_->Alloc(need)));
	auto tmp_allocator = reinterpret_cast <IMemoryManager **> (tmp);
	*tmp_allocator = CMemoryManagerSwitcher::now_;
	return reinterpret_cast < void *> (reinterpret_cast <std::uintptr_t>(tmp) + pointer_sz);
}
void operator delete(void *ptr) {
	std::uintptr_t begin_of_allocator = reinterpret_cast <std::uintptr_t> (ptr) - pointer_sz;
	auto get_allocator = reinterpret_cast <IMemoryManager **> (begin_of_allocator);
	if (*get_allocator == nullptr) {
		std::free(ptr);
		return;
	}
	(*get_allocator)->Free(reinterpret_cast <void *> (begin_of_allocator));
}