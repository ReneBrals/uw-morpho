#include "SIMD.h"
#include <stdio.h>
#include <string.h>

#ifdef __AVX2__
//Processing 32 bytes at a time will require AVX2 support (Intel Haswell and newer)

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

#elif __SSE2__
//Processing 16 bytes at a time will require SSE2 support (Intel NetBurst and newer)

// Stores the minimum of a[i] and b[i] into c[i]
void simdMin(unsigned char* c, unsigned char* a, unsigned char* b, size_t x){
    size_t i;
    for(i=0; i < x/16; i++){
        min16x8(&(c[i*16]),&(a[i*16]),&(b[i*16]));
    }

    for(i = x - (x % 16); i<x; i++){
            c[i] = b[i] < a[i] ? b[i] : a[i];
    }
}

// Stores the minimum of a[i] and b[i] into a[i]
void simdMinInPlace(unsigned char* a, unsigned char* b, size_t x){
    size_t i;
    for(i=0; i < x/16; i++){
        min16x8_ip(&(a[i*16]),&(b[i*16]));
    }

    for(i = x - (x % 16); i<x; i++){
        if(b[i] < a[i] ) a[i] = b[i];
    }
}

// Stores the maximum of a[i] and b[i] into c[i]
void simdMax(unsigned char* c, unsigned char* a, unsigned char* b, size_t x){
    size_t i;
    for(i=0; i < x/16; i++){
        max16x8(&(c[i*16]),&(a[i*16]),&(b[i*16]));
    }

    for(i = x - (x % 16); i<x; i++){
            c[i] = b[i] > a[i] ? b[i] : a[i];
    }
}

// Stores the maximum of a[i] and b[i] into a[i]
void simdMaxInPlace(unsigned char* a, unsigned char* b, size_t x){
    size_t i;
    for(i=0; i < x/16; i++){
        max16x8_ip(&(a[i*16]),&(b[i*16]));
    }

    for(i = x - (x % 16); i<x; i++){
        if(b[i] > a[i] ) a[i] = b[i];
    }
}

// Subtracts b[i] from a[i], storing the result in a[i]
void simdSubInPlace(unsigned char* a, unsigned char* b, size_t x){
    size_t i;
    for(i=0; i < x/16; i++){
        sub16x8_ip(&(a[i*16]),&(b[i*16]));
    }

    for(i = x - (x % 16); i<x; i++){
        a[i] = a[i] > b[i] ? a[i]-b[i] : 0;
    }
}

#else
//If SSE2 is not available, fall back to the boring way of doing things...

// Stores the minimum of a[i] and b[i] into c[i]
void simdMin(unsigned char* c, unsigned char* a, unsigned char* b, size_t x){
    size_t i;

    for(i = 0; i < x; i++){
        c[i] = b[i] < a[i] ? b[i] : a[i];
    }
}

// Stores the minimum of a[i] and b[i] into a[i]
void simdMinInPlace(unsigned char* a, unsigned char* b, size_t x){
    size_t i;

    for(i = 0; i < x; i++){
        if(b[i] < a[i] ) a[i] = b[i];
    }
}

// Stores the maximum of a[i] and b[i] into c[i]
void simdMax(unsigned char* c, unsigned char* a, unsigned char* b, size_t x){
    size_t i;

    for(i = 0; i < x; i++){
        c[i] = b[i] > a[i] ? b[i] : a[i];
    }
}

// Stores the maximum of a[i] and b[i] into a[i]
void simdMaxInPlace(unsigned char* a, unsigned char* b, size_t x){
    size_t i;

    for(i = 0; i < x; i++){
        if(b[i] > a[i] ) a[i] = b[i];
    }
}

// Subtracts b[i] from a[i], storing the result in a[i]
void simdSubInPlace(unsigned char* a, unsigned char* b, size_t x){
    size_t i;

    for(i = 0; i < x; i++){
        a[i] = a[i] > b[i] ? a[i]-b[i] : 0;
    }
}

#endif
