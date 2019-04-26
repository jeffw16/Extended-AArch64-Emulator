#include "functions.h"

void mem_write8(uint64_t addr, uint8_t data) {
    // memory.insert(it, pair<uint64_t, uint8_t>(addr, data));
    // memory[addr] = data;
    // memory->set(addr, data);
    memory_set(addr, data);
}

uint64_t sign_extend_64(uint64_t in, uint64_t most) {
    uint64_t signext = in >> (most - 1);
    if ( signext == 1 ) {
        uint64_t bitmask = 0xFFFFFFFFFFFFFFFF;
        bitmask = (bitmask >> most) << most;
        uint64_t out = in | bitmask;
        return out;
    } else {
        return in;
    }
}

uint32_t sign_extend_32(uint32_t in, uint32_t most) {
    uint32_t signext = in >> (most - 1);
    if ( signext == 1 ) {
        uint32_t bitmask = 0xFFFFFFFF;
        bitmask = (bitmask >> most) << most;
        uint32_t out = in | bitmask;
        return out;
    } else {
        return in;
    }
}

// uint64_t zero_extend_x(string unextended_bitstr, uint64_t x) {
//     int len = unextended_bitstr.length();
//     string prepend_content = "";
//     for ( uint64_t i = 0; i < x - len; i++ ) {
//         prepend_content += "0";
//     }
//     string result = prepend_content + unextended_bitstr;
//     bitset<64> result_bs(result);
//     uint64_t result_int = result_bs.to_ulong();
//     return result_int;
// }

// uint64_t logical_shift_left(uint64_t orig, unsigned int scale) {
//     return orig << scale;
// }
//
// uint64_t logical_shift_right(uint64_t orig, unsigned int scale) {
//     return orig >> scale;
// }
//
// uint32_t logical_shift_left32(uint32_t orig, unsigned int scale) {
//     return orig << scale;
// }
//
// uint32_t logical_shift_right32(uint32_t orig, unsigned int scale) {
//     return orig >> scale;
// }

uint64_t arithmetic_shift_right(uint64_t orig, unsigned int scale) {
    int64_t signedOrig = (int64_t) orig;
    signedOrig = signedOrig >> scale;
    return (uint64_t) signedOrig;
}

uint64_t arithmetic_shift_right32(uint32_t orig, unsigned int scale) {
    int32_t signedOrig = (int32_t) orig;
    signedOrig = signedOrig >> scale;
    return signedOrig;
}

uint64_t rotate_right(uint64_t orig, unsigned int scale) {
    unsigned int N = 64;
    unsigned int m = scale % N;
    return logical_shift_right(orig, m) | logical_shift_left(orig, N - m);
}

uint32_t rotate_right32(uint32_t orig, unsigned int scale) {
    unsigned int N = 32;
    unsigned int m = scale % N;
    return logical_shift_right32(orig, m) | logical_shift_left32(orig, N - m);
}

bool termin(uint64_t address, unsigned long instruction, uint64_t* reg) {

    printf("unknown instruction %lx at %lx\n", instruction, address);
    for ( int i = 0; i < 32; i++ ) {
        unsigned long val = reg[i];
        if ( i == 31 ) {
            printf("XSP : %016lX\n", val);
        } else {
            printf("X%02d : %016lX\n", i, val);
        }
    }
    
    exit(0);
    return true;
}

int finalTermin() {
    exit(0);
}

// void memory_set(uint64_t address, uint8_t data) {
//     // if ( memory.find(address) != memory.end() ) {
//         // check if it exists
//         // memory[address] = data;
//     // } else
//     if ( address != 0xFFFFFFFFFFFFFFFFULL ) {
//         memory[address] = data;
//         // memory->set(address, data);
//     } else {
//         // if magic address, print the data
//         printf("%c", data);
//     }
//     // printf("%x\n", data);
// }

