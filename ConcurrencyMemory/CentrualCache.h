#pragma once
#include "CommonH.h"

//单例模式
class CentrualCache
{
public:
	static CentrualCache* GetInstance()
	{
		return &_sInst;
	}
	//从中心缓存获取一定数量的对象给thread cache
	size_t FetchRangeObj(void*& start,void*& end,size_t n,size_t size);
	//在SpanList 或 page cache 获取一个非空的span
	Span* GetOneSpan(SpanList& list, size_t size);

	//将一定数量的对象挂到桶里
	void ReleaseListToSpan(void* start, size_t size);

private:
	SpanList _spanLists[NFREELITS];
private:

	CentrualCache()
	{ }
	CentrualCache(const CentrualCache&) = delete;

	static CentrualCache _sInst;
};