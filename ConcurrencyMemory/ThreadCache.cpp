#include "ThreadCache.h"
#include "CentrualCache.h"
void* ThreadCache::Allocate(size_t size)
{
	void* i = nullptr;
	assert(size <= MAX_BYTES);
	size_t alignSize = SizeClass::RoundUp(size);
	size_t index = SizeClass::Index(alignSize);
	assert(index < NFREELITS);
	
	if (!_freeLists[index].Empty())
	{
		return _freeLists[index].Pop();
	}
	else
	{
		return FetchFromCentralCache(index, alignSize);
	}

	return i;
}
void ThreadCache::Deallocate(void* ptr, size_t size)
{
	assert(size <= MAX_BYTES);
	assert(ptr);

	//找到映射的自由链表桶，对象插入进入
	size_t index = SizeClass::Index(size);
	_freeLists[index].Push(ptr);

	//释放对象的时候发现悬挂的列表太长了，要释放回中心缓存
	if (_freeLists[index].getSize() >= _freeLists[index].getMasSize())
	{
		ListToolLong(_freeLists[index],size);
	}

}
//释放对象的时候发现悬挂的列表太长了，要释放回中心缓存
void ThreadCache::ListToolLong(FreeList& list, size_t size)
{
	void* start = nullptr;
	void* end = nullptr;
	list.PopRange(start, end, list.getMasSize());

	CentrualCache::GetInstance()->ReleaseListToSpan(start,size);

}

//向中心缓存区 申请内存
void* ThreadCache::FetchFromCentralCache(size_t index, size_t size)
{
	//慢开始反馈调节算法 一次获取多少个
	//1.第一次扩容给少一点 因为要太多了可能还会用不完
	//2.并且NumMoveSize（）函数 保证了size越大，给的batchNum就越小  size越小，给的batchNum就越大
	size_t batchNum = min(_freeLists[index].getMasSize(), SizeClass::NumMoveSize(size));
	//3.但是如果一直有扩容的需求就会给上限慢慢增长
	//为以后的扩容  慢慢解除限制
	if (_freeLists[index].getMasSize() == batchNum)
	{
		_freeLists[index].getMasSize() += 1;
	}

	void* start = nullptr;
	void* end = nullptr;

	//实际拿取到的空间大小
	size_t actualNum = CentrualCache::GetInstance()->FetchRangeObj(start, end, batchNum, size);
	assert(actualNum > 0);

	if (actualNum == 1)
	{
		assert(start == end);
		return start;
	}
	else
	{
		_freeLists[index].PushRang(NextObj(start), end, actualNum - 1);
		return start;
	}


	return nullptr;
}