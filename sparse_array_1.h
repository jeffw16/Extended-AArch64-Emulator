#ifndef SPARSE_ARRAY_1_H
#define SPARSE_ARRAY_1_H

#include <cstdlib>
#include <cstdint>
#include <iostream>

#include "core_functions.h"

using namespace std;

const uint64_t CHUNKSIZE = 4194304;

class A1 {
    uint8_t *arr;
public:
    A1() {
        arr = new uint8_t[CHUNKSIZE];
    }

    ~A1() {
        delete[] arr;
    }

    inline uint8_t get(uint64_t address) {
        // return arr[address % CHUNKSIZE];
        return arr[extract(address, 21, 0)];
    }

    inline uint32_t get_32(uint64_t address) {
        // extract(address, 21, 0)
        // return *((uint32_t*) (arr + (address % CHUNKSIZE)));
        return *((uint32_t*) (arr + extract(address, 21, 0)));
    }

    inline uint64_t get_64(uint64_t address) {
        // return *((uint64_t*) (arr + (address % CHUNKSIZE)));
        return *((uint64_t*) (arr + extract(address, 21, 0)));
    }

    inline void set(uint64_t address, uint8_t data) {
        // cout << address << " " << address / CHUNKSIZE << " " << address % CHUNKSIZE << endl;
        // arr[address % CHUNKSIZE] = data;
        arr[extract(address, 21, 0)] = data;
    }

    inline void set_32(uint64_t address, uint32_t data) {
        // *((uint32_t*) (arr + (address % CHUNKSIZE))) = data;
        *((uint32_t*) (arr + extract(address, 21, 0))) = data;
    }

    inline void set_64(uint64_t address, uint64_t data) {
        // *((uint64_t*) (arr + (address % CHUNKSIZE))) = data;
        *((uint64_t*) (arr + extract(address, 21, 0))) = data;
    }

    inline void set_128(uint64_t address, unsigned __int128 data) {
        *((unsigned __int128*) (arr + extract(address, 21, 0))) = data;
    }
};

#endif // SPARSE_ARRAY_1_H
