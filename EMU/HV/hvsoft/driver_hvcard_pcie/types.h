#ifndef _HV_TYPES_H
#define _HV_TYPES_H
#ifdef __KERNEL__
#include <linux/types.h>
#else
#include <stdint.h>
#endif
typedef unsigned long  ULONG;
typedef uint16_t  USHORT;
typedef uint8_t   UCHAR;
typedef uint32_t  UINT;
#endif
