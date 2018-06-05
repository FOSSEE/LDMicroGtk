#include "linuxUI.h"

std::vector<HEAPRECORD> HeapRecords;

HANDLE HeapCreate(DWORD  flOptions, SIZE_T dwInitialSize, SIZE_T dwMaximumSize)
{
    HANDLE hHeap = NULL;
    HEAPRECORD hHeapRecord;
    hHeapRecord.dwMaximumSize = dwMaximumSize;
    hHeap = malloc(dwInitialSize);

    if (hHeap == NULL)
        return NULL;
    
    hHeapRecord.dwSize = dwInitialSize;
    hHeapRecord.hHeap = hHeap;
    hHeapRecord.dwAllocatedSizeOffset = 0;
    hHeapRecord.HeapID = hHeapRecords.size()+1;
    HeapRecords.push_back(hHeapRecord);

    return hHeap;
}

LPVOID HeapAlloc(HANDLE hHeap, DWORD  dwFlags, SIZE_T dwBytes)
{
    auto it = std::find_if(HeapRecords.begin(), HeapRecords.end(),  [&hHeap](HEAPRECORD &Record) { return Record.hHeap == hHeap; });
    
    if (it == HeapRecords.end())
        return NULL;

    if ((*it).dwAllocatedSizeOffset + dwBytes > (*it).dwSize)
    {
        if ((*it).dwMaximumSize != 0)
            if((*it).dwAllocatedSizeOffset + dwBytes > (*it).dwMaximumSize)
                return NULL;
        
        (*it).hHeap = realloc((*it).hHeap, (*it).dwAllocatedSizeOffset + dwBytes);
        hHeap = (*it).hHeap;
    }

    /// HEAP_ZERO_MEMORY is set by default
    DWORD flags = MAP_ANONYMOUS;
    if ( (dwFlags & HEAP_ZERO_MEMORY) == HEAP_ZERO_MEMORY)
        flags = MAP_ANONYMOUS | MAP_UNINITIALIZED;
    //void * memset ( void * ptr, int value, size_t num );
    LPVOID p = mmap(hHeap + (*it).dwAllocatedSizeOffset, dwBytes, PROT_EXEC, flags, -1, 0);

    if (p == NULL)
        return NULL;
    
    (*it).dwAllocatedSizeOffset += dwBytes;
    HEAPCHUNCK chunck;
    chunck.Chunck = p;
    chunck.dwSize = dwBytes;
    (*it).Element.push_back(chunck);

    return p;
}

BOOL HeapFree(HANDLE hHeap, DWORD  dwFlags, LPVOID lpMem)
{
    auto heap_it = std::find_if(HeapRecords.begin(), HeapRecords.end(),  [&hHeap](HEAPRECORD &Record) { return Record.hHeap == hHeap; });
    
    if (heap_it == HeapRecords.end())
        return NULL;
    
    auto chunck_it = std::find_if((*heap_it).Element.begin(), (*heap_it).Element.end(),  [&lpMem](HEAPCHUNCK &Chunck) { return Chunck.Chunck == lpMem; });
    
    if (chunck_it == (*heap_it).Element.end())
        return NULL;
    
    int result = munmap((*chunck_it).Chunck, (*chunck_it).dwSize);

    if (result == 0)
    {
        (*heap_it).Element.erase(chunck_it);
        return TRUE;
    }
    else 
        return FALSE;

}

