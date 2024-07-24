#pragma once

typedef unsigned int uint32_t;

#if defined(_WIN64)
    typedef unsigned long long uint_ptr;
#else
    typedef unsigned int uint_ptr;
#endif
