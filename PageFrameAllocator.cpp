/* 
 * File:   PageFrameAllocator.cpp
 * Authors: Grant Kochmann and Maxwell Evans
 * 
 * Created on January 27, 2018, 2:15 PM
 */

#include <vector>
#include "PageFrameAllocator.h"

PageFrameAllocator::PageFrameAllocator(int numPageFrames) {
    int totalSize = numPageFrames * 0x1000;
    memory.resize(totalSize, 0); //Resizes memory
    
    freeListHead = 0xFFFFFFFF; // The page frame number of the first page frame in the free list (0xFFFFFFFF if list empty)
    uint32_t index, v32;
    memcpy(&memory[0], &freeListHead, sizeof(uint32_t)); // Default so that if list is empty, first 4 bytes have value of 0xFFFFFFFF
    for (int i = 0; i < numPageFrames; i++) { // Build free list
        freeListHead = 0;
        index = i*0x1000;
        v32 = i+1;
        if (i == numPageFrames-1) {
            v32 = 0xFFFFFFFF; // Last element of the list so 0xFFFFFFFF
        }
        memcpy(&memory[index], &v32, sizeof(uint32_t));
    }
    
    pageFramesFree = numPageFrames;  
    pageFramesTotal = numPageFrames;
}

bool PageFrameAllocator::Allocate(uint32_t count, std::vector<uint32_t> &page_frames) {
        if (getPageFramesFree() >= count){ 
            for(int i = count; i > 0; i--){
                page_frames.push_back(memory[i * 0x1000]); //Add page frame to the allocated vector
                memory.pop_back();//[i * 0x1000]; //Erasing head
                updateFreeListHead(); //Updating head
                setPageFramesFree(pageFramesFree - 1); //Removing 1 free page frame
            }
        } else {
            return false; //Not enough frames free so nothing allocated
        } 
        
    return true;
}

void PageFrameAllocator::updateFreeListHead() {
    freeListHead -= 0x1000;
}

bool PageFrameAllocator::Deallocate(uint32_t count, std::vector<uint32_t> &page_frames) {
    if (count <= page_frames.size()) {
        for (int i = 0; i < count; i++) {
            memory.push_back(page_frames[i]);          
            page_frames.pop_back();
            setPageFramesFree(pageFramesFree + 1); //Adding 1 free page frame
        }
        return true;
    } else {
        return false;
    }
    
}
