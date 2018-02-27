/* 
 * File:   PageFrameAllocator.cpp
 * Authors: Grant Kochmann and Maxwell Evans
 * 
 * Created on January 27, 2018, 2:15 PM
 */

#include <vector>
#include "PageFrameAllocator.h"
using mem::Addr;

  /**
   * PageFrameAllocator - Constructor for PageFrameAllocator class
   * 
   * @param &mem        Address of the memory from MMU, passed in from main
   */
PageFrameAllocator::PageFrameAllocator(mem::MMU &mem)
: memory(mem),
  pageFramesFree(mem.get_frame_count()),
  pageFramesTotal(mem.get_frame_count()),
  freeListHead(0) {
    //cout << unsigned(pageFramesFree) << endl;
    for (uint32_t i = 0; i < mem.get_frame_count()-1; i++) {
        uint8_t v8[] = {0, 0, 0, i+1}; // Since uint32_t is required, separate into 4 uint8_t to simulate uint32_t
        if (i+1 == mem.get_frame_count()-1) {
            v8[0] = 0xFF; // Last one has to be 0xFFFFFFFF, this is somewhat the same
            v8[1] = 0xFF;
            v8[2] = 0xFF;
            v8[3] = 0xFF;
        }
        memory.put_bytes(i*0x1000, sizeof(uint32_t), v8);
    }
}

  /**
   * Allocate - Allocates page frames for 1st/2nd level page tables
   * 
   * @param count        Amount of frames to be allocated, should always be a multiple of the total number of page frames
   * @param &pageFrames  Address of a vector that simulates all the pageFrames
   */
bool PageFrameAllocator::Allocate(uint32_t count, std::vector<uint32_t> &pageFrames) {
    if (getPageFramesFree() >= count) {
        for (int i = 0; i < count; i++) {
            pageFrames.push_back(freeListHead);
            uint8_t v8[4];
            memory.get_bytes(v8, freeListHead*0x1000, sizeof(uint32_t)); 
            pageFramesFree--; // Update pageFramesFree
            
            //freeListHead -= 0x1000;
            v8[0] = v8[0] << 24; // Update freeListHead
            v8[1] = v8[1] << 16;
            v8[2] = v8[2] << 8; //next[3] does not shift
            freeListHead = v8[0] | v8[1] | v8[2] | v8[3];
        }
        return true;
    }
    else {
        return false;
    }
}

  /**
   * Deallocate - Deallocates page frames for 1st/2nd level page tables
   * 
   * @param count        Amount of frames to be deallocated, should always be a multiple of the total number of page frames
   * @param &pageFrames  Address of a vector that simulates all the pageFrames
   */
bool PageFrameAllocator::Deallocate(uint32_t count, std::vector<uint32_t> &pageFrames) {
    if (count <= pageFrames.size()) { // If there is enough to deallocate...
        for (int i = 0; i < count; i++) {
            // Return next frame to head of free list
            uint8_t temp = pageFrames.back();
            pageFrames.pop_back();
            pageFramesFree++; // Update pageFramesFree
            
            uint8_t v8[] = {0, 0, 0, freeListHead};
            //freeListHead += 0x1000;
            freeListHead = temp; // Update freeListHead
            memory.put_bytes(temp*0x1000, sizeof(uint32_t), v8);
        }
        return true;
    } else {
        return false;
    }
    
}

