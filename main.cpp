/* 
 * File:   main.cpp
 * Author: grant and max
 *
 * Created on February 19, 2018, 2:24 PM
 */

#include <cstdlib>
#include <iostream>
#include <MMU.h>
#include "PageFrameAllocator.h"
#include "ProcessTrace.h"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    if (argc != 2) {
        cerr << "File name not specified." << endl;
        exit(1);
    }
    
    // Create fstream to open file
    fstream inFile;
    string fileName = argv[1];
    inFile.open(fileName);
    if (inFile.fail()) { // Exit program if it can't open file
        cerr << "ERROR: Unable to open text file: " << fileName << endl;
        exit(1);
    }
    
    mem::MMU mem(256);
    PageFrameAllocator allocator(mem);
    ProcessTrace process(fileName, mem, allocator);
    process.Execute();
    
    //From lecture; how to organize program
    /*
     Main:
     mem:MMU memory(256)
     PageFrameAllocator allocator(memory) 
     ProcessTrace process(argv[1], memory, allocator)
     processes.execute();
     
     PageFrameAllocator:
        constructor:
           ref or pointer to MMU (memory) in main thats passed in
      
     ProcessTrace:
        Constructor:
            Set up first level page table.
            Set up PMCB. Pointing to begining of page table
      
      
      
     
     */
    return 0;
}