// void memory_set_64(uint64_t address, uint64_t data) {
//     memory[address] = (uint8_t) extract(data, 7, 0);
//     memory[address + 1] = (uint8_t) extract(data, 15, 8);
//     memory[address + 2] = (uint8_t) extract(data, 23, 16);
//     memory[address + 3] = (uint8_t) extract(data, 31, 24);
//     memory[address + 4] = (uint8_t) extract(data, 39, 32);
//     memory[address + 5] = (uint8_t) extract(data, 47, 40);
//     memory[address + 6] = (uint8_t) extract(data, 55, 48);
//     memory[address + 7] = (uint8_t) extract(data, 63, 56);
//     // memory->set(address, (uint8_t) extract(data, 7, 0));
//     // memory->set(address + 1, (uint8_t) extract(data, 15, 8));
//     // memory->set(address + 2, (uint8_t) extract(data, 23, 16));
//     // memory->set(address + 3, (uint8_t) extract(data, 31, 24));
//     // memory->set(address + 4, (uint8_t) extract(data, 39, 32));
//     // memory->set(address + 5, (uint8_t) extract(data, 47, 40));
//     // memory->set(address + 6, (uint8_t) extract(data, 55, 48));
//     // memory->set(address + 7, (uint8_t) extract(data, 63, 56));
// }
//
// void memory_set_32(uint64_t address, uint32_t data) {
//     memory[address] = (uint8_t) extract(data, 7, 0);
//     memory[address + 1] = (uint8_t) extract(data, 15, 8);
//     memory[address + 2] = (uint8_t) extract(data, 23, 16);
//     memory[address + 3] = (uint8_t) extract(data, 31, 24);
//     // memory->set(address, (uint8_t) extract(data, 7, 0));
//     // memory->set(address + 1, (uint8_t) extract(data, 15, 8));
//     // memory->set(address + 2, (uint8_t) extract(data, 23, 16));
//     // memory->set(address + 3, (uint8_t) extract(data, 31, 24));
// }

// uint8_t memory_get(uint64_t address) {
//     // if ( memory.find(address) == memory.end() ) {
//         // cout << "Error! Memory address ";
//         // printf("%lx", address);
//         // cout << " not found" << endl;
//     // }
//     return memory[address];
//     // return memory->get(address);
// }

// inline uint64_t memory_get_64(uint64_t address) {
//     // return memory->getBlock(address, 8);
//     // uint64_t mem0 = memory[address], mem1 = memory[address + 1], mem2 = memory[address + 2], mem3 = memory[address + 3], mem4 = memory[address + 4], mem5 = memory[address + 5], mem6 = memory[address + 6], mem7 = memory[address + 7];
//     // uint64_t mem0 = memory->get(address), mem1 = memory->get(address + 1), mem2 = memory->get(address + 2), mem3 = memory->get(address + 3), mem4 = memory->get(address + 4), mem5 = memory->get(address + 5), mem6 = memory->get(address + 6), mem7 = memory->get(address + 7);
//     // return ((mem7 << 56) | (mem6 << 48) | (mem5 << 40) | (mem4 << 32) | (mem3 << 24) | (mem2 << 16) | (mem1 << 8) | mem0);
//     return *((uint64_t *) (memory + address));
// }

// uint32_t memory_get_32(uint64_t address) {
//     uint8_t mem0 = memory[address], mem1 = memory[address + 1], mem2 = memory[address + 2], mem3 = memory[address + 3];
//     // uint64_t mem0 = memory->get(address), mem1 = memory->get(address + 1), mem2 = memory->get(address + 2), mem3 = memory->get(address + 3);
//     return ((mem3 << 24) | (mem2 << 16) | (mem1 << 8) | mem0);
// }

// uint64_t extract(uint64_t full, int left, int right) {
//     return (full << (63 - left)) >> (63 - left + right);
// }
//
// uint32_t extract32(uint32_t full, int left, int right) {
//     return (full << (31 - left)) >> (31 - left + right);
// }
//
// uint64_t extract_single(uint64_t full, int pos) {
//     return extract(full, pos, pos);
// }
//
// uint32_t extract_single32(uint32_t full, int pos) {
//     return extract32(full, pos, pos);
// }

void add_with_carry64(uint64_t operand1, uint64_t operand2, uint8_t carry, uint64_t* result, uint8_t* nzcv) {
    unsigned long long int unsigned_sum = operand1 + operand2 + carry;
    *result = (uint64_t) unsigned_sum;
    uint8_t n = 0;
    if ( ((int64_t) *result) < 0 ) {
        n = 1;
    }
    uint8_t z = 0;
    if ( *result == 0 ) {
        z = 1;
    }
    uint8_t c = 0;
    if ( operand1 + operand2 + carry < operand1 || operand1 + operand2 + carry < operand2 ) {
        c = 1;
    }
    int64_t signed_operand1 = (int64_t) operand1;
    int64_t signed_operand2 = (int64_t) operand2;
    int64_t signed_carry = (int64_t) carry;
    uint8_t v = 0;
    if ( (signed_operand1 + signed_operand2 + signed_carry < 0 && signed_operand1 > 0 && signed_operand2 > 0) || (signed_operand1 + signed_operand2 + signed_carry > 0 && signed_operand1 < 0 && signed_operand2 < 0) ) {
        v = 1;
    }
    *nzcv = (n << 3) | (z << 2) | (c << 1) | v;
    return;
}

