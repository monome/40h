#ifndef __TYPES_H__
#define __TYPES_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef int8_t     sint8;
typedef uint8_t    uint8;

typedef int16_t    sint16;
typedef uint16_t   uint16;

typedef int32_t    sint32;
typedef uint32_t   uint32;

typedef int64_t    sint64;
typedef uint64_t   uint64;

typedef union {
    uint16 uInt;
    uint8  uChar[2];
} UInt;

typedef union {
    uint32 uLong;
    uint16 uInt[2];
    uint8  uChar[4];
} ULong;

#ifndef __cplusplus
typedef uint8 bool;
#endif

#ifndef TRUE
#define TRUE       (1)
#endif

#ifndef FALSE
#define FALSE      (0)
#endif

#ifndef true
#define true        TRUE
#endif

#ifndef false
#define false       FALSE
#endif

#ifdef __cplusplus
}
#endif

#endif
