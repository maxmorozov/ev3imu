#if !defined(__UTILS_UTILITIES_H__)
#define __UTILS_UTILITIES_H__

#include <stdlib.h>

namespace utils {

	struct destructible
	{
		virtual ~destructible()	{}
	};

	class noncopyable
	{
	protected:
		noncopyable()
		{	}

		//Don't allow to create copies
	private:
		noncopyable(const noncopyable &);
		const noncopyable &operator =(const noncopyable &);
	};


	template<typename T,size_t size>
	inline size_t count_of(T (&)[size]) { 
		return size; 
	}
}

#endif // !defined(__UTILS_UTILITIES_H__)
