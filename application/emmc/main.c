/**
  ******************************************************************************
  * @file    main.c
  * @author  eWBM
  * @version v1.0
  * @date    29-07-2016
  * @brief   EMMC application
  ******************************************************************************
  * @attention
  *
  * Copyright(c) 2015 ~ 2020 eWBM Korea , Ltd.
  * All rights reserved.
  * This software contains confidential information of eWBM Korea Co.,Ltd.
  * and unauthorized distribution of this software, or any portion of it, are
  * prohibited.
  *
  ******************************************************************************
  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ewbm_global_config.h"


#ifdef __FREE_RTOS__
#include "FreeRTOS.h"
#include "task.h"
#endif

#include "ewbm_delay.h"
#include "ewbm_emmc.h"

#define USE_UART_DEBUG    1

#if USE_UART_DEBUG
#include "ewbm_uart.h"
static tUART_HANDLE debugHandle;
#define EMMC_DBG(fmt,args...)    UART_Printf(fmt,##args)
#else
#define EMMC_DBG(fmt,args...)
#endif

#define BLOCK_CNT    (1)


void mem_clear(int num_word, uint32_t* memory)
{
    int i;
    for (i = 0; i < num_word / sizeof(uint32_t); i++)
        *(memory++) = 0x00000000;
}

void mem_count_write(int num_word, uint32_t* memory)
{
    int i;
    uint8_t byte_pattern[4];

    byte_pattern[0] = 0;
    for (i = 0; i < num_word / sizeof(uint32_t); i++)
    {
        byte_pattern[1] = byte_pattern[0] + 1;
        byte_pattern[2] = byte_pattern[1] + 1;
        byte_pattern[3] = byte_pattern[2] + 1;

        *(memory++) = byte_pattern[0] + (byte_pattern[1] << 8) + (byte_pattern[2] << 16) + (byte_pattern[3] << 24);

        byte_pattern[0] = byte_pattern[3] + 1;
    }
}

int mem_count_read_compare(int num_word, uint32_t* memory)
{
    int err = 0;
    int i;
    int pattern;
    uint8_t byte_pattern[4];

    byte_pattern[0] = 0;
    for (i = 0; i < num_word / sizeof(uint32_t); i++)
    {
        byte_pattern[1] = byte_pattern[0] + 1;
        byte_pattern[2] = byte_pattern[1] + 1;
        byte_pattern[3] = byte_pattern[2] + 1;

        pattern = byte_pattern[0] + (byte_pattern[1] << 8) + (byte_pattern[2] << 16) + (byte_pattern[3] << 24);

        if (*(memory++) != pattern)
        {
            err++;
            break;
        }
        byte_pattern[0] = byte_pattern[3] + 1;
    }

    if (err)
        return -1;
    else
        return 0;
}

void emmc_test(void)
{
    tEMMC_HANDLE emmcHandle;
    uint8_t writeBuff[512*BLOCK_CNT];
    int ret = 0;

    mem_count_write(BLOCK_CNT, (uint32_t*)writeBuff);

    //emmcHandle.clock = 1000000;
    emmcHandle.clock = 48000000;
    emmcHandle.busWidth = 8;
    EMMC_Init(&emmcHandle);

    EMMC_WriteBlock(&emmcHandle, writeBuff, 0, BLOCK_CNT);

    EMMC_ReadBlock(&emmcHandle, writeBuff, 0, BLOCK_CNT);

    ret = mem_count_read_compare(BLOCK_CNT, (uint32_t*)writeBuff);

    if ( ret == 0 )
        EMMC_DBG("Test Success\r\n");
    else
        EMMC_DBG("Test Faile\r\n");
}


int main(void)
{
#if USE_UART_DEBUG
    // Set UART1 handle
    debugHandle.bank = EWBM_DEBUG_UART_PORT;
    debugHandle.baudRate = UART_BAUDRATE_115200;
    debugHandle.wordLength = UART_WORDLENGTH_8BIT;

    // Initialize UART1 handle
    UART_Init(&debugHandle);

    // Enable fifo UART1
    UART_EnableFifo(&debugHandle, UART_FIFO_LEVEL_7_OVER_8, UART_FIFO_LEVEL_1_OVER_8);

    // Enable UART1 to TX mode
    UART_EnableUart(&debugHandle, UART_MODE_TX);

    // Initialize debug with the UART1 handle
    UART_PrintInit(&debugHandle);
#endif

    EMMC_DBG("MMC Test\r\n");

    while (1) {
        emmc_test();
        __m_delay(1000);
    }

    return 0;
}

