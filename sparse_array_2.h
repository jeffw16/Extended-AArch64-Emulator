#ifndef SPARSE_ARRAY_2_H
#define SPARSE_ARRAY_2_H

using namespace std;

#include "sparse_array_1.h"

extern const uint64_t CHUNKSIZE;

class A2 {
public:
    A1 **arrs;
    A2() {
        arrs = new A1*[CHUNKSIZE];
    }

    ~A2() {
        delete[] arrs;
    }

    inline uint8_t get(uint64_t address) {
        if ( arrs[address >> 22] != NULL ) {
            return arrs[address >> 22]->get(extract(address, 21, 0));
        }
        return 0;
    }

    inline uint32_t get_32(uint64_t address) {
        if ( arrs[address >> 22] != NULL ) {
            return arrs[address >> 22]->get_32(extract(address, 21, 0));
        }
        return 0;
    }

    inline uint64_t get_64(uint64_t address) {
        if ( arrs[address >> 22] != NULL ) {
            return arrs[address >> 22]->get_64(extract(address, 21, 0));
        }
        return 0;
    }

    inline void set(uint64_t address, uint8_t data) {
        // cout << address << " " << address >> 22 << " " << address % CHUNKSIZE << endl;
        if ( arrs[address >> 22] != NULL ) {
            arrs[address >> 22]->set(extract(address, 21, 0), data);
        } else {
            arrs[address >> 22] = new A1;
            arrs[address >> 22]->set(extract(address, 21, 0), data);
        }
    }

    inline void set_32(uint64_t address, uint32_t data) {
        if ( arrs[address >> 22] != NULL ) {
            arrs[address >> 22]->set_32(extract(address, 21, 0), data);
        } else {
            arrs[address >> 22] = new A1;
            arrs[address >> 22]->set_32(extract(address, 21, 0), data);
        }
    }

    inline void set_64(uint64_t address, uint64_t data) {
        if ( arrs[address >> 22] != NULL ) {
            arrs[address >> 22]->set_64(extract(address, 21, 0), data);
        } else {
            arrs[address >> 22] = new A1;
            arrs[address >> 22]->set_64(extract(address, 21, 0), data);
        }
    }

    inline void set_128(uint64_t address, unsigned __int128 data) {
        if ( arrs[address >> 22] != NULL ) {
            arrs[address >> 22]->set_128(extract(address, 21, 0), data);
        } else {
            arrs[address >> 22] = new A1;
            arrs[address >> 22]->set_128(extract(address, 21, 0), data);
        }
    }
};

#endif // SPARSE_ARRAY_2_H
