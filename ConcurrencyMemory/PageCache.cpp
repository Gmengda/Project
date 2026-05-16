#include "PageCache.h"

PageCache PageCache::_sInstan;

//获取一个k页的Span
Span* PageCache::NewSpan(size_t k)
{
	//检查一下第K个桶里有没有span
	assert(k > 0 && k < NPAGES);
	if (!_spanList[k].Empty())
	{
		return _spanList[k].PopFront();
	}
	//检查一下后面的桶有没有更大的span进行切分使用
	for (size_t i = k + 1; i < NPAGES; i++)
	{
		if (!_spanList[i].Empty())
		{
			Span* nSpan = _spanList[i].PopFront();
			Span* kSpan = new Span;
			//在nSpan头部切一个k页下来
			kSpan->_pageId = nSpan->_pageId;
			kSpan->_n = k;
			nSpan->_pageId += k;
			nSpan->_n -= k;
			//把剩下的nSpan 挂回去
			_spanList[nSpan->_n].PushFront(nSpan);

			//存储nSpan的首位页号和末尾页号 与span的映射 
			//方便pagecache 回收内存时进行内存查找
			_idSpanMap[nSpan->_pageId] = nSpan;
			_idSpanMap[nSpan->_pageId + nSpan->_n - 1] = nSpan;

			//建立 id 和 span 的映射 方便centrual cache返回内存时 找到对应的span
			for (PAGE_ID i = 0; i < kSpan->_n; i++)
			{
				_idSpanMap[kSpan->_pageId + i] = kSpan;
			}

			return kSpan;
		}
	}
	//1-128都没有空间 就去和堆要空间
	Span* bigSpan = new Span;
	void* ptr = SystemAlloc(NPAGES - 1);
	bigSpan->_pageId = (PAGE_ID)ptr >> PAGE_SHIFT;
	bigSpan->_n = NPAGES - 1;

	_spanList[bigSpan->_n].PushFront(bigSpan);

	return NewSpan(k);
}

Span* PageCache::MapObjectToSpan(void* obj)
{
	//算出页号
	PAGE_ID id = (PAGE_ID)obj >> PAGE_SHIFT;
	auto ret = _idSpanMap.find(id);
	if (ret != _idSpanMap.end())
	{
		return ret->second;
	}
	assert(false);
	return nullptr;
}

void PageCache::ReleasepanToPageCache(Span* span)
{
	//尝试对前页进行合并
	while (1)
	{
		PAGE_ID prevId = span->_pageId - 1;
		auto ret = _idSpanMap.find(prevId);
		if (ret == _idSpanMap.end())
		{
			break;
		}

		Span* prevSpan = ret->second;
		if (prevSpan->_isUse)
		{
			break;
		}
		//合并超出128页的span 没有办法管理
		if (prevSpan->_n + span->_n > NPAGES - 1)
		{
			break;
		}
		span->_pageId = prevSpan->_pageId;
		span->_n += prevSpan->_n;

		_spanList[prevSpan->_n].Rease(prevSpan);
		delete prevSpan;
	}

	//尝试对后页进行合并
	while (1)
	{
		PAGE_ID nextId = span->_pageId + span->_n;
		auto ret = _idSpanMap.find(nextId);
		if (ret == _idSpanMap.end())
		{
			break;
		}

		Span* nextSpan = ret->second;
		if (nextSpan->_isUse)
		{
			break;
		}
		//合并超出128页的span 没有办法管理
		if (nextSpan->_n + span->_n > NPAGES - 1)
		{
			break;
		}
		span->_n += nextSpan->_n;

		_spanList[nextSpan->_n].Rease(nextSpan);
		delete nextSpan;
	}
	_spanList[span->_n].PushFront(span);
	span->_isUse = true;
	//_idSpanMap[span->_pageId] = span;
	//_idSpanMap[span->_pageId + span->_n - 1] = span;
	for (PAGE_ID i = 0; i < span->_n - 1; ++i)
	{
		_idSpanMap[span->_pageId + i] = span;
	}
}