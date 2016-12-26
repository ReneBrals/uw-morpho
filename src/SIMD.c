#include "SIMD.h"
#include <stdio.h>
#include <string.h>
#include <omp.h>

// Stores the minimum of a[i] and b[i] into c[i]
void simdMin(unsigned char* c, unsigned char* a, unsigned char* b, size_t x){
    size_t i;
    for(i=0; i < x/32; i++){
        min32x8(&(c[i*32]),&(a[i*32]),&(b[i*32]));
    }

    for(i = x - (x % 32); i<x; i++){
            c[i] = b[i] < a[i] ? b[i] : a[i];
    }
}

// Stores the minimum of a[i] and b[i] into a[i]
void simdMinInPlace(unsigned char* a, unsigned char* b, size_t x){
    size_t i;
    for(i=0; i < x/32; i++){
        min32x8_ip(&(a[i*32]),&(b[i*32]));
    }

    for(i = x - (x % 32); i<x; i++){
        if(b[i] < a[i] ) a[i] = b[i];
    }
}

// Stores the maximum of a[i] and b[i] into c[i]
void simdMax(unsigned char* c, unsigned char* a, unsigned char* b, size_t x){
    size_t i;
    for(i=0; i < x/32; i++){
        max32x8(&(c[i*32]),&(a[i*32]),&(b[i*32]));
    }

    for(i = x - (x % 32); i<x; i++){
            c[i] = b[i] > a[i] ? b[i] : a[i];
    }
}

// Stores the maximum of a[i] and b[i] into a[i]
void simdMaxInPlace(unsigned char* a, unsigned char* b, size_t x){
    size_t i;
    for(i=0; i < x/32; i++){
        max32x8_ip(&(a[i*32]),&(b[i*32]));
    }

    for(i = x - (x % 32); i<x; i++){
        if(b[i] > a[i] ) a[i] = b[i];
    }
}

// Subtracts b[i] from a[i], storing the result in a[i]
void simdSubInPlace(unsigned char* a, unsigned char* b, size_t x){
    size_t i;
    for(i=0; i < x/32; i++){
        sub32x8_ip(&(a[i*32]),&(b[i*32]));
    }

    for(i = x - (x % 32); i<x; i++){
        a[i] = a[i] > b[i] ? a[i]-b[i] : 0;
    }
}
