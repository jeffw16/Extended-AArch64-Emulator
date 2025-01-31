#ifndef SPARSE_ARRAY_3_H
#define SPARSE_ARRAY_3_H

using namespace std;

#include "sparse_array_2.h"

extern const uint64_t CHUNKSIZE;

class A3 {
    A2 **arrs;
public:
    A3() {
        arrs = new A2*[CHUNKSIZE];
    }

    ~A3() {
        delete[] arrs;
    }

    inline uint8_t get(uint64_t address) {
        if ( arrs[address >> 44] != NULL ) {
            return arrs[address >> 44]->get(extract(address, 43, 0));
        }
        return 0;
    }

    inline uint32_t get_32(uint64_t address) {
        if ( arrs[address >> 44] != NULL ) {
            return arrs[address >> 44]->get_32(extract(address, 43, 0));
        } else {
            return 0;
        }
    }

    inline uint64_t get_64(uint64_t address) {
        if ( arrs[address >> 44] != NULL ) {
            return arrs[address >> 44]->get_64(extract(address, 43, 0));
        }
        return 0;
    }

    inline atomic32data* get_32_atomic(uint64_t address) {
        if ( arrs[address >> 44] != NULL ) {
            return arrs[address >> 44]->get_32_atomic(extract(address, 43, 0));
        } else {
            return new atomic32data;
        }
    }

    inline atomic64data* get_64_atomic(uint64_t address) {
        if ( arrs[address >> 44] != NULL ) {
            return arrs[address >> 44]->get_64_atomic(extract(address, 43, 0));
        }
        return new atomic64data;
    }

    inline void set(uint64_t address, uint8_t data) {
        // cout << address << " " << address >> 44 << " " << extract(address, 43, 0) << endl;
        if ( arrs[address >> 44] != NULL ) {
            arrs[address >> 44]->set(extract(address, 43, 0), data);
        } else {
            arrs[address >> 44] = new A2;
            arrs[address >> 44]->set(extract(address, 43, 0), data);
        }
    }

    inline void set_32(uint64_t address, uint32_t data) {
        if ( arrs[address >> 44] != NULL ) {
            arrs[address >> 44]->set_32(extract(address, 43, 0), data);
        } else {
            arrs[address >> 44] = new A2;
            arrs[address >> 44]->set_32(extract(address, 43, 0), data);
        }
    }

    inline void set_64(uint64_t address, uint64_t data) {
        if ( arrs[address >> 44] != NULL ) {
            arrs[address >> 44]->set_64(extract(address, 43, 0), data);
        } else {
            arrs[address >> 44] = new A2;
            arrs[address >> 44]->set_64(extract(address, 43, 0), data);
        }
    }

    inline void set_32_atomic(uint64_t address, uint32_t data) {
        if ( arrs[address >> 44] != NULL ) {
            arrs[address >> 44]->set_32_atomic(extract(address, 43, 0), data);
        } else {
            arrs[address >> 44] = new A2;
            arrs[address >> 44]->set_32_atomic(extract(address, 43, 0), data);
        }
    }

    inline void set_64_atomic(uint64_t address, uint64_t data) {
        if ( arrs[address >> 44] != NULL ) {
            arrs[address >> 44]->set_64_atomic(extract(address, 43, 0), data);
        } else {
            arrs[address >> 44] = new A2;
            arrs[address >> 44]->set_64_atomic(extract(address, 43, 0), data);
        }
    }

    inline void set_128(uint64_t address, unsigned __int128 data) {
        if ( arrs[address >> 44] != NULL ) {
            arrs[address >> 44]->set_128(extract(address, 43, 0), data);
        } else {
            arrs[address >> 44] = new A2;
            arrs[address >> 44]->set_128(extract(address, 43, 0), data);
        }
    }
};

#endif // SPARSE_ARRAY_3_H
