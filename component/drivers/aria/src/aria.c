/**
  ******************************************************************************
  * @file    aria.c
  * @author  eWBM
  * @version  0.8
  * @date    2016-07-12
  * @brief   ARIA
  ******************************************************************************
  * @attention
  *
  * Copyright(c) 2015 ~ 2020 eWBM Korea , Ltd.
  * All rights reserved.
  * This software contains confidential information of eWBM Korea Co.,Ltd.
  * and unauthorized distribution of this software, or any portion of it, are
  * prohibited.
  *
  * Brief history
  * -------------
  *  2016-07-12 : Draft by Leon
  *  2016-07-14 : ARIA Encrypt/Decrypt optimize and enhancement
  *  a performence
  ******************************************************************************
  */

#include <string.h>
#include <stdint.h>
#include "aria.h"
#include "ewbm_delay.h"

#define ARIA_DATA_COPY_BLOCK_NUM        (4)

#define aria_htonl(x) \
({ \
   uint32_t __x = (x); \
   ((uint32_t)( \
       (((uint32_t)(__x) & (uint32_t)0x000000ffUL) << 24) | \
       (((uint32_t)(__x) & (uint32_t)0x0000ff00UL) <<  8) | \
       (((uint32_t)(__x) & (uint32_t)0x00ff0000UL) >>  8) | \
       (((uint32_t)(__x) & (uint32_t)0xff000000UL) >> 24) )); \
})


int ESAL_ARIA_SetKey(void* key, eARIA_KEYSIZE keysize)
{
    tARIA_REG* baseAddr = ARIA;

    volatile uint32_t* keycontext = NULL;
    uint32_t copykey_len = 0;
    uint32_t* write_dummy = (uint32_t*)key;
    int x = 0;
    
    uint32_t cache_config = ICACHE->cache_config;
    uint32_t cache_ctrl   = ICACHE->cache_ctrl;

    ICACHE->cache_ctrl   = 0;
    ICACHE->cache_config = 0;

    keycontext = (volatile uint32_t*)&baseAddr->keylr3;

    switch ( keysize )
    {
        case ARIA_KEY_SIZE_128B:
            copykey_len = 4;
            baseAddr->cr = (ARIA_KEY_SIZE_128B << 1);
            break;

        case ARIA_KEY_SIZE_192B:
            copykey_len = 6;
            baseAddr->cr = (ARIA_KEY_SIZE_192B << 1);
            break;

        case ARIA_KEY_SIZE_256B:
            copykey_len = 8;
            baseAddr->cr = (ARIA_KEY_SIZE_256B << 1);
            break;

        case ARIA_KEY_SIZE_NOTUSED:
            baseAddr->cr = (ARIA_KEY_SIZE_NOTUSED << 1);
            break;

        default:
            return ARIA_FAIL;
    }

    for ( x=0; x<copykey_len; x++ )
        *keycontext-- = aria_htonl(*write_dummy++);
    
    ICACHE->cache_config = cache_config;
    ICACHE->cache_ctrl   = cache_ctrl;

    return ARIA_OK;
}

int ESAL_ARIA_Decrypt(uint8_t* cipher_text, uint8_t* plain_text)
{
    tARIA_REG* baseAddr = ARIA;
    int i = 0;
    uint32_t* cipher_dummy = (uint32_t*)cipher_text;
    uint32_t* plain_dummy   = (uint32_t*)plain_text;
    
    uint32_t cache_config = ICACHE->cache_config;
    uint32_t cache_ctrl   = ICACHE->cache_ctrl;

    ICACHE->cache_ctrl   = 0;
    ICACHE->cache_config = 0;

    baseAddr->cr |= (ARIA_REG_FIELD_ENABLE << 10);
    baseAddr->cr |= (ARIA_REG_FIELD_ENABLE << 9);
    baseAddr->cr |= (ARIA_MODE_DECRYPTION << 3);
    baseAddr->cr |= (ARIA_REG_FIELD_ENABLE << 0);

    for ( i=0; i<ARIA_DATA_COPY_BLOCK_NUM; i++ )
        baseAddr->dinr = aria_htonl(cipher_dummy[i]); // Data is write into Data Input Register

    // Interrupt Clear
    while ( !(baseAddr->sr & 0x01) );

    for ( i=0; i<ARIA_DATA_COPY_BLOCK_NUM; i++ )
        plain_dummy[i] = aria_htonl(baseAddr->doutr); // ARIA DATA OUTPUT REGISTER

    baseAddr->cr &= ~(ARIA_REG_FIELD_ENABLE << 0);    
    baseAddr->cr &= 0x06;
    
    ICACHE->cache_config = cache_config;
    ICACHE->cache_ctrl   = cache_ctrl;
    
    return ARIA_OK;
}

int ESAL_ARIA_Encrypt(uint8_t* plain_text, uint8_t* cipher_text)
{
    tARIA_REG* baseAddr = ARIA;
    int i = 0;
    uint32_t* plain_dummy = (uint32_t*)plain_text;
    uint32_t* cipher_dummy = (uint32_t*)cipher_text;
    
    uint32_t cache_config = ICACHE->cache_config;
    uint32_t cache_ctrl   = ICACHE->cache_ctrl;

    ICACHE->cache_ctrl   = 0;
    ICACHE->cache_config = 0;

    baseAddr->cr |= (ARIA_REG_FIELD_ENABLE << 10);
    baseAddr->cr |= (ARIA_REG_FIELD_ENABLE << 9);
    baseAddr->cr |= (ARIA_MODE_ENCRYPTION << 3);
    baseAddr->cr |= (ARIA_REG_FIELD_ENABLE << 0);

    for ( i=0; i<ARIA_DATA_COPY_BLOCK_NUM; i++ )
        baseAddr->dinr = aria_htonl(plain_dummy[i]); // Data is write into Data Input Register

    // Interrupt Clear
    while ( !(baseAddr->sr & 0x00000001) );

    for ( i=0; i<ARIA_DATA_COPY_BLOCK_NUM; i++ )
       cipher_dummy[i] = aria_htonl(baseAddr->doutr); // ARIA DATA OUTPUT REGISTER

    baseAddr->cr &= ~(ARIA_REG_FIELD_ENABLE << 0);    
    baseAddr->cr &= 0x06;
    
    ICACHE->cache_config = cache_config;
    ICACHE->cache_ctrl   = cache_ctrl;
    
    return ARIA_OK;
}

int ESAL_ARIA_Init(void)
{
    tARIA_REG* baseAddr = ARIA;

    if ( baseAddr->peri_id != ARIA_PERI_PARTNUM )
        return ARIA_FAIL;

    baseAddr->cr = 0x0UL;

    // ARIA Field Initialize
    baseAddr->dinr   = 0x0UL;
    baseAddr->keyrr0 = 0x0UL;
    baseAddr->keyrr1 = 0x0UL;
    baseAddr->keyrr2 = 0x0UL;
    baseAddr->keyrr3 = 0x0UL;
    baseAddr->keylr0 = 0x0UL;
    baseAddr->keylr1 = 0x0UL;
    baseAddr->keylr2 = 0x0UL;
    baseAddr->keylr3 = 0x0UL;

    return ARIA_OK;
}