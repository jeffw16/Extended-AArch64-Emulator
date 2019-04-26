#include "sparse_array_1.h"

/*
A1::A1() {
    arr = new uint8_t[CHUNKSIZE];
}

A1::~A1() {
    delete[] arr;
}

uint8_t A1::get(uint64_t address) {
    return arr[address % CHUNKSIZE];
}

uint64_t A1::getBlock(uint64_t address, uint8_t size) {
    uint64_t data = 0;
    for ( uint8_t i = 0; i < size; i++ ) {
        data |= arr[(address % CHUNKSIZE) + i] << (i * 8);
    }
    return data;
}

void A1::set(uint64_t address, uint8_t data) {
    arr[address % CHUNKSIZE] = data;
}


uint8_t A1::&operator[](uint64_t address) {
    return arr[address % CHUNKSIZE];
}
*/
