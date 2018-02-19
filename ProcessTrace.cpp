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
ProcessTrace::ProcessTrace(string executionFile) {
    // Open execution trace file
    inFile.open(executionFile);
    if (inFile.fail()) { // Exit program if it can't open file
        cerr << "ERROR: Cannot open text file: " << executionFile << endl;
        exit(2);
    }
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
    getline(inFile, tempLine);
    std::istringstream iss(tempLine);
    
    iss >> tempWord; // First line of text file is always alloc, so do before while-loop
    iss >> tempDouble;
    numPages = tempDouble/1000;
    numPages = ceil(numPages); // Rounds to the nearest 0x1000 and then is divided by 0x100 to get number of pages
    mem::MMU mem(numPages); // Uses MMU.h file to create MMU object initialized with number of pages, which is the the space allocated divided by 0x1000
    cout << tempWord << " " << tempDouble << endl;
    
    while (getline(inFile, tempLine)) { // Go through line by line
        cout << tempLine << endl;
        std::istringstream iss(tempLine);
        while (iss >> tempWord) { // Look at the first word of the line
            if (tempWord == "alloc") {
                cout << "This shouldn't happen!" << endl;
            }
            else if (tempWord == "compare") {
                unsigned int addr, val;
                iss >> std::hex >> addr;
                while (iss >> val) { // Goes through each expected value in the line
                    uint8_t data[1]; // Create a temp array of size 1 and store byte in it, and then is compared one by one
                    mem.get_byte(data, addr);
                    if (data[0] != val) {
                       cout << "compare error at address " << std::hex << addr << ", expected " << std::hex << val << ", actual " << std::hex << unsigned(data[0]) << endl; 
                    }
                    addr++;
                }
            }
            else if (tempWord == "put") {
                unsigned int addr, val;
                uint8_t data[1];
                iss >> std::hex >> addr;
                while (iss >> val) {
                    data[0] = val; // Puts value into an array of size 1 and then that array is put into the specified address
                    mem.put_byte(addr, data); // Uses MMU.h file to store bytes
                    addr++;
                }
            }
            else if (tempWord == "fill") {
                unsigned int addr, count, val;
                iss >> std::hex >> addr; // First three values are the address, the amount of numbers to put, and the value to be put in
                iss >> std::hex >> count;
                iss >> std::hex >> val;
                uint8_t data[1];
                for (int i = 0; i < count; i++) {
                    data[0] = val; // Does the same as 'put', except loops for however many times is specified
                    mem.put_byte(addr, data); // Uses MMU.h file to store bytes
                    addr++;
                }
            }
            else if (tempWord == "copy") {
                unsigned int dest_addr, src_addr, count;
                iss >> std::hex >> dest_addr; // First three values are the destination address, the source address, and the amount of numbers to copy
                iss >> std::hex >> src_addr;
                iss >> std::hex >> count;
                uint8_t data[1];
                for (int i = 0; i < count; i++) {
                    mem.get_byte(data, src_addr); // Uses MMU.h file to get bytes...
                    mem.put_byte(dest_addr, data); // ...which is then stored again using the MMU.h file
                    src_addr++;
                    dest_addr++;
                }
            }
            else if (tempWord == "dump") {
                unsigned int addr, count;
                iss >> std::hex >> addr; // First two values are the address and the amount to dump
                iss >> std::hex >> count;
                cout << addr << endl;
                int i = 0;
                uint8_t data[1];
                for (; i < count; i++) { // For loop to go through each value starting from address
                    mem.get_byte(data, addr);
                    cout << " " << std::setw(2) << std::setfill('0') << std::hex << unsigned(data[0]);
                    if (i % 16 == 15) // New line every 16 values
                        cout << endl;
                    addr++;
                }
                if (i % 16 != 0) // Only prints another line after the dump if it doesn't end at 16
                    cout << endl;
            }
            else {
                cerr << "Invalid file type - cannot execute trace file" << endl;
                exit(2);
            }
        }
    }
}

