#ifndef STD_TYPES_H_
#define STD_TYPES_H_

#define HIGH        (1u)
#define LOW         (0u)

#define OUTPUT        (1u)
#define INPUT         (0u)

#ifndef TRUE
#define TRUE        (1u)
#endif

#ifndef FALSE
#define FALSE       (0u)
#endif

typedef unsigned char         uint8;
typedef signed char           sint8;
typedef unsigned short        uint16;
typedef signed short          sint16;
typedef unsigned long         uint32;
typedef signed long           sint32;
typedef unsigned long long    uint64;
typedef signed long long      sint64;
typedef float                 float32;
typedef double                float64;

#endif