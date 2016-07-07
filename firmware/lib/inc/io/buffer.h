//
// buffer.h
// ~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef __IO_BUFFER_H
#define __IO_BUFFER_H

#include <stdint.h>
#include <stddef.h>
#include <utils/copy.h>
#include <utils/inline.h>

namespace io {

    typedef uint8_t size_type;

    class mutable_buffer;
    class const_buffer;

    namespace detail {
        void* buffer_cast_helper(const mutable_buffer&);
        const void* buffer_cast_helper(const const_buffer&);

        size_type buffer_size_helper(const mutable_buffer&);
        size_type buffer_size_helper(const const_buffer&);
    } // namespace detail


    // Holds a buffer that can be modified.
    // The mutable_buffer class provides a safe representation of a buffer that can
    // be modified. It does not own the underlying data, and so is cheap to copy or
    // assign.
    //
    // The contents of a buffer may be accessed using the @ref buffer_size
    // and buffer_cast functions:
    //
    // io::mutable_buffer b1 = ...;
    // uint8_t s1 = io::buffer_size(b1);
    // unsigned char* p1 = io::buffer_cast<unsigned char*>(b1);
    //
    // The io::buffer_cast function permits violations of type safety, so
    // uses of it in application code should be carefully considered.
    class mutable_buffer
    {
    public:
        // Construct an empty buffer.
        mutable_buffer()
            : data_(0), size_(0)
        {
        }

        // Construct a buffer to represent a given memory range.
        mutable_buffer(void* data, uint8_t size)
            : data_(data), size_(size)
        {
        }

    private:
        friend void* io::detail::buffer_cast_helper(const mutable_buffer& b);
        friend size_type io::detail::buffer_size_helper(const mutable_buffer& b);

        void* data_;
        size_type size_;
    };

    namespace detail {

        inline void* buffer_cast_helper(const mutable_buffer& b)
        {
            return b.data_;
        }

        inline size_type buffer_size_helper(const mutable_buffer& b)
        {
            return b.size_;
        }

    } // namespace detail


    // Holds a buffer that cannot be modified.
    // The const_buffer class provides a safe representation of a buffer that cannot
    // be modified. It does not own the underlying data, and so is cheap to copy or
    // assign.
    //
    // The contents of a buffer may be accessed using the buffer_size
    // and buffer_cast functions:
    //
    // io::const_buffer b1 = ...;
    // uint8_t s1 = io::buffer_size(b1);
    // const unsigned char* p1 = io::buffer_cast<const unsigned char*>(b1);
    //
    // The io::buffer_cast function permits violations of type safety, so
    // uses of it in application code should be carefully considered.
    class const_buffer
    {
    public:
        // Construct an empty buffer.
        const_buffer()
            : data_(0), size_(0)
        {
        }

        // Construct a buffer to represent a given memory range.
        const_buffer(const void* data, size_type size)
            : data_(data), size_(size)
        {
        }

        // Construct a non-modifiable buffer from a modifiable one.
        const_buffer(const mutable_buffer& b)
            : data_(io::detail::buffer_cast_helper(b)), size_(io::detail::buffer_size_helper(b))
        {
        }

    private:
      friend const void* io::detail::buffer_cast_helper(const const_buffer& b);
      friend size_type io::detail::buffer_size_helper(
          const const_buffer& b);

      const void* data_;
      size_type size_;
    };

    namespace detail {

        inline const void* buffer_cast_helper(const const_buffer& b)
        {
            return b.data_;
        }

        inline size_type buffer_size_helper(const const_buffer& b)
        {
            return b.size_;
        }

    } // namespace detail

    //
    // The io::buffer_size function determines the total number of
    // bytes in a buffer or buffer sequence.

    /// Get the number of bytes in a modifiable buffer.
    inline size_type buffer_size(const mutable_buffer& b)
    {
      return detail::buffer_size_helper(b);
    }

    /// Get the number of bytes in a non-modifiable buffer.
    inline size_type buffer_size(const const_buffer& b)
    {
      return detail::buffer_size_helper(b);
    }


