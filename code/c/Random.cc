#include "Random.h"


// Use STL pseudo-random number generators with thread-local storage.

thread_local std::mt19937* _dp_rand_gen = NULL;

unsigned long RandInt(void) {
    if( __builtin_expect( _dp_rand_gen == NULL, false ) ) {
        std::random_device rd;
        _dp_rand_gen = new std::mt19937(rd());
    }

    return (*_dp_rand_gen)();
}

double RandDouble(void)  {
    if( __builtin_expect( _dp_rand_gen == NULL, false ) ) {
        std::random_device rd;
        _dp_rand_gen = new std::mt19937(rd());
    }

    return std::generate_canonical<double, 32>(*_dp_rand_gen);
}

