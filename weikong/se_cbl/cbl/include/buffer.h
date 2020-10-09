#ifndef BUFFER_2019_11_12
#define BUFFER_2019_11_12
#include <string>
using namespace std;
typedef void* (*allocation_function)(size_t size);
typedef void (*deallocation_function)(void* ptr);

struct memory_management_function_impl
{
	static void* default_allocate(size_t size)
	{
		return malloc(size);
	}

	static void default_deallocate(void* ptr)
	{
		free(ptr);
	}
};


template <typename T>
struct memory_management_function_storage
{
	static allocation_function allocate;
	static deallocation_function deallocate;
};

template <typename T> allocation_function memory_management_function_storage<T>::allocate = memory_management_function_impl::default_allocate;
template <typename T> deallocation_function memory_management_function_storage<T>::deallocate = memory_management_function_impl::default_deallocate;

typedef memory_management_function_storage<int> memory_function;

template<class T> struct auto_deleter
{
	typedef void (*D)(T*);
	T *data;
	D deleter;
	
	auto_deleter(T *data_, D deleter_): data(data_), deleter(deleter_) {

	}

	~auto_deleter() {
		if (data) deleter(data);
	}

	T* release() {
		T* result = data;
		data = 0;
		return result;
	}
};

int hexBuffer(const void *pBuffer,int nSize,bool bReverse,string& sHex);

#endif