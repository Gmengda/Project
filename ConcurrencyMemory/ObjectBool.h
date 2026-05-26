#pragma once
#include "CommonH.h"

//定长内存池
// template<size_t N>
//class ObjectPool
//{
//};
template<class T>
class ObjectPool
{
public:
	T* New()
	{
		T* obj = nullptr;
		//如果_freeList不为空的话，就要考虑被释放的空间能不能重复利用
		if (_freeList)
		{
			void* next = *((void**)_freeList);
			obj = (T*)_freeList;
			_freeList = next;
		}
		else
		{
			//如果剩余空间不够，重新申请内存，剩余空间标准必须大于sizeof(T)
			if (_remain < sizeof(T))
			{
				_remain = 128 * 1024;
				//_memory = (char*)malloc(_remain);
				_memory = (char*)SystemAlloc(_remain>>13);
				if (_memory == nullptr)
				{
					throw std::bad_alloc();
				}
			}
			obj = (T*)_memory;
			size_t objSize = sizeof(T) < sizeof(void*) ? sizeof(void*) : sizeof(T);
			_memory += objSize;
			_remain -= objSize;
		}
		new(obj)T ;
		return obj;
	}

	void Delete(T* deleteObj)
	{
		deleteObj->~T();
		//if (_freeList == nullptr)
		//{
		//	_freeList = deleteObj;
		//	//*((int*)deleteObj) = nullptr;
		//	//上面写的问题就是32位 和 64位程序下有所不同
		//	// 二级指针直接指向指向指针大小
		//	//void**这是一个指向 void* 的指针，即二级指针。它存储的是另一个指针的地址，而那个指针的类型是 void*
		//	*((void**)deleteObj) = nullptr;
		//}
		//else
		//{
		//头插
		//如果是头插的话 上面if部分就可有可无了
		*((void**)deleteObj) = _freeList;
		_freeList = deleteObj;
		
	}

private:
	//空间头指向
	char* _memory = nullptr;
	//空间尾指向
	size_t _remain = 0;
	void* _freeList = nullptr;
};
