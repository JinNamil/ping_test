#ifndef __ARIA_HAL_H__
#define __ARIA_HAL_H__

#include "ewbm_device.h"

typedef enum
{
    ARIA_OK             = 0,
    ARIA_FAIL           = -1,
} eARIA_ERROR;

typedef enum
{
    ARIA_KEY_SIZE_128B    = 0x00,
    ARIA_KEY_SIZE_192B    = 0x01,
    ARIA_KEY_SIZE_256B    = 0x02,
    ARIA_KEY_SIZE_NOTUSED = 0x03
} eARIA_KEYSIZE;

int ESAL_ARIA_Init(void);
int ESAL_ARIA_Encrypt(uint8_t* plain_text, uint8_t* cipher_text);
int ESAL_ARIA_Decrypt(uint8_t* cipher_text, uint8_t* plain_text);
int ESAL_ARIA_SetKey(void* key, eARIA_KEYSIZE keysize);


#endif