    // The io::buffer_cast function is used to obtain a pointer to
    // the underlying memory region associated with a buffer.
    //
    // Examples:
    //
    // To access the memory of a non-modifiable buffer, use:
    // io::const_buffer b1 = ...;
    // const unsigned char* p1 = io::buffer_cast<const unsigned char*>(b1);
    //
    // To access the memory of a modifiable buffer, use:
    // io::mutable_buffer b2 = ...;
    // unsigned char* p2 = io::buffer_cast<unsigned char*>(b2);
    //
    // The io::buffer_cast function permits violations of type safety, so
    // uses of it in application code should be carefully considered.

    // Cast a non-modifiable buffer to a specified pointer to POD type.
    template <typename PointerToPodType>
    inline PointerToPodType buffer_cast(const mutable_buffer& b)
    {
        return static_cast<PointerToPodType>(detail::buffer_cast_helper(b));
    }

    // Cast a non-modifiable buffer to a specified pointer to POD type.
    template <typename PointerToPodType>
    inline PointerToPodType buffer_cast(const const_buffer& b)
    {
        return static_cast<PointerToPodType>(detail::buffer_cast_helper(b));
    }

    // Create a new modifiable buffer that is offset from the start of another.
    inline mutable_buffer operator+(const mutable_buffer& b, size_type start)
    {
        if (start > buffer_size(b))
            return mutable_buffer();
        char* new_data = buffer_cast<char*>(b) + start;
        size_type new_size = buffer_size(b) - start;
        return mutable_buffer(new_data, new_size);
    }

    // Create a new modifiable buffer that is offset from the start of another.
    inline mutable_buffer operator+(size_type start, const mutable_buffer& b)
    {
        if (start > buffer_size(b))
            return mutable_buffer();
        char* new_data = buffer_cast<char*>(b) + start;
        size_type new_size = buffer_size(b) - start;
        return mutable_buffer(new_data, new_size);
    }

    // Create a new non-modifiable buffer that is offset from the start of another.
    inline const_buffer operator+(const const_buffer& b, size_type start)
    {
        if (start > buffer_size(b))
            return const_buffer();
        const char* new_data = buffer_cast<const char*>(b) + start;
        size_type new_size = buffer_size(b) - start;
        return const_buffer(new_data, new_size);
    }

    // Create a new non-modifiable buffer that is offset from the start of another.
    inline const_buffer operator+(size_type start, const const_buffer& b)
    {
        if (start > buffer_size(b))
            return const_buffer();
        const char* new_data = buffer_cast<const char*>(b) + start;
        size_type new_size = buffer_size(b) - start;
        return const_buffer(new_data, new_size);
    }

