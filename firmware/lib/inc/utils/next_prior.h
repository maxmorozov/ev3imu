#ifndef __UTILS_NEXT_PREV_H
#define __UTILS_NEXT_PREV_H

#include <iterator>

namespace utils {

    template<class ForwardIterator>
    inline ForwardIterator next(ForwardIterator first, typename std::iterator_traits<ForwardIterator>::difference_type offset = 1)	{
        std::advance(first, offset);
        return first;
    }

    template<class BidirectionalIterator> 
    inline BidirectionalIterator prev(BidirectionalIterator first, typename std::iterator_traits<BidirectionalIterator>::difference_type offset = 1) {
        // decrement iterator
        std::advance(first, -offset);
        return first;
    }

}


#endif //__UTILS_NEXT_PREV_H