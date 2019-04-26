#ifndef CORE_FUNCTIONS_H
#define CORE_FUNCTIONS_H

inline uint64_t extract(uint64_t full, int left, int right) {
    return (full << (63 - left)) >> (63 - left + right);
}

inline uint32_t extract32(uint32_t full, int left, int right) {
    return (full << (31 - left)) >> (31 - left + right);
}

inline uint64_t extract_single(uint64_t full, int pos) {
    return extract(full, pos, pos);
}

inline uint32_t extract_single32(uint32_t full, int pos) {
    return extract32(full, pos, pos);
}

#endif