    // The io::buffer function is used to create a buffer object to
    // represent raw memory, an array of POD elements, a vector of POD elements,
    // or a std::string.
    //
    // A buffer object represents a contiguous region of memory as a 2-tuple
    // consisting of a pointer and size in bytes. A tuple of the form {void*,
    // size_type} specifies a mutable (modifiable) region of memory. Similarly, a
    // tuple of the form {const void*, size_type} specifies a const
    // (non-modifiable) region of memory. These two forms correspond to the classes
    // mutable_buffer and const_buffer, respectively. To mirror C++'s conversion
    // rules, a mutable_buffer is implicitly convertible to a const_buffer, and the
    // opposite conversion is not permitted.
    //
    // The simplest use case involves reading or writing a single buffer of a
    // specified size:
    //
    // sock.send(io::buffer(data, size));
    //
    // In the above example, the return value of io::buffer meets the
    // requirements of the ConstBufferSequence concept so that it may be directly
    // passed to the socket's write function. A buffer created for modifiable
    // memory also meets the requirements of the MutableBufferSequence concept.
    //
    // An individual buffer may be created from a builtin array.
    // This helps prevent buffer overruns by automatically determining the size
    // of the buffer:
    //
    // char d1[128];
    // size_type bytes_transferred = sock.receive(io::buffer(d1));
    //
    // In all three cases above, the buffers created are exactly 128 bytes long.
    // Note that a vector is never automatically resized when creating or using
    // a buffer. The buffer size is determined using the vector's size()
    // member function, and not its capacity.
    //
    // Accessing Buffer Contents
    //
    // The contents of a buffer may be accessed using the buffer_size and
    // buffer_cast functions:
    //
    // io::mutable_buffer b1 = ...;
    // size_type s1 = io::buffer_size(b1);
    // unsigned char* p1 = io::buffer_cast<unsigned char*>(b1);
    //
    // io::const_buffer b2 = ...;
    // size_type s2 = io::buffer_size(b2);
    // const void* p2 = io::buffer_cast<const void*>(b2);
    //
    // The io::buffer_cast function permits violations of type safety, so
    // uses of it in application code should be carefully considered.
    //
    // For convenience, the buffer_size function also works on buffer
    // sequences (that is, types meeting the ConstBufferSequence or
    // MutableBufferSequence type requirements). In this case, the function returns
    // the total size of all buffers in the sequence.
    //
    // Buffer Copying
    //
    // The buffer_copy function may be used to copy raw bytes between
    // individual buffers and buffer sequences.
    //
    // In particular, when used with the buffer_size, the buffer_copy
    // function can be used to linearise a sequence of buffers. For example:
    //
    // vector<const_buffer> buffers = ...;
    //
    // vector<unsigned char> data(io::buffer_size(buffers));
    // io::buffer_copy(io::buffer(data), buffers);
    //
    // Note that buffer_copy is implemented in terms of memcpy, and
    // consequently it cannot be used to copy between overlapping memory regions.
    //
    // Buffer Invalidation
    //
    // A buffer object does not have any ownership of the memory it refers to. It
    // is the responsibility of the application to ensure the memory region remains
    // valid until it is no longer required for an I/O operation. When the memory
    // is no longer available, the buffer is said to have been invalidated.
    //
    // Buffer Arithmetic
    //
    // Buffer objects may be manipulated using simple arithmetic in a safe way
    // which helps prevent buffer overruns. Consider an array initialised as
    // follows:
    //
    // boost::array<char, 6> a = { 'a', 'b', 'c', 'd', 'e' };
    //
    // A buffer object b1 created using:
    //
    // b1 = io::buffer(a);
    //
    // represents the entire array, { 'a', 'b', 'c', 'd', 'e' }. An
    // optional second argument to the io::buffer function may be used to
    // limit the size, in bytes, of the buffer:
    //
    // b2 = io::buffer(a, 3);
    //
    // such that b2 represents the data { 'a', 'b', 'c' }. Even if the
    // size argument exceeds the actual size of the array, the size of the buffer
    // object created will be limited to the array size.
    //
    // An offset may be applied to an existing buffer to create a new one:
    //
    // b3 = b1 + 2;
    //
    // where b3 will set to represent { 'c', 'd', 'e' }. If the offset
    // exceeds the size of the existing buffer, the newly created buffer will be
    // empty.
    //
    // Both an offset and size may be specified to create a buffer that corresponds
    // to a specific range of bytes within an existing buffer:
    //
    // b4 = io::buffer(b1 + 1, 3);
    //
    // so that b4 will refer to the bytes { 'b', 'c', 'd' }.

    // Create a new modifiable buffer that represents the given memory range.
    INLINE mutable_buffer buffer(void* data, size_type size_in_bytes)
    {
        return mutable_buffer(data, size_in_bytes);
    }

    // Create a new non-modifiable buffer that represents the given memory range.
    INLINE const_buffer buffer(const void* data, size_type size_in_bytes)
    {
        return const_buffer(data, size_in_bytes);
    }

    // Create a new modifiable buffer that represents the given POD array.
    #pragma inline=forced
    template <typename PodType, size_type N>
    inline mutable_buffer buffer(PodType (&data)[N])
    {
        return mutable_buffer(data, N * sizeof(PodType));
    }

