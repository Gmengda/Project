#pragma once
#include "CommonH.h"

class PageCache
{
public:
	static PageCache* GetPageCache()
	{
		return &_sInstan;
	}

	//获取一个k页的Span
	Span* NewSpan(size_t k);
	//获取从对象到span 的映射
	Span* MapObjectToSpan(void* obj);
	//释放空闲的Span回到pageCache
	void ReleasepanToPageCache(Span* span);

	std::mutex& getMutex()
	{
		return _pageMtx;
	}

private:
	SpanList _spanList[NPAGES];
	std::mutex _pageMtx;

	std::unordered_map<PAGE_ID, Span*> _idSpanMap;

	PageCache()
	{ }
	PageCache(const PageCache&) = delete;
	static PageCache _sInstan;
};