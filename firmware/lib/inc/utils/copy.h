#ifndef __UTILS_COPY_H
#define __UTILS_COPY_H

namespace utils {

    //std::copy uses memmove that actually slower than this code
	//copy [_First, _Last) to [_Dest, ...), arbitrary iterators
	//
	//This code is very fast if "begin" and "end" are constant addresses,
	//in the other cases IAR generates bad code
	template <typename InputIterator, typename OutputIterator>
	inline OutputIterator copy(InputIterator begin, InputIterator end, OutputIterator out) {
	    for (; begin != end; ++begin, ++out)
	        *out = *begin;
	    return out;
	}

	//This code works well if "src" and "target" are on stack.
	//It is slower that the implemenetation on iterators in case of constant addresses,
	//but much faster in other cases
	template <typename T>
	inline void copy(T* target, const T* src, uint8_t size) {
	    for (uint8_t i = 0; i < size; ++i)
	        target[i] = src[i];
	}

}


#endif //__UTILS_COPY_H