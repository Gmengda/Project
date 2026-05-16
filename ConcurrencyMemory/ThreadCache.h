#pragma once
#include "CommonH.h"
#include "CentrualCache.h"
class ThreadCache
{
public:
	//申请和释放内存
	void* Allocate(size_t size);
	void Deallocate(void* ptr, size_t size);

	//从中心缓存获取对象
	void* FetchFromCentralCache(size_t index ,size_t size);
	//释放对象的时候发现悬挂的列表太长了，要释放回中心缓存
	void ListToolLong(FreeList& freeList, size_t size);
private:
	FreeList _freeLists[NFREELITS];
};
//pTLSThreadCache 恰好扮演了“每个线程独有指针”的角色，指向该线程自己的 ThreadCache 对象
static _declspec(thread) ThreadCache* pTLSThreadCache = nullptr;