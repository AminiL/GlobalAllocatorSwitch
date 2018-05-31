#pragma once
#include <memory>
class IMemoryManager {
public:
	virtual void* Alloc(size_t size) = 0;
	virtual void Free(void *ptr) = 0;
};
class std_allocator : public IMemoryManager {
public:
	std_allocator() {}
	void* Alloc(size_t size) {
		return std::malloc(size);
	}
	void Free(void *ptr) {
		std::free(ptr);
	}
};


class CMemoryManagerSwitcher {
public:
	static IMemoryManager* now_;
	IMemoryManager* prev_;
	CMemoryManagerSwitcher(IMemoryManager* now) {
		prev_ = now_;
		now_ = now;
	}

	~CMemoryManagerSwitcher() {
		now_ = prev_;
	}
};


void *operator new(size_t size);

void operator delete(void *ptr);

class RuntimeHeap {
public:
	static void* Alloc(size_t size) {
		return std::malloc(size);
	}
	static void Free(void *ptr) {
		std::free(ptr);
	}
};

class CurrentMemoryManager {
public:
	static void* Alloc(size_t size) {
		return operator new(size);
	}
	static void Free(void *ptr) {
		delete ptr;
	}
};

template <class default_allocator>
class CAllocatedOn {
public:
	void* operator new (size_t size) {
		return default_allocator::Alloc(size);
	}
	void operator delete (void *ptr) {
		default_allocator::Free(ptr);
	}
};

