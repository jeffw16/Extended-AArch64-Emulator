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

uint32_t count_leading_zero_bits32(uint32_t num){

  int8_t end = 31;

  while(end >= 0){
    if (extract_single32(num, end--) == 1){
      break;
    }
    //--end;
  }
  return 32 - end - 1;
}

uint64_t count_leading_zero_bits64(uint64_t num){

  int8_t end = 63;

  while(end >= 0){
    if (extract_single(num, end--) == 1){
      break;
    }
    //--end;
  }
  return 64 - end - 1;
}


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

bool termin(uint64_t address, unsigned long instruction, uint64_t* reg, unsigned __int128* simd_reg, bool debug) {
    if ( debug ) {
        printf("unknown instruction %lx at %lx\n", instruction, address);
        for ( int i = 0; i < 32; i++ ) {
            uint64_t val = reg[i];
            if ( i == 31 ) {
                printf("XSP : %016lX\n", val);
            } else {
                printf("X%02d : %016lX\n", i, val);
            }
        }

        for ( int i = 0; i < 32; i++ ) {
            unsigned __int128 val = simd_reg[i];
            printf("Q%02d : %032llX\n", i, (unsigned long long) val);
        }
    }
    exit(0);
    return true;
}

int finalTermin() {
    exit(0);
}

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

uint64_t extend(uint64_t cutval, uint64_t n, bool unsign, uint64_t most) {
  if(unsign){
    return cutval;
  }
  else{
    return sign_extend_64(cutval, most);
  }
}
uint64_t extend_reg64(uint64_t m, uint64_t extend_type, uint64_t shift, uint64_t* reg) {
  //assert shift >= 0 && shift <= 4;
  uint64_t val = reg[m];
  bool unsign = false;
  uint64_t len = 0;
  unsign = (extend_type <= 4);
  len = 1 << (extract(extend_type, 1, 0) + 3);

  if (64 - shift < len){
    len = 64 - shift;
  }

  return extend(extract(val, len - 1, 0) << shift, 64, unsign, len - 1);
}

bool conditionHolds(uint8_t fullcond, uint8_t compareto) {
    bool condHolds = false;
    uint8_t cond = extract32(fullcond, 3, 1);
    uint8_t condLast = extract_single32(fullcond, 0);
    if ( cond == 0 ) {
        condHolds = (extract_single32(compareto, 2) == 1); // Z == 1 <-> EQ or NE
    } else if ( cond == 1 ) {
        condHolds = (extract_single32(compareto, 1) == 1); // C == 1 <-> CS or CC
    } else if ( cond == 2 ) {
        condHolds = (extract_single32(compareto, 3) == 1); // N == 1 <-> MI or PL
    } else if ( cond == 3 ) {
        condHolds = (extract_single32(compareto, 0) == 1); // V == 1 <-> VS or VC
    } else if ( cond == 4 ) {
        condHolds = (extract_single32(compareto, 1) == 1 && extract_single32(compareto, 2) == 0); // C == 1 && Z == 0 <-> HI or LS
    } else if ( cond == 5 ) {
        condHolds = (extract_single32(compareto, 3) == extract_single32(compareto, 0)); // N == V <-> GE or LT
    } else if ( cond == 6 ) {
        condHolds = (extract_single32(compareto, 3) == extract_single32(compareto, 0) && extract_single32(compareto, 2) == 0); // N == V && Z == 0 <-> GT or LE
    } else {
        condHolds = true; // AL
    }
    if ( extract_single32(condLast, 0) == 1 && cond != 7 ) {
        condHolds = !condHolds;
    }
    return condHolds;
}
