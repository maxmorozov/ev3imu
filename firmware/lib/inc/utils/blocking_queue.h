#ifndef __UTILS_BLOCKING_QUEUE_H
#define __UTILS_BLOCKING_QUEUE_H

#include <OS_Services.h>
#include <utils/ring_buffer.h>

namespace utils {

    //--------------------------------------------------------------------------
    //
    //       NAME       :  blocking_queue
    //
    //       PURPOSE    :  Data channel for transferring data
    //                     objects of arbitrary type
    //
    //       NOTES      :  critical sections in methods of this class protect
    //                     buffer data and internal OS structures that are used
    //                     to reschedule processes.
    //                     I tried to remove critical sections because I used
    //                     wait-free single  producer/single consumer queue, but
    //                     I nedded to use critical sections to wrap system calls.
    //                     Therefore I remain usage of critical sections.
    //
    template<typename T, size_t Size, typename Container = utils::ring_buffer<T, Size> >
    class blocking_queue : protected OS::TService
    {
    public:
        typedef Container buffer_type;
        typedef typename buffer_type::size_type size_type;

    private:
        volatile OS::TProcessMap ProducersProcessMap;
        volatile OS::TProcessMap ConsumersProcessMap;
        buffer_type pool;

    public:
        INLINE blocking_queue()
            : ProducersProcessMap(0), ConsumersProcessMap(0)
        {
        }

        //----------------------------------------------------------------
        //
        //    Data transfer functions
        //
        //Blocking versions
        void write(const T* data, const size_type cont);
        bool read (T* data, const size_type count, timeout_t timeout = 0);

        void push(const T& item);
        bool pop(T& item, timeout_t timeout = 0);

        //Non-blocking versions
        size_type write_isr(const T* data, const size_type count) {
            TCritSect cs;

            size_type written_count = pool.push(data, count);
            resume_all(ConsumersProcessMap);

            return written_count;
        }
        size_type read_isr (T* const data, const size_type count) {
            TCritSect cs;

            size_type read_count = pool.pop(data, count);
            resume_all(ProducersProcessMap);

            return read_count;
        }

        bool push_isr(T item) {
            TCritSect cs;

            if (pool.push(item)) {
                resume_all_isr(ConsumersProcessMap);
                return true;
            }
            return false;
        }
        bool pop_isr(T& item) {
            TCritSect cs;

            if(pool.pop(item))
            {
                resume_all_isr(ProducersProcessMap);
                return true;
            }
            return false;
        }

        //----------------------------------------------------------------
        //
        //    Service functions
        //
        size_type size() const          { TCritSect cs; return pool.size();     }
        size_type get_free_size() const { TCritSect cs; return pool.write_available(); }
        bool empty() const              { TCritSect cs; return pool.empty(); }
        bool is_full() const            { TCritSect cs; return pool.is_full(); }
        void flush();
    };
    //--------------------------------------------------------------------------

    template<typename T, size_t Size, typename Container>
    void blocking_queue<T, Size, Container>::push(const T& item)
    {
        TCritSect cs;

        while(!pool.push(item))
        {
            // buffer is full, suspend current process until data removed
            suspend(ProducersProcessMap);
        }

        resume_all(ConsumersProcessMap);
    }

    template<typename T, size_t Size, typename Container>
    bool blocking_queue<T, Size, Container>::pop(T& item, timeout_t timeout)
    {
        TCritSect cs;

        if(pool.pop(item))
        {
            resume_all(ProducersProcessMap);
            return true;
        }
        else
        {
            cur_proc_timeout() = timeout;

            for(;;)
            {
                // channel is empty, suspend current process until data received or timeout
                suspend(ConsumersProcessMap);

                if(is_timeouted(ConsumersProcessMap))
                    return false;

                if(pool.pop(item))
                {
                    cur_proc_timeout() = 0;
                    resume_all(ProducersProcessMap);
                    return true;
                }
                // otherwise another process caught data
            }
        }
    }

    template<typename T, size_t Size, typename Container>
    void blocking_queue<T, Size, Container>::flush()
    {
        TCritSect cs;
        pool.flush();
        resume_all(ProducersProcessMap);
    }

    template<typename T, size_t Size, typename Container>
    void blocking_queue<T, Size, Container>::write(const T* data, const size_type count)
    {
        TCritSect cs;

        const T* end = data + count;
        while((data = pool.push(data, end)) != end)
        {
            // channel does not have enough space, suspend current process until data removed
            suspend(ProducersProcessMap);
        }

        resume_all(ConsumersProcessMap);
    }

    template<typename T, size_t Size, typename Container>
    bool blocking_queue<T, Size, Container>::read(T* data, size_type count, timeout_t timeout)
    {
        TCritSect cs;

        cur_proc_timeout() = timeout;

        while(pool.size() < count)
        {
            // channel doesn't contain enough data, suspend current process until data received or timeout
            suspend(ConsumersProcessMap);
            if(is_timeouted(ConsumersProcessMap))
                return false;
        }

        cur_proc_timeout() = 0;
        pool.pop(data, count);
        resume_all(ProducersProcessMap);

        return true;
    }

}

#endif //__UTILS_BLOCKING_QUEUE_H