#ifndef __IO_BUFFER_ITERATOR_H
#define __IO_BUFFER_ITERATOR_H

#include <io/buffer.h>
#include <iterator>

namespace io {

    template <typename T>
    class const_buffer_iterator : public std::iterator<
            random_access_iterator_tag,
            T,
            ptrdiff_t,
            const T*,
            const T&> {
    private:
        const T* current;

        typedef std::iterator<
            random_access_iterator_tag,
            T,
            ptrdiff_t,
            const T*,
            const T&> iterator;
    public:
        using iterator::iterator_category;
        using typename iterator::value_type;
        using typename iterator::difference_type;
        using typename iterator::distance_type;    // retained
        using typename iterator::pointer;
        using typename iterator::reference;

        typedef const_buffer_iterator<T> _Myt;
    public:
        // construct with uninitialized wrapped pointer
        const_buffer_iterator()
        {
        }

        // construct wrapped pointer from ptr
        const_buffer_iterator(const T* ptr)
            : current(ptr)
        {
        }

        const_buffer_iterator(const io::const_buffer& buffer_)
            : current(io::buffer_cast<const T*>(buffer_))
        {
        }

        const_buffer_iterator(const _Myt& other)
            : current(other.current)
        {
        }

        _Myt& operator= (const _Myt& other)
        {
            current = other.current;
            return *this;
        }


        const T& operator*() const { return *current; }

        const T* operator->() const { return current; }

        // preincrement
        _Myt& operator++()
        {
            ++current;
            return (*this);
        }

        // postincrement
        _Myt operator++(int)
        {
            _Myt _Tmp = *this;
            ++current;
            return (_Tmp);
        }

        // test if wrapped pointer == integer (null pointer constant)
        bool operator==(int right) const
        {
            return (current == (pointer)right);
        }

        // test for iterator equality
        bool operator==(const _Myt& right) const
        {
            return (current == right.current);
        }

        // test for iterator inequality
        bool operator!=(const _Myt& right) const
        {
            return !(*this == right);
        }

        // increment by integer
        _Myt& operator+=(difference_type off)
        {
            current += off;
            return *this;
        }

        // return this + integer
        _Myt operator+(difference_type off) const
        {
            return _Myt(current + off);
        }

        // decrement by integer
        _Myt& operator-=(difference_type off)
        {
            current -= off;
            return *this;
        }

        // return this - integer
        _Myt operator-(difference_type off) const
        {
            return _Myt(current - off);
        }

        // subscript
        reference operator[](difference_type off) const
        {
            return *(*this + off);
        }

        // test if this < right
        bool operator<(const _Myt& right) const
        {
            return current < right.current;
        }

        // test if this > right
        bool operator>(const _Myt& right) const
        {
            return right < *this;
        }

        // test if this <= right
        bool operator<=(const _Myt& right) const
        {
            return !(right < *this);
        }

        // test if this >= _Right
        bool operator>=(const _Myt& right) const
        {
            return !(*this < right);
        }

        // return difference of iterators
        difference_type operator-(const _Myt& right) const
        {
            return (current - right.current);
        }

    };

}

#endif //__IO_BUFFER_ITERATOR_H