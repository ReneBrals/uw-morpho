#ifndef _SIMDMIN
#define _SIMDMIN

#include "emmintrin.h"
#include <immintrin.h>
#include <smmintrin.h>

#define _mm_cmpge_epu8(a, b) \
        _mm_cmpeq_epi8(_mm_max_epu8(a, b), a)

#define _mm_cmple_epu8(a, b) _mm_cmpge_epu8(b, a)

#define _mm256_cmpge_epu8(a, b) \
        _mm256_cmpeq_epi8(_mm256_max_epu8(a, b), a)

#define _mm256_cmple_epu8(a, b) _mm256_cmpge_epu8(b, a)


static inline void min16x8(unsigned char* c, unsigned char* a, unsigned char* b){
    __m128i x = _mm_loadu_si128((const __m128i*)a);
    __m128i y = _mm_loadu_si128((const __m128i*)b);
    //__m128i res = _mm_min_epu8(x, y);
    _mm_storeu_si128((__m128i*)c,_mm_min_epu8(x, y));
}

static inline void min16x8_ip(unsigned char* a, unsigned char* b){
    __m128i x = _mm_loadu_si128((const __m128i*)a);
    __m128i y = _mm_loadu_si128((const __m128i*)b);
    //__m128i res = _mm_min_epu8(x, y);
    _mm_storeu_si128((__m128i*)a,_mm_min_epu8(x, y));
}

static inline void min32x8(unsigned char* c, unsigned char* a, unsigned char* b){
    __m256i x = _mm256_loadu_si256((const __m256i*)a);
    __m256i y = _mm256_loadu_si256((const __m256i*)b);
    _mm256_storeu_si256((__m256i*)c,_mm256_min_epu8(x, y));
}

static inline void min32x8_ip(unsigned char* a, unsigned char* b){
    __m256i x = _mm256_loadu_si256((const __m256i*)a);
    __m256i y = _mm256_loadu_si256((const __m256i*)b);
    _mm256_storeu_si256((__m256i*)a,_mm256_min_epu8(x, y));
}

void simdMin(unsigned char* c, unsigned char* a, unsigned char* b, size_t x);
void simdMinInPlace(unsigned char* a, unsigned char* b, size_t x);

#endif
