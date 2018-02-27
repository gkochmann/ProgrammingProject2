/* 
 * File:   ProcessTrace.cpp
 * Author: Grant Kochmann and Maxwell Evans
 * 
 * Created on January 20, 2018, 4:12 PM
 * Edited on February 5th, 2018.
 */

#include "ProcessTrace.h"

  /**
   * ProcessTrace - Constructor for ProcessTrace class
   * 
   * @param executionFile     Name of file to be opened
   * @throws                  Throws exception on error opening file
   */
ProcessTrace::ProcessTrace(string executionFile, mem::MMU &mem, PageFrameAllocator &allocator)
: memory(mem) {
    // Open execution trace file
    inFile.open(executionFile);
    if (inFile.fail()) { // Exit program if it can't open file
        cerr << "ERROR: Cannot open text file: " << executionFile << endl;
        exit(2);
    }
    // Set up first level page table
    allocator.Allocate(256, memory); //????
    // Set up PMCB. Pointing to beginning of page table
    //mem.set_PMCB();
}

  /**
   * Execute - creates empty array of type uint8_t vector, reads the input trace file and acts on commands from the file
   * Commands include alloc, compare, put, fill, copy, and dump
   * As of February 5th, 2018, now uses MMU object (from MMU.h) instead of a vector
   * 
   * @throws Throws exception if the file type is invalid (as in it doesn't use one of the six commands on one of the lines)
   */
void ProcessTrace::Execute() {
    // execute the commands in the trace file, using the vector as the memory referenced by the commands
    string tempLine, tempWord;
    double tempDouble, numPages;
    std::istringstream iss(tempLine);
    int lineNum = 1;
    while (getline(inFile, tempLine)) { // Go through line by line
        cout << lineNum << ":" << tempLine << endl;
        if (tempLine.at(0) != '#') {
            std::istringstream iss(tempLine);
            while (iss >> tempWord) { // Look at the first word of the line
                uint32_t address;
                try { // try catch to find exceptions
                    if (tempWord == "alloc") {
                        uint32_t vaddr, size, paddr;
                        iss >> std::hex >> vaddr;
                        iss >> std::hex >> size;
                        
                        // allocate memory somehow
                        mem::MMU mem(size); // Create next-level page table
                        PageFrameAllocator newAllocation(mem);
                        newAllocation.Allocate(size, mem);
                        
                        mem::PMCB testPMCB;
                        memory.get_PMCB(testPMCB);
                        testPMCB.vm_enable = false;
                        memory.ToPhysical(vaddr, paddr, true); // pages marked writable
                        // Sometimes PhysicalMemoryBoundsException, sometimes Segmentation Fault
                        mem.put_bytes(vaddr, size, 0); // New allocated memory is initialized to 0
                    } else if (tempWord == "compare") {
                        uint32_t addr, expected_values;
                        addr = address;
                        while (iss >> expected_values) { // Goes through each expected value in the line
                            uint8_t data[1]; // Create a temp array of size 1 and store byte in it, and then is compared one by one
                            memory.get_byte(data, addr);
                            if (data[0] != expected_values) {
                                cout << "compare error at address " << std::hex << addr << ", expected " << std::hex << expected_values << ", actual " << std::hex << unsigned(data[0]) << endl;
                            }
                            addr++;
                        }
                    } else if (tempWord == "put") {
                        uint32_t addr, val;
                        uint8_t data[1];
                        iss >> std::hex >> addr;
                        while (iss >> val) {
                            data[0] = val; // Puts value into an array of size 1 and then that array is put into the specified address
                            memory.put_byte(addr, data); // Uses MMU.h file to store bytes
                            addr++;
                        }
                    } else if (tempWord == "fill") {
                        uint32_t addr, count, val;
                        iss >> std::hex >> addr; // First three values are the address, the amount of numbers to put, and the value to be put in
                        iss >> std::hex >> count;
                        iss >> std::hex >> val;
                        uint8_t data[1];
                        for (int i = 0; i < count; i++) {
                            data[0] = val; // Does the same as 'put', except loops for however many times is specified
                            memory.put_byte(addr, data); // Uses MMU.h file to store bytes
                            addr++;
                        }
                    } else if (tempWord == "copy") {
                        uint32_t dest_addr, src_addr, count;
                        iss >> std::hex >> dest_addr; // First three values are the destination address, the source address, and the amount of numbers to copy
                        iss >> std::hex >> src_addr;
                        iss >> std::hex >> count;
                        uint8_t data[1];
                        for (int i = 0; i < count; i++) {
                            memory.get_byte(data, src_addr); // Uses MMU.h file to get bytes...
                            memory.put_byte(dest_addr, data); // ...which is then stored again using the MMU.h file
                            src_addr++;
                            dest_addr++;
                        }
                    } else if (tempWord == "dump") {
                        uint32_t addr, count;
                        iss >> std::hex >> addr; // First two values are the address and the amount to dump
                        iss >> std::hex >> count;
                        cout << addr << endl;
                        int i = 0;
                        uint8_t data[1];
                        for (; i < count; i++) { // For loop to go through each value starting from address
                            memory.get_byte(data, addr);
                            cout << " " << std::setw(2) << std::setfill('0') << std::hex << unsigned(data[0]);
                            if (i % 16 == 15) // New line every 16 values
                                cout << endl;
                            addr++;
                        }
                        if (i % 16 != 0) // Only prints another line after the dump if it doesn't end at 16
                            cout << endl;
                    } else if (tempWord == "writable") {
                        uint32_t vaddr, size, status;
                        iss >> std::hex >> vaddr;
                        iss >> std::hex >> size;
                        iss >> std::hex >> status;
                        for (int i = 0; i < size; i++) {
                            // Writable bit in the 2nd level page table should be cleared for all Present pages in the range
                            mem::MMU mem(size);
                            mem::Addr paddr;
                            if (status == 0)
                                mem.ToPhysical(vaddr, paddr, false);
                            else // status == 1
                                mem.ToPhysical(vaddr, paddr, true);
                        }
                    }
                } catch (mem::PageFaultException e1) {
                    mem::PMCB tempPMCB;
                    memory.get_PMCB(tempPMCB);
                    tempPMCB.operation_state = mem::PMCB::NONE;
                    cout << "Exception: PageFaultException at " << tempPMCB.next_vaddress << endl;
                    cout << "what() message: " << e1.what() << endl;
                    memory.set_PMCB(tempPMCB);
                    memory.FlushTLB();
                } catch (mem::WritePermissionFaultException e2) {
                    mem::PMCB tempPMCB;
                    memory.get_PMCB(tempPMCB);
                    cout << "Exception: WritePermissionFaultException at " << tempPMCB.next_vaddress << endl;
                    cout << "what() message: " << e2.what() << endl;
                    tempPMCB.operation_state = mem::PMCB::NONE;
                    memory.set_PMCB(tempPMCB);
                    memory.FlushTLB();
                }
            }
        }
        lineNum++;
    }
}

