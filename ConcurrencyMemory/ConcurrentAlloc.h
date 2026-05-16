#pragma once
#include "CommonH.h"
#include "ThreadCache.h"

static void* ConcurrentAlloc(size_t size)
{
	if (pTLSThreadCache == nullptr)
	{
		//通过TLS 每个线程无锁的获取自己专属的ThreadCache对象
		pTLSThreadCache = new ThreadCache;
	}
	cout << std::this_thread::get_id() << ":" << pTLSThreadCache << endl;
	return pTLSThreadCache->Allocate(size);
}

static void ConcurrentFree(void* ptr , size_t size)
{
	assert(pTLSThreadCache);
	pTLSThreadCache->Deallocate(ptr, size);
}