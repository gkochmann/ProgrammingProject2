/* 
 * File:   PageFrameAllocator.h
 * Authors: Grant Kochmann and Maxwell Evans
 *
 * Created on January 27, 2018, 2:15 PM
 */

#ifndef PAGEFRAMEALLOCATOR_H
#define PAGEFRAMEALLOCATOR_H
using namespace std;

#include <iostream>
#include <vector>
#include <cstring>
#include <fstream>
#include <iostream>
#include <MMU.h>

class PageFrameAllocator {
public:
    PageFrameAllocator(mem::MMU mem);
    ~PageFrameAllocator() {
        // implement this bitch
    };
    
    PageFrameAllocator(const PageFrameAllocator& orig) = delete; // Class does not allow copy/move constructors/assignments
    PageFrameAllocator(PageFrameAllocator&& other) = delete;
    PageFrameAllocator& operator=(const PageFrameAllocator& orig) = delete;
    PageFrameAllocator& operator=(PageFrameAllocator&& orig) = delete;
    
    bool Allocate(uint32_t count, std::vector<uint32_t> &page_frames);
    bool Deallocate(uint32_t count, std::vector<uint32_t> &page_frames);
    void updateFreeListHead();
    
    //Getters
    uint32_t getPageFramesFree() const { return pageFramesFree; }
    uint32_t getPageFramesTotal() const { return pageFramesTotal; }
    
    //Setters
    void setPageFramesFree(uint32_t newFrames) {pageFramesFree = newFrames;}
    
private:
    std::vector<uint8_t> memory; //Byte array that contains page frames to be managed
    uint32_t pageFramesTotal; //A count of the total number of page frames in memory (memory size divided by 0x1000)
    uint32_t pageFramesFree; //The current number of free page frames
    uint32_t freeListHead; //The page frame number of the first page frame in the free list (0xFFFFFFFF if list empty)
    mem::MMU memory(256);
};

#endif /* PAGEFRAMEALLOCATOR_H */

