/* 
 * File:   PageFrameAllocator.cpp
 * Authors: Grant Kochmann and Maxwell Evans
 * 
 * Created on January 27, 2018, 2:15 PM
 */

#include <vector>
#include "PageFrameAllocator.h"

PageFrameAllocator::PageFrameAllocator(mem::MMU &mem)
: memory(mem),
  pageFramesFree(memory.get_frame_count()),
  pageFramesTotal(memory.get_frame_count()),
  freeListHead(0) {
    for (uint32_t i = 0; i < memory.get_frame_count(); i++) {
        uint8_t v8 = i+1;
        if (i == memory.get_frame_count()-1)
            v8 = 0xFFFFFFFF;
        memory.put_byte(i*0x1000, &v8); // next is a uint8_t because the address of a uint32_t cannot be passed in for some reason
    }
}

bool PageFrameAllocator::Allocate(uint32_t count, mem::MMU &pageFrames) {
    if (getPageFramesFree() >= count) {
        for (int i = 0; i < count; i++) {
            pageFrames.put_byte(pageFrames.get_frame_count()+i, &freeListHead);
            freeListHead -= 0x1000;
            pageFramesFree--;
        }
        return true;
    }
    else {
        return false;
    }
}

bool PageFrameAllocator::Deallocate(uint32_t count, mem::MMU &pageFrames) {
    if (count <= pageFrames.get_frame_count()) { // If there is enough to deallocate...
        for (int i = 0; i < count; i++) {
            // Return next frame to head of free list
            uint8_t frame;
            pageFrames.get_byte(&frame, pageFrames.get_frame_count());
            freeListHead += 0x1000;
            freeListHead = frame;
            pageFramesFree++;
        }
        return true;
    } else {
        return false;
    }
    
}
