#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <map>

#include "core_functions.h"
#include "sparse_array_3.h"

using namespace std;

// extern map<uint64_t, uint8_t> memory;
extern A3 *memory;
// extern map<uint64_t, uint8_t>::iterator it;
// extern uint8_t memory[4000000000];

void mem_write8(uint64_t addr, uint8_t data);

uint64_t sign_extend_64(uint64_t in, uint64_t most);

uint32_t sign_extend_32(uint32_t in, uint32_t most);

uint64_t zero_extend_x(string unextended_bitstr, uint64_t x);

// uint64_t logical_shift_left(uint64_t orig, unsigned int scale);
//
// uint64_t logical_shift_right(uint64_t orig, unsigned int scale);
//
// uint32_t logical_shift_left32(uint32_t orig, unsigned int scale);
//
// uint32_t logical_shift_right32(uint32_t orig, unsigned int scale);

inline uint64_t logical_shift_left(uint64_t orig, unsigned int scale) {
    return orig << scale;
}

inline uint64_t logical_shift_right(uint64_t orig, unsigned int scale) {
    return orig >> scale;
}

inline uint32_t logical_shift_left32(uint32_t orig, unsigned int scale) {
    return orig << scale;
}

inline uint32_t logical_shift_right32(uint32_t orig, unsigned int scale) {
    return orig >> scale;
}

uint64_t arithmetic_shift_right(uint64_t orig, unsigned int scale);

uint64_t arithmetic_shift_right32(uint32_t orig, unsigned int scale);

uint64_t rotate_right(uint64_t orig, unsigned int scale);

uint32_t rotate_right32(uint32_t orig, unsigned int scale);

bool termin(uint64_t address, unsigned long instruction, uint64_t* reg);

int finalTermin();

// void memory_set(uint64_t address, uint8_t data);

inline void memory_set(uint64_t address, uint8_t data) {
    // if ( memory.find(address) != memory.end() ) {
        // check if it exists
        // memory[address] = data;
    // } else
    if ( address != 0xFFFFFFFFFFFFFFFFULL ) {
        // memory[address] = data % 4000000000;
        memory->set(address, data);
    } else {
        // if magic address, print the data
        printf("%c", data);
    }
    // printf("%x\n", data);
}

// void memory_set_32(uint64_t address, uint32_t data);

// void memory_set_64(uint64_t address, uint64_t data);

inline void memory_set_32(uint64_t address, uint32_t data) {
    // *((uint32_t*) (memory + address % 4000000000)) = data;
    memory->set_32(address, data);
}

inline void memory_set_64(uint64_t address, uint64_t data) {
    // *((uint64_t*) (memory + address % 4000000000)) = data;
    memory->set_64(address, data);
}

// uint8_t memory_get(uint64_t address);

inline uint8_t memory_get(uint64_t address) {
    // if ( memory.find(address) == memory.end() ) {
        // cout << "Error! Memory address ";
        // printf("%lx", address);
        // cout << " not found" << endl;
    // }
    // return memory[address % 4000000000];
    return memory->get(address);
}

// uint32_t memory_get_32(uint64_t address);

// uint64_t memory_get_64(uint64_t address);

inline uint32_t memory_get_32(uint64_t address) {
    // return memory->getBlock(address, 8);
    // uint64_t mem0 = memory[address], mem1 = memory[address + 1], mem2 = memory[address + 2], mem3 = memory[address + 3], mem4 = memory[address + 4], mem5 = memory[address + 5], mem6 = memory[address + 6], mem7 = memory[address + 7];
    // uint64_t mem0 = memory->get(address), mem1 = memory->get(address + 1), mem2 = memory->get(address + 2), mem3 = memory->get(address + 3), mem4 = memory->get(address + 4), mem5 = memory->get(address + 5), mem6 = memory->get(address + 6), mem7 = memory->get(address + 7);
    // return ((mem7 << 56) | (mem6 << 48) | (mem5 << 40) | (mem4 << 32) | (mem3 << 24) | (mem2 << 16) | (mem1 << 8) | mem0);
    // return *((uint32_t*) (memory + address % 4000000000));
    return memory->get_32(address);
}

inline uint64_t memory_get_64(uint64_t address) {
    // return memory->getBlock(address, 8);
    // uint64_t mem0 = memory[address], mem1 = memory[address + 1], mem2 = memory[address + 2], mem3 = memory[address + 3], mem4 = memory[address + 4], mem5 = memory[address + 5], mem6 = memory[address + 6], mem7 = memory[address + 7];
    // uint64_t mem0 = memory->get(address), mem1 = memory->get(address + 1), mem2 = memory->get(address + 2), mem3 = memory->get(address + 3), mem4 = memory->get(address + 4), mem5 = memory->get(address + 5), mem6 = memory->get(address + 6), mem7 = memory->get(address + 7);
    // return ((mem7 << 56) | (mem6 << 48) | (mem5 << 40) | (mem4 << 32) | (mem3 << 24) | (mem2 << 16) | (mem1 << 8) | mem0);
    // return *((uint64_t*) (memory + address % 4000000000));
    return memory->get_64(address);
}

// uint64_t extract(uint64_t full, int left, int right);
//
// uint32_t extract32(uint32_t full, int left, int right);
//
// uint64_t extract_single(uint64_t full, int pos);
//
// uint32_t extract_single32(uint32_t full, int pos);

void add_with_carry64(uint64_t operand1, uint64_t operand2, uint8_t carry, uint64_t* result, uint8_t* nzcv);

void add_with_carry32(uint32_t operand1, uint32_t operand2, uint8_t carry, uint32_t* result, uint8_t* nzcv);

uint64_t shift_reg64(uint64_t regVal, uint64_t type, uint64_t amt);

uint32_t shift_reg32(uint32_t regVal, uint32_t type, uint32_t amt);

uint64_t highest_set_bit7(string bitstring);

uint64_t replicate(uint64_t bits, unsigned int times, unsigned int size);

void decode_bit_masks(uint64_t N, uint64_t imms, uint64_t immr, bool immediate, uint64_t* wmask, uint64_t* tmask);

// uint64_t set_reg_32(uint64_t origRegVal, uint32_t val);

inline uint64_t set_reg_32(uint64_t origRegVal, uint32_t val) {
    // uint64_t newRegVal = (origRegVal >> 32) << 32;
    // newRegVal |= val;
    return val;
}

// bool is_instruction(uint64_t actual, uint64_t desired);

inline bool is_instruction(uint64_t actual, uint64_t desired) {
    return ( (actual & desired) == desired ) && ( (actual | desired) == actual );
}

#endif // FUNCTIONS_H
