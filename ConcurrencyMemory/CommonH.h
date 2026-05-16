#pragma once
#include <vector>
#include <time.h>
#include <assert.h>
#include <thread>
#include <iostream>
#include <mutex>
#include <algorithm>
#include <unordered_map>
#ifdef _WIN32
#include <windows.h>
#endif // _WIN32


using std::cout;
using std::endl;

//局限于该作用域 定义了一个静态常量，用于表示最大字节数256 KB
static const size_t MAX_BYTES = 256 * 1024;
static const size_t NFREELITS = 208;
static const size_t NPAGES = 129;
static const size_t PAGE_SHIFT = 13;

#ifdef _WIN64
typedef unsigned long long PAGE_ID;
#elif _WIN32
typedef size_t PAGE_ID;
#endif // _WIN32 _WIN64

inline static void* SystemAlloc(size_t kpage)
{
#ifdef _WIN32
	void* ptr = VirtualAlloc(0, kpage << 13, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
#else
	// linux下brk mmap等
#endif

	if (ptr == nullptr)
		throw std::bad_alloc();

	return ptr;
}


static void*& NextObj(void* obj)
{
	return *((void**)obj);
}

class FreeList
{
public:
	void Push(void* obj)
	{
		assert(obj != nullptr);
		//头插
		NextObj(obj) = _freeList;
		_freeList = obj;
		++_size;
		assert(_size <= 1024);
	}
	void PushRang(void* start, void* end,size_t size)
	{
		//头插
		NextObj(end) = _freeList;
		_freeList = start;
		_size += size;
	}
	void PopRange(void*& start, void*& end, size_t size)
	{
		assert(size <= _size);
		start = _freeList;
		end = start;
		for (size_t i = 0; i < size - 1; i++)
		{
			end = NextObj(end);
		}
		_freeList = NextObj(end);
		NextObj(end) = nullptr;
		_size -= size;
	}
	void* Pop()
	{
		assert(_freeList != nullptr);
		//头删
		void* obj = _freeList;
		_freeList = NextObj(obj);
		--_size;
		return obj;
	}
	bool Empty()
	{
		return _freeList == nullptr;
	}
	size_t& getMasSize()
	{
		return _maxSize;
	}
	size_t& getSize()
	{
		return _size;
	}
private:
	void* _freeList = nullptr;

	size_t _maxSize = 1;
	size_t _size = 0;
};


//计算对象大小的对齐映像规则
class SizeClass
{
public:
	static size_t _RoundUp(size_t size, size_t AlignSize)
	{
		size_t num = AlignSize;
		if (size % AlignSize != 0)
		{
			num = (size / AlignSize + 1)* AlignSize;
		}
		return num;
		//6 太变态了
		//return ((size + AlignSize - 1) & ~(AlignSize -1));
	}

	static size_t RoundUp(size_t size)
	{
		if (size <= 128)
		{
			return _RoundUp(size, 8);
		}
		else if (size <= 1024)
		{
			return _RoundUp(size, 16);
		}
		else if (size <= 8*1024)
		{
			return _RoundUp(size, 128);
		}
		else if (size <= 64*1024)
		{
			return _RoundUp(size, 1024);
		}
		else if (size <= 256*1024)
		{
			return _RoundUp(size, 8*1024);
		}
		else
		{
			assert(false);
			return 0;
		}
		return 0;
	}

	static inline size_t _Index(size_t bytes, size_t align_shift)
	{
		return ((bytes + (1 << align_shift) - 1) >> align_shift) - 1;
	}

	// 计算映射的哪一个自由链表桶
	static inline size_t Index(size_t bytes)
	{
		assert(bytes <= MAX_BYTES);

		// 每个区间有多少个链
		static int group_array[4] = { 16, 56, 56, 56 };
		if (bytes <= 128) {
			return _Index(bytes, 3);
		}
		else if (bytes <= 1024) {
			return _Index(bytes - 128, 4) + group_array[0];
		}
		else if (bytes <= 8 * 1024) {
			return _Index(bytes - 1024, 7) + group_array[1] + group_array[0];
		}
		else if (bytes <= 64 * 1024) {
			return _Index(bytes - 8 * 1024, 10) + group_array[2] + group_array[1] + group_array[0];
		}
		else if (bytes <= 256 * 1024) {
			return _Index(bytes - 64 * 1024, 13) + group_array[3] + group_array[2] + group_array[1] + group_array[0];
		}
		else {
			assert(false);
		}

		return -1;
	}
	//一次thread cache 从中心缓存获取多少个对象
	static size_t NumMoveSize(size_t size)
	{
		assert(size > 0);
		size_t num = MAX_BYTES / size;
		if (num < 2)
		{
			num = 2;
		}
		if (num > 512)
		{
			num = 512;
		}
		return num;
	}
	//一次Centrual cache 从PageCache获取多少个页
	static size_t NumMovePage(size_t size)
	{
		size_t num = NumMoveSize(size);
		size_t npage = num * size;

		//计算所需空间大概要多少页
		npage >>= PAGE_SHIFT;
		if (npage == 0)
		{
			npage = 1;
		}
		return npage;
	}

};

//管理对各连续页大块内存的
struct Span
{
	PAGE_ID _pageId = 0;			//大块内存起始页数
	size_t _n = 0;				//页的数量

	Span* _prev = nullptr;
	Span* _next = nullptr;

	size_t _useCount = 0;		//切好的小块内存 被分到记数
	void* _freeList = nullptr;  //切好的小块内存自由链表

	bool _isUse = false;
};

class SpanList
{
public:
	SpanList()
	{
		_hand = new Span;
		_hand->_prev = _hand;
		_hand->_next = _hand;
	}

	Span* Begin()
	{
		return _hand->_next;
	}
	Span* end()
	{
		return _hand;
	}
	bool Empty()
	{
		return _hand->_next == _hand;
	}

	void PushFront(Span* newSpan)
	{
		Insert(Begin(), newSpan);
	}
	Span* PopFront()
	{
		Span* front = _hand->_next;
		Rease(_hand->_next);
		return front;
	}

	void Insert(Span* pos, Span* newSpan)
	{
		assert(pos);
		assert(newSpan);
		Span* prev = pos->_prev;
		prev->_next = newSpan;
		newSpan->_prev = prev;
		newSpan->_next = pos;
		pos->_prev = newSpan;

	}
	void Rease(Span* pos)
	{
		assert(pos);
		assert(pos != _hand);
		Span* prev = pos->_prev;
		Span* next = pos->_next;
		next->_prev = prev;
		prev->_next = next;
		
	}

private:
	Span* _hand;
public:
	std::mutex _mtx;   //桶锁  限制进出
};