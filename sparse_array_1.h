#ifndef SPARSE_ARRAY_1_H
#define SPARSE_ARRAY_1_H

#include <cstdlib>
#include <cstdint>
#include <iostream>

#include "core_functions.h"

using namespace std;

const uint64_t CHUNKSIZE = 4194304;

typedef struct atomic32data {
    uint32_t data;
    bool semaphore;
} atomic32data;

typedef struct atomic64data {
    uint32_t data;
    bool semaphore;
} atomic64data;

class A1 {
    uint8_t *arr;
    bool *semaphores;
public:
    A1() {
        arr = new uint8_t[CHUNKSIZE];
        semaphores = new bool[CHUNKSIZE];
    }

    ~A1() {
        delete[] arr;
        delete[] semaphores;
    }

    inline uint8_t get(uint64_t address) {
        return arr[extract(address, 21, 0)];
    }

    inline uint32_t get_32(uint64_t address) {
        return *((uint32_t*) (arr + extract(address, 21, 0)));
    }

    inline uint64_t get_64(uint64_t address) {
        return *((uint64_t*) (arr + extract(address, 21, 0)));
    }

    inline atomic32data* get_32_atomic(uint64_t address) {
        atomic32data* retdata = new atomic32data;
        retdata->data = *((uint32_t*) (arr + extract(address, 21, 0)));
        retdata->semaphore = *((bool*) (semaphores + extract(address, 21, 0)));
        return retdata;
    }

    inline atomic64data* get_64_atomic(uint64_t address) {
        atomic64data* retdata = new atomic64data;
        retdata->data = *((uint64_t*) (arr + extract(address, 21, 0)));
        retdata->semaphore = *((bool*) (semaphores + extract(address, 21, 0)));
        return retdata;
    }

    inline void set(uint64_t address, uint8_t data) {
        arr[extract(address, 21, 0)] = data;
    }

    inline void set_32(uint64_t address, uint32_t data) {
        *((uint32_t*) (arr + extract(address, 21, 0))) = data;
    }

    inline void set_64(uint64_t address, uint64_t data) {
        *((uint64_t*) (arr + extract(address, 21, 0))) = data;
    }

    inline void set_32_atomic(uint64_t address, uint32_t data) {
        *((uint32_t*) (arr + extract(address, 21, 0))) = data;
        *((bool*) (semaphores + extract(address, 21, 0))) = true;
    }

    inline void set_64_atomic(uint64_t address, uint64_t data) {
        *((uint64_t*) (arr + extract(address, 21, 0))) = data;
        *((bool*) (semaphores + extract(address, 21, 0))) = true;
    }

    inline void set_128(uint64_t address, unsigned __int128 data) {
        *((unsigned __int128*) (arr + extract(address, 21, 0))) = data;
    }
};

#endif // SPARSE_ARRAY_1_H