    // Create a new modifiable buffer that represents the given POD array.
    #pragma inline=forced
    template <typename PodType, size_type N>
    inline mutable_buffer buffer(PodType (&data)[N], size_type max_size_in_bytes)
    {
        return mutable_buffer(data,
            N * sizeof(PodType) < max_size_in_bytes ? N * sizeof(PodType) : max_size_in_bytes);
    }

    // Create a new non-modifiable buffer that represents the given POD array.
    #pragma inline=forced
    template <typename PodType, size_type N>
    inline const_buffer buffer(const PodType (&data)[N])
    {
        return const_buffer(data, N * sizeof(PodType));
    }

    /// Create a new non-modifiable buffer that represents the given POD array.
    #pragma inline=forced
    template <typename PodType, size_type N>
    inline const_buffer buffer(const PodType (&data)[N], size_type max_size_in_bytes)
    {
        return const_buffer(data,
            N * sizeof(PodType) < max_size_in_bytes ? N * sizeof(PodType) : max_size_in_bytes);
    }

    // Create a new modifiable buffer from an existing buffer.
    INLINE mutable_buffer buffer(const mutable_buffer& b, std::size_t max_size_in_bytes)
    {
        return mutable_buffer(buffer_cast<void*>(b),
            buffer_size(b) < max_size_in_bytes ? buffer_size(b) : max_size_in_bytes);
    }

    // Create a new non-modifiable buffer from an existing buffer.
    INLINE const_buffer buffer(const const_buffer& b, std::size_t max_size_in_bytes)
    {
        return const_buffer(buffer_cast<const void*>(b),
            buffer_size(b) < max_size_in_bytes ? buffer_size(b) : max_size_in_bytes);
    }

    // The io::buffer_copy function is used to copy bytes from a
    // source buffer (or buffer sequence) to a target buffer (or buffer sequence).
    //
    // The buffer_copy function is available in two forms:
    //    A 2-argument form: buffer_copy(target, source)
    //    A 3-argument form: buffer_copy(target, source, max_bytes_to_copy)
    //
    // Both forms return the number of bytes actually copied. The number of bytes
    // copied is the lesser of:
    //    buffer_size(target)
    //    buffer_size(source)
    //    If specified, max_bytes_to_copy.
    //
    // This prevents buffer overflow, regardless of the buffer sizes used in the
    // copy operation.
    //
    // Note that buffer_copy is implemented in terms of memcpy, and
    // consequently it cannot be used to copy between overlapping memory regions.

    /// Copies bytes from a source buffer to a target buffer.
    // target - a modifiable buffer representing the memory region to which
    //    the bytes will be copied.
    // source - a non-modifiable buffer representing the memory region from
    //    which the bytes will be copied.
    //
    // The number of bytes copied is the lesser of:
    //    buffer_size(target)
    //    buffer_size(source)
    //
    // This function is implemented in terms of memcpy, and consequently it
    // cannot be used to copy between overlapping memory regions.
    inline size_type buffer_copy(const mutable_buffer& target, const const_buffer& source)
    {
        size_type target_size = buffer_size(target);
        size_type source_size = buffer_size(source);
        size_type n = target_size < source_size ? target_size : source_size;
        utils::copy(buffer_cast<uint8_t*>(target), buffer_cast<const uint8_t*>(source), n);
        return n;
    }

    inline size_type buffer_copy(const mutable_buffer& target, const mutable_buffer& source)
    {
        return buffer_copy(target, const_buffer(source));
    }

    inline size_type buffer_copy(const mutable_buffer& target,
        const const_buffer& source, size_type max_bytes_to_copy)
    {
        return buffer_copy(buffer(target, max_bytes_to_copy), source);
    }

    inline size_type buffer_copy(const mutable_buffer& target,
        const mutable_buffer& source, size_type max_bytes_to_copy)
    {
        return buffer_copy(buffer(target, max_bytes_to_copy), source);
    }

}

#endif //__IO_BUFFER_H