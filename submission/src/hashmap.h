#pragma once
#include "common.h"

#define LARGE_PRIME     (23456789)


//template<typename T, int PN = LARGE_PRIME>
#define PN      (LARGE_PRIME)
typedef void* TDummy;
class TSHashSet
{
private:
    std::unordered_map<uint64, TDummy> _bucket[PN];
    struct {
        int Count;
        std::atomic_bool Lock;
    } _meta[PN];

public:
    TSHashSet() 
        : _bucket{}, _meta{}
    {
        memset(_bucket, 0x00, sizeof(_bucket));
        memset(_meta, 0x00, sizeof(_meta));
    }

    TDummy& GetOrAdd(uint64 hash) {
        uint32 idx = hash % PN;
        while (true) {
            bool expected = false;
            if (_meta[idx].Lock.compare_exchange_weak(expected, true)) break;
        }
        TDummy& firstGramInfo = _bucket[idx][hash];
        _meta[idx].Lock.store(false);
        return firstGramInfo;
    }
};
