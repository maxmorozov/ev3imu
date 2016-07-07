#ifndef __UTILS_RING_BUFFER_H
#define __UTILS_RING_BUFFER_H

#include <algorithm>
#include <utils/inline.h>
#include <utils/select_size.h>
#include <utils/next_prior.h>

//#define POP_VIA_CONSUME

namespace utils {

	template <typename T, size_t max_size>
	class ring_buffer
	{
	public:
		typedef T value_type;
		typedef typename select_size<max_size>::type size_type;

	private:
        static_assert((max_size & (max_size -1)) == 0, "max_size must be a power of 2");

		T buffer_[max_size];

	    volatile size_type read_count_;
	    volatile size_type write_count_;
		static const size_type mask_ = max_size - 1;

		size_type read_index(size_type read_count) const
        {
            return read_count & mask_;
		}

   		size_type read_index() const
        {
            return read_index(read_count_);
		}

		size_type write_index(size_type write_count) const
        {
            return write_count & mask_;
		}

		size_type write_index() const
        {
            return write_index(write_count_);
		}

	protected:
		size_type max_number_of_elements() const
		{
			return max_size;
		}

        //Helper class to write the data into an output iterator
        template <typename OutputIterator>
        class consumer {
        private:
            OutputIterator it_;

        public:
            consumer(OutputIterator it) : it_(it) {}

            void operator()(T& item) {
                *it_++ = item;
            }
        };

	public:
        INLINE ring_buffer() : read_count_(0), write_count_(0) {
        }

        //Places the object to the tail of the queue
        //Returns true if the object has been added succeflully
		INLINE bool push(T value)
        {
			if (is_full())
				return false;

            size_type write_count = write_count_;
            buffer_[write_index(write_count)] = value;
            write_count_ = write_count + 1;

			return true;
		}

        //Extracts the object from the head of the queue
        //Returns true if the object has been extracted
        //#pragma inline=forced
		bool pop(T& output)
        {
#ifdef POP_VIA_CONSUME
            return consume_one(consumer<T*>(&output));
#else
			if (empty())
				return false;

            size_type read_count = read_count_;
			output = buffer_[read_index(read_count)];
            read_count_ = read_count + 1;

			return true;
#endif
		}


        //Extracts the object from the head of the queue and
        //#pragma inline=forced
		template <typename Functor>
		bool consume_one(Functor f)
        {
			if (empty())
				return false;

            size_type read_count = read_count_;
			f(buffer_[read_index(read_count)]);
            read_count_ = read_count + 1;

			return true;
		}

        //#pragma inline=forced
		template <typename Functor>
		size_type consume_all(Functor f)
		{
            const size_type avail = size();
            if (avail == 0)
                return false;

            size_type start_index = read_index();
            size_type new_read_count = read_count_ + avail;
            size_type end_index = read_index(new_read_count);

            if (end_index <= start_index) {
                std::for_each(buffer_ + start_index, buffer_ + max_size, f);
                std::for_each(buffer_, buffer_ + end_index, f);
            } else {
                std::for_each(buffer_ + start_index, buffer_ + end_index, f);
            }
            read_count_ = new_read_count;

            return avail;
		}

		size_type push(const T* data, size_type size)
		{
			return push(data, data + size) - data;
		}

		template <size_type size>
		size_type push(T const (&t)[size])
		{
			return push(t, size);
		}

		template <typename ConstIterator>
		ConstIterator push(ConstIterator begin, ConstIterator end)
		{
            size_type avail = write_available();
            if (avail == 0)
                return begin;

            size_t input_count = std::distance(begin, end);
	        input_count = std::min(input_count, (std::size_t)avail);

	        const ConstIterator last = utils::next(begin, input_count);

            size_type start_index = write_index();
            size_type new_write_count = write_count_ + input_count;
	        size_type end_index = write_index(new_write_count);

            if (end_index <= start_index) {
                size_type count1 = max_size - start_index;
                ConstIterator midpoint = utils::next(begin, count1);
                std::copy(begin, midpoint, buffer_ + start_index);
                std::copy(midpoint, last, buffer_);
            } else {
                std::copy(begin, last, buffer_ + start_index);
            }
            write_count_ = new_write_count;

			return last;
		}

		size_type pop(T * output_buffer, size_t output_count)
		{
            size_type avail = size();
            if (avail == 0)
                return 0;

	        output_count = std::min(output_count, avail);

            size_type start_index = read_index();
	        size_type new_read_count = read_count_ + output_count;
	        size_type end_index = read_index(new_read_count);

            if (end_index <= start_index) {
                size_type count1 = max_size - start_index;

                std::copy(buffer_ + start_index, buffer_ + max_size, output_buffer);
                std::copy(buffer_, buffer_ + end_index, output_buffer + count1);
            } else {
                std::copy(buffer_ + start_index, buffer_ + end_index, output_buffer);
            }
            read_count_ = new_read_count;

			return output_count;
		}

        //#pragma inline=forced
		template <typename OutputIterator>
		size_type pop_to_output_iterator(OutputIterator it)
		{
#ifdef POP_VIA_CONSUME
            return consume_all(consumer<OutputIterator>(it));
#else
            const size_type avail = size();
            if (avail == 0)
                return 0;

            size_type start_index = read_index();
            size_type new_read_count = read_count_ + avail;
            size_type end_index = read_index(new_read_count);

            if (end_index <= start_index) {
                std::copy(buffer_ + start_index, buffer_ + max_size, it);
                std::copy(buffer_, buffer_ + end_index, it);
            } else {
                std::copy(buffer_ + start_index, buffer_ + end_index, it);
            }
            read_count_ = new_read_count;

            return avail;
#endif
		}

        void flush() {
            write_count_ = read_count_;
        }

		const T& front(void) const
		{
			return buffer_[read_index()];
		}

		T& front(void)
		{
			return buffer_[read_index()];
		}

		bool empty() const
		{
            size_type write_count = write_count_;
			return write_count == read_count_;
		}

		bool is_full() const
		{
            size_type write_count = write_count_;
			return (size_type(write_count - read_count_) & size_type(~mask_)) != 0;
		}

		size_type size() const
		{
            size_type read_count = read_count_;
            return size_type(write_count_ - read_count);
		}

		size_type write_available() const
		{
			return max_size - size();
		}
	};

}


#endif//__UTILS_RING_BUFFER_H