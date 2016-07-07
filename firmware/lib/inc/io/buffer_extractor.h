#ifndef __IO_BUFFER_EXTRACTOR_H
#define __IO_BUFFER_EXTRACTOR_H

#include <io/buffer.h>
#include <iterator>

namespace io {

    template <typename T>
    class buffer_extractor {
    public:
        typedef uint8_t size_type;
    private:
        const T* buffer;
        size_type size;
        size_type offset;

    public:
        // construct with uninitialized wrapped pointer
        buffer_extractor()
        {
        }

        buffer_extractor(const io::const_buffer& buffer_)
            : buffer(io::buffer_cast<const T*>(buffer_)),
            size(io::buffer_size(buffer_)), offset(0)
        {
        }

        buffer_extractor(const buffer_extractor& other)
            : buffer(other.buffer)
        {
        }

        buffer_extractor& operator= (const buffer_extractor& other)
        {
            size = other.size;
            offset = other.offset;
            buffer = other.buffer;
            return *this;
        }

        buffer_extractor& operator= (const io::const_buffer& buffer_)
        {
            buffer = io::buffer_cast<const T*>(buffer_);
            size = io::buffer_size(buffer_);
            offset = 0;
            return *this;
        }

        void assign(const T* data, size_type size_) {
            buffer = data;
            size = size_;
            offset = 0;
        }

        bool pop(T& out) {
            if (offset < size) {
                out = buffer[offset];
                return true;
            }
            return false;
        }

        bool empty() const {
            return offset == size;
        }



    };

}

#endif //__IO_BUFFER_ACCESSOR_H