#include "CentrualCache.h"
#include "PageCache.h"
CentrualCache CentrualCache::_sInst;

size_t CentrualCache::FetchRangeObj(void*& start, void*& end, size_t n, size_t size)
{
	size_t index = SizeClass::Index(size);
	//加锁，保证安全
	_spanLists[index]._mtx.lock();

	//获取一个非空的span 有内存的span 
	Span* span = GetOneSpan(_spanLists[index], size);
	assert(span);
	assert(span->_freeList);

	start = span->_freeList;
	end = start;
	
	size_t actualNum = 1;
	for (size_t i = 0; i < n - 1; i++)
	{
		//如果超出限制，后面就会崩掉
		if (NextObj(end) == nullptr)
		{
			break;
		}
		end = NextObj(end);
		actualNum++;
	}
	span->_freeList = NextObj(end);
	NextObj(end) = nullptr;

	span->_useCount += actualNum;

	//// 条件断点
	int j = 0;
	void* cur = start;
	while (cur)
	{
		cur = NextObj(cur);
		++j;
	}

	if (j != actualNum)
	{
		int x = 0;
	}

	//解锁
	_spanLists[index]._mtx.unlock();
	return actualNum;
}



//获取一个非空的 span 有内存的span
Span* CentrualCache::GetOneSpan(SpanList& list, size_t size)
{
	//1.查看当前span 列表中是否还有未分配的空间
	Span* it = list.Begin();
	while (it != list.end())
	{
		if (it->_freeList != nullptr)
		{
			return it;
		}
		else
		{
			it = it->_next;
		}
	}
	//先把central cache桶锁解开 这样其他线程释放对象也可以进来
	list._mtx.unlock();




	//2.没有新的空间，只能和pagechche要
	//既然要去访问PageCache 保证锁安全
	PageCache::GetPageCache()->getMutex().lock();
	Span* span = PageCache::GetPageCache()->NewSpan(SizeClass::NumMovePage(size));
	span->_isUse = true;
	PageCache::GetPageCache()->getMutex().unlock();
	
	//后续给span切分 再加锁？
	//list._mtx.lock();
	//不用再给centrualcache再加锁了，因为其他线程这会拿不到这个span
	

	//计算span 大块内存的起始地址
	char* start = (char*)(span->_pageId << PAGE_SHIFT);
	//计算 大块内存的字节数 大小
	size_t bytes = span->_n << PAGE_SHIFT;
	char* end = start + bytes;

	//3.把大块内存切成块挂起来 设置NextOBJ
	//尾插保证数据连续性
	span->_freeList = start;
	start += size;
	void* tail = span->_freeList;
	while (start < end)
	{
		NextObj(tail) = start;
		tail = NextObj(tail);
		start += size;
	}
	NextObj(tail) = nullptr;
	//4.把新获取的Span 插入到list里
	//切好span后需要把span挂到桶里面去的时候再加锁
	list._mtx.lock();
	list.PushFront(span);

	return span;
}

//将一定数量的对象挂到桶里
void CentrualCache::ReleaseListToSpan(void* start, size_t size)
{
	
	size_t index = SizeClass::Index(size);
	_spanLists[index]._mtx.lock();

	//start 自由链里面不一定都是同一span里面 的空间  
	// 需要找到对应空间的span
	//还给桶
	while (start)
	{
		void* next = NextObj(start);
		//获取id号对应的span
		Span* span = PageCache::GetPageCache()->MapObjectToSpan(start);
		//头插
		NextObj(start) = span->_freeList;
		span->_freeList = start;
		
		span->_useCount--;
		//说明切分出去的小块都回来了
		//这就可以再回收给pageCache  pageCache可以再去尝试合并前后页进行管理
		if (span->_useCount == 0)
		{
			_spanLists[index].Rease(span);
			span->_freeList = nullptr;
			span->_next = nullptr;
			span->_prev = nullptr;

			//释放span给pageCache
			_spanLists[index]._mtx.unlock();
			PageCache::GetPageCache()->getMutex().lock();
			PageCache::GetPageCache()->ReleasepanToPageCache(span);
			PageCache::GetPageCache()->getMutex().unlock();
			_spanLists[index]._mtx.lock();
		}
		start = next;
	}

	_spanLists[index]._mtx.unlock();
}