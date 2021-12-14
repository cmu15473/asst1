#ifndef CORE_LIB_ARRAY_H
#define CORE_LIB_ARRAY_H

#include "Exception.h"

namespace CoreLib
{
	namespace Basic
	{
		template<typename T, int size>
		class Array
		{
		private:
			T _buffer[size];
			int _count;
		public:
			T* begin() const
			{
				return (T*)_buffer;
			}
			T* end() const
			{
				return (T*)_buffer+_count;
			}
		public:
			Array()
			{
				_count = 0;
			}
			inline int GetCapacity() const
			{
				return size;
			}
			inline int Count() const
			{
				return _count;
			}
			inline void SetSize(int newSize)
			{
#ifdef _DEBUG
				if (newSize > size)
					throw IndexOutofRangeException(L"size too large.");
#endif
				_count = newSize;
			}
			inline void Add(const T & item)
			{
#ifdef _DEBUG
				if (_count == size)
					throw IndexOutofRangeException(L"out of range access to static array.");
#endif
				_buffer[_count++] = item;
			}
			inline void Add(T && item)
			{
#ifdef _DEBUG
				if (_count == size)
					throw IndexOutofRangeException(L"out of range access to static array.");
#endif
				_buffer[_count++] = _Move(item);
			}

			inline T & operator [](int id)
			{
#if _DEBUG
				if(id >= _count || id < 0)
					throw IndexOutofRangeException(L"Operator[]: Index out of Range.");
#endif
				return _buffer[id];
			}

			inline T* Buffer() const
			{
				return (T*)_buffer;
			}

			inline void Clear()
			{
				_count = 0;
			}

			template<typename T2>
			int IndexOf(const T2 & val) const
			{
				for (int i = 0; i < _count; i++)
				{
					if (_buffer[i] == val)
						return i;
				}
				return -1;
			}

			template<typename T2>
			int LastIndexOf(const T2 & val) const
			{
				for (int i = _count - 1; i >= 0; i--)
				{
					if(_buffer[i] == val)
						return i;
				}
				return -1;
			}
		};
	}
}

#endif