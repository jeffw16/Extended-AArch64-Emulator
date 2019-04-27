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

extern A3 *memory;

void mem_write8(uint64_t addr, uint8_t data);

uint64_t sign_extend_64(uint64_t in, uint64_t most);

uint32_t sign_extend_32(uint32_t in, uint32_t most);

uint64_t zero_extend_x(string unextended_bitstr, uint64_t x);

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

bool termin(uint64_t address, unsigned long instruction, uint64_t* reg, unsigned __int128* simd_reg, bool debug);

int finalTermin();

inline void memory_set(uint64_t address, uint8_t data) {
    if ( address != 0xFFFFFFFFFFFFFFFFULL ) {
        memory->set(address, data);
    } else {
        // if magic address, print the data
        printf("%c", data);
    }
}

inline void memory_set_32(uint64_t address, uint32_t data) {
    memory->set_32(address, data);
}

inline void memory_set_64(uint64_t address, uint64_t data) {
    memory->set_64(address, data);
}

inline void memory_set_128(uint64_t address, unsigned __int128 data) {
    memory->set_128(address, data);
}

inline uint8_t memory_get(uint64_t address) {
    return memory->get(address);
}

inline uint32_t memory_get_32(uint64_t address) {
    return memory->get_32(address);
}

inline uint64_t memory_get_64(uint64_t address) {
    return memory->get_64(address);
}

void add_with_carry64(uint64_t operand1, uint64_t operand2, uint8_t carry, uint64_t* result, uint8_t* nzcv);

void add_with_carry32(uint32_t operand1, uint32_t operand2, uint8_t carry, uint32_t* result, uint8_t* nzcv);

uint64_t shift_reg64(uint64_t regVal, uint64_t type, uint64_t amt);

uint32_t shift_reg32(uint32_t regVal, uint32_t type, uint32_t amt);

uint64_t highest_set_bit7(string bitstring);

uint64_t replicate(uint64_t bits, unsigned int times, unsigned int size);

void decode_bit_masks(uint64_t N, uint64_t imms, uint64_t immr, bool immediate, uint64_t* wmask, uint64_t* tmask);

inline uint64_t set_reg_32(uint64_t origRegVal, uint32_t val) {
    return val;
}

inline bool is_instruction(uint64_t actual, uint64_t desired) {
    return ( (actual & desired) == desired ) && ( (actual | desired) == actual );
}

#endif // FUNCTIONS_H