void add_with_carry32(uint32_t operand1, uint32_t operand2, uint8_t carry, uint32_t* result, uint8_t* nzcv) {
    uint64_t unsigned_sum = operand1 + operand2 + carry;
    *result = (uint32_t) unsigned_sum;
    uint8_t n = 0;
    if ( ((int32_t) *result) < 0 ) {
        n = 1;
    }
    uint8_t z = 0;
    if ( *result == 0 ) {
        z = 1;
    }
    uint8_t c = 0;
    // if ( ((uint64_t) *result) == unsigned_sum ) {
    if ( operand1 + operand2 + carry < operand1 || operand1 + operand2 + carry < operand2 ) {
        c = 1;
    }
    int32_t signed_operand1 = (int32_t) operand1;
    int32_t signed_operand2 = (int32_t) operand2;
    int32_t signed_carry = (int32_t) carry;
    uint8_t v = 0;
    if ( (signed_operand1 + signed_operand2 + signed_carry < 0 && signed_operand1 > 0 && signed_operand2 > 0) || (signed_operand1 + signed_operand2 + signed_carry > 0 && signed_operand1 < 0 && signed_operand2 < 0) ) {
        v = 1;
    }
    *nzcv = (n << 3) | (z << 2) | (c << 1) | v;
    return;
}

uint64_t shift_reg64(uint64_t regVal, uint64_t type, uint64_t amt) {
    uint64_t result = regVal;
    if ( type == 0 ) {
        result = logical_shift_left(result, amt);
    } else if ( type == 1 ) {
        result = logical_shift_right(result, amt);
    } else if ( type == 2 ) {
        result = arithmetic_shift_right(result, amt);
    } else if ( type == 3 ) {
        result = rotate_right(result, amt);
    } else {
        cout << "Error: Invalid shift type" << endl;
    }
    return result;
}

uint32_t shift_reg32(uint32_t regVal, uint32_t type, uint32_t amt) {
    uint32_t result = regVal;
    if ( type == 0 ) {
        // cout << "Shift type: LSL" << endl;
        result = logical_shift_left32(result, amt);
    } else if ( type == 1 ) {
        // cout << "Shift type: LSR" << endl;
        result = logical_shift_right32(result, amt);
    } else if ( type == 2 ) {
        // cout << "Shift type: ASR" << endl;
        result = arithmetic_shift_right32(result, amt);
    } else if ( type == 3 ) {
        // cout << "Shift type: ROR" << endl;
        result = rotate_right32(result, amt);
    } else {
        cout << "Error: Invalid shift type" << endl;
    }
    return result;
}

// uint64_t highest_set_bit7(string bitstring) {
//     bitset<7> bs(bitstring);
//     for ( int i = 6; i >= 0; i-- ) {
//         if ( bs[i] == 1 ) {
//             return i;
//         }
//     }
//     return 0;
// }

uint64_t highest_set_bit7(uint8_t bs) {
    // bitset<7> bs(bitstring);
    for ( int i = 6; i >= 0; i-- ) {
        if ( extract_single32(bs, i) == 1 ) {
            return i;
        }
    }
    return 0;
}

uint64_t replicate(uint64_t bits, unsigned int times, unsigned int size) {
    uint64_t result = 0;
    for ( unsigned int i = 0; i < times; i++ ) {
        result |= (bits << (i * size));
    }
    return result;
}

void decode_bit_masks(uint64_t N, uint64_t imms, uint64_t immr, bool immediate, uint64_t* wmask, uint64_t* tmask) {
    uint8_t levels = 0;
    uint64_t len = highest_set_bit7(N << 7 | ~imms);
    for ( uint8_t i = 0; i < len; i++ ) {
        levels |= 1 << i;
    }
    uint64_t S = imms & levels;
    uint64_t R = immr & levels;
    uint64_t diff = S - R;
    uint64_t esize = 1 << len;
    uint64_t d = extract(diff, len - 1, 0);
    uint64_t welem = 0, telem = 0;
    for ( uint8_t i = 0; i < S + 1; i++ ) {
        welem |= 1 << i;
    }
    for ( uint8_t i = 0; i < d + 1; i++ ) {
        telem |= 1 << i;
    }
    if ( N == 1 ) {
        *wmask = replicate(rotate_right(welem, R), 64 / esize, esize);
    } else {
        *wmask = replicate(rotate_right32(welem, R), 64 / esize, esize);
    }
    *tmask = replicate(telem, 64 / esize, esize);
}

// uint64_t set_reg_32(uint64_t origRegVal, uint32_t val) {
//     // uint64_t newRegVal = (origRegVal >> 32) << 32;
//     // newRegVal |= val;
//     uint64_t newRegVal = val;
//     return newRegVal;
// }

// bool is_instruction(uint64_t actual, uint64_t desired) {
//     return ( (actual & desired) == desired ) && ( (actual | desired) == actual );
// }
