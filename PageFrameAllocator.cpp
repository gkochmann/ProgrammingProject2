/* 
 * File:   PageFrameAllocator.cpp
 * Authors: Grant Kochmann and Maxwell Evans
 * 
 * Created on January 27, 2018, 2:15 PM
 */

#include <vector>
#include "PageFrameAllocator.h"
using mem::Addr;

PageFrameAllocator::PageFrameAllocator(mem::MMU &mem)
: memory(mem) { 
    freeListHead = 0; // The page frame number of the first page frame in the free list (0xFFFFFFFF if list empty)

    int totalSize = memory.get_frame_count() * 0x1000;
    //memory.resize(totalSize, 0); //Resizes memory
    
    freeListHead = 0xFFFFFFFF; // The page frame number of the first page frame in the free list (0xFFFFFFFF if list empty)
    Addr index, v32;
    //memcpy(&memory[0], &freeListHead, sizeof(uint32_t)); // Default so that if list is empty, first 4 bytes have value of 0xFFFFFFFF
    for (int i = 0; i < memory.get_frame_count(); i++) { // Build free list
        freeListHead = 0;
        index = i*0x1000;
        v32 = i+1;
        if (i == memory.get_frame_count()-1) {
            v32 = 0xFFFFFFFF; // Last element of the list so 0xFFFFFFFF
        }
        uint8_t index8 = index;
        uint8_t next = v32;
        memory.put_bytes(index8, next, sizeof(uint8_t));
        //memcpy(&memory[index], &v32, sizeof(uint32_t));
    }
    
    pageFramesFree = memory.get_frame_count();  
    pageFramesTotal = memory.get_frame_count();
  
}

bool PageFrameAllocator::Allocate(Addr count, std::vector<Addr> &page_frames) {
        if (getPageFramesFree() >= count){ 
            for(int i = count; i > 0; i--){
                uint8_t a = i*0x1000;
                uint8_t b = page_frames[i];
                uint8_t c = count;
                memory.get_bytes(a, b, c);
                //page_frames.push_back(memory[i * 0x1000]); //Add page frame to the allocated vector
                //memory.pop_back();//[i * 0x1000]; //Erasing head
                freeListHead -= 0x1000; //Updating head
                setPageFramesFree(pageFramesFree - 1); //Removing 1 free page frame
            }
        } else {
            return false; //Not enough frames free so nothing allocated
        } 
        
    return true;
}

bool PageFrameAllocator::Deallocate(Addr count, std::vector<Addr> &page_frames) {
    if (count <= page_frames.size()) {
        for (int i = 0; i < count; i++) {
            uint8_t a = page_frames[i];
            uint8_t b = count;
            uint8_t c = page_frames[i+1];
            memory.put_bytes(a, b, c);
            //memory.push_back(page_frames[i]);          
            page_frames.pop_back();
            setPageFramesFree(pageFramesFree + 1); //Adding 1 free page frame
        }
        return true;
    } else {
        return false;
    }
    
}


//OLD CODE BELOW



/*bool PageFrameAllocator::Allocate(Addr count, std::vector<Addr> &page_frames) {
        if (getPageFramesFree() >= count){ 
            for(int i = count; i > 0; i--){
                
                page_frames.push_back(pageFrames[i * 0x1000]); //Add page frame to the allocated vector
                pageFrames.pop_back();//[i * 0x1000]; //Erasing head
                freeListHead -= 0x1000;
                memory.get_bytes((uint8_t)freeListHead, sizeof(Addr), count);
                memory.put_bytes((uint8_t)i*0x1000, &page_frames[i], 0);
                //updateFreeListHead(); //Updating head
                setPageFramesFree(pageFramesFree - 1); //Removing 1 free page frame
            }
        } else {
            return false; //Not enough frames free so nothing allocated
        } 
        
    return true;
}

bool PageFrameAllocator::Deallocate(Addr count, std::vector<Addr> &page_frames) {
    if (count <= page_frames.size()) {
        for (int i = 0; i < count; i++) {
            //memory.get_bytes()
            pageFrames.push_back(page_frames[i]); 
            memory.put_bytes(page_frames[i], sizeof(Addr), count);
            page_frames.pop_back();
            setPageFramesFree(pageFramesFree + 1); //Adding 1 free page frame
        }
        return true;
    } else {
        return false;
    }
    
}*/



//Old constructor
/*Addr index, v32; //v32 is address of frame
    
    //freeListHead = 0xFFFFFFFF; // The page frame number of the first page frame in the free list (0xFFFFFFFF if list empty)

    //Loops through all addresses
    //Has address object declared 
    //In for looop he's computing next address 
    //Then using memory.put_bytes to store it
    //After for loop accoutns for last page
    
    
    
    for (int i = 0; i < memory.get_frame_count(); i++) { 
        freeListHead = 0;
        Addr address = i*0x1000;
        index = i*0x1000;
        
        //Address - Total number of page frames - 1 x kPageSize
        
        v32 = i+1; //next frame
        if (i == memory.get_frame_count()-1) {
            v32 = 0xFFFFFFFF; // Last element of the list so 0xFFFFFFFF
        }
        
        memory.put_bytes((uint8_t)address, sizeof(uint8_t), &v32);
        //memcpy(&pageFrames[index], &v32, sizeof(uint32_t));
    }
    //Addr lastAddres
    //memory.put_bytes()
    pageFramesFree = memory.get_frame_count();  
    pageFramesTotal = memory.get_frame_count();*/