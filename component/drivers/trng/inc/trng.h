#ifndef __TRNG_H__
#define __TRNG_H__

#include "ewbm_device.h"

typedef enum
{
    TRNG_OK             = 0,
    TRNG_FAIL           = -1,
} eTRNG_ERROR;

void ESAL_TRNG_Init(void);
int  ESAL_TRNG_GetRandomData(void* out, uint32_t readSz, const void* reseed);
int  ESAL_TRNG_ReSeed(const void* nonce);
int  ESAL_TRNG_GetSeed(void* out);


#endif
