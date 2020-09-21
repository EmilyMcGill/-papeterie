// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#ifndef BITCOIN_ALLOCATORS_H
#define BITCOIN_ALLOCATORS_H

#include <string.h>
#include <string>
#include <boost/thread/mutex.hpp>
#include <map>

#ifdef WIN32
#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif
#define _WIN32_WINNT 0x0501
#define WIN32_LEAN_AND_MEAN 1
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
// This is used to attempt to keep keying material out of swap
// Note that VirtualLock does not provide this as a guarantee on Windows,
// but, in practice, memory that has been VirtualLock'd almost never gets written to
// the pagefile except in rare circumstances where memory is extremely low.
#else
#include <sys/mman.h>
#include <limits.h> // for PAGESIZE
#include <unistd.h> // for sysconf
#endif

/**
 * Thread-safe class to keep track of locked (ie, non-swappable) memory pages.
 *
 * Memory locks do not stack, that is, pages which have been locked several times by calls to mlock()
 * will be unlocked by a single call to munlock(). This can result in keying material ending up in swap when
 * those functions are used naively. This class simulates stacking memory locks by keeping a counter per page.
 *
 * @note By using a map from each page base address to lock count, this class is optimized for
 * small objects that span up to a few pages, mostly smaller than a page. To support large allocations,
 * something like an interval tree would be the preferred data structure.
 */
template <class Locker> class LockedPageManagerBase
{
public:
    LockedPageManagerBase(size_t page_size):
        page_size(page_size)
    {
        // Determine bitmask for extracting page from address
        assert(!(page_size & (page_size-1))); // size must be power of two
        page_mask = ~(page_size - 1);
    }

    // For all pages in affected range, increase lock count
    void LockRange(void *p, size_t size)
    {
        boost::mutex::scoped_lock lock(mutex);
        if(!size) return;
        const size_t base_addr = reinterpret_cast<size_t>(p);
        const size_t start_page = base_addr & page_mask;
        const size_t end_page = (base_addr + size - 1) & page_mask;
        for(size_t page = start_page; page <= end_page; page += page_size)
        {
            Histogram::iterator it = histogram.find(pa