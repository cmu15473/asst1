#ifndef RASTER_RENDERER_PARALLEL_H
#define RASTER_RENDERER_PARALLEL_H

#ifndef _MSC_VER
#define USE_TBB
#endif
#ifdef USE_TBB
#include "../lib/tbb/parallel_for.h"
#include "../lib/tbb/scalable_allocator.h"
#else
#include <ppl.h>
#endif

class Parallel
{
public:
    template<typename Func>
    inline static void SerialFor(int first, int last, const Func &f)
    {
        for (int i = first; i<last; i++)
            f(i);
    }
#ifdef USE_TBB
    template<typename Func>
    inline static void For(int first, int last, const Func &f)
    {
        tbb::parallel_for(first, last, f);
    }
    template<typename Func>
    inline static void For(int first, int last, int chunkSize, const Func &f)
    {
        tbb::parallel_for(tbb::blocked_range<int>(first, last, chunkSize), [&](const tbb::blocked_range<int> & range)
        {
            for (int i = range.begin(); i<range.end(); i++)
                f(i);
        });
    }
#else
    template<typename Func>
    inline static void For(int first, int last, const Func &f)
    {
        concurrency::parallel_for(first, last, f);
    }
    template<typename Func>
    inline static void For(int first, int last, int chunkSize, const Func &f)
    {
        concurrency::parallel_for(first, last, f, concurrency::simple_partitioner(chunkSize));
    }
#endif
    inline static void * Alloc(size_t size)
    {
#ifdef USE_TBB
        return scalable_malloc(size);
#else
        return concurrency::Alloc(size);
#endif
    }
    inline static void Free(void * ptr)
    {
#ifdef USE_TBB
        return scalable_free(ptr);
#else
        return concurrency::Free(ptr);
#endif
    }
};
#endif
