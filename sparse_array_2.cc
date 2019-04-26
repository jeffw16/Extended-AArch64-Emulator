#include "sparse_array_2.h"

/*
A2::A2() {
    arrs = new A1*[CHUNKSIZE];
}

A2::~A2() {
    delete[] arrs;
}

uint8_t A2::get(uint64_t address) {
    if ( arrs[address % CHUNKSIZE] == NULL ) {
        return 0;
    }
    return arrs[address % CHUNKSIZE]->get(address % CHUNKSIZE);
}

uint64_t A2::getBlock(uint64_t address, uint8_t size) {
    // uint64_t data = (size == 64) ? (arrs[address % CHUNKSIZE]->getBlock(address % CHUNKSIZE, size/2) | (arrs[address % CHUNKSIZE + 4]->getBlock(address % CHUNKSIZE + 4, size/2) << 32)) : arrs[address % CHUNKSIZE]->getBlock(address % CHUNKSIZE, size);
    uint64_t data = 0;
    for ( uint8_t i = 0; i < size; i++ ) {
        data |= get(address + i) << (i * 8);
    }
    return data;
}

void A2::set(uint64_t address, uint8_t data) {
    if ( arrs[address % CHUNKSIZE] == NULL ) {
        arrs[address % CHUNKSIZE] = new A1;
    }
    arrs[address % CHUNKSIZE]->set(address % CHUNKSIZE, data);
}


uint8_t A2::&operator[](uint64_t address) {
    uint64_t modaddr = address % (CHUNKSIZE);
    if ( arrs[modaddr] == NULL ) {
        arrs[modaddr] = new A1;
    }
    return arrs[modaddr];
}
*/
