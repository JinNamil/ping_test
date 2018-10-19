/**
  ******************************************************************************
  * @file    main.c
  * @author  eWBM
  * @version v1.0
  * @date    29-07-2016
  * @brief   Ambient Light Sensor application
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
#include "ewbm_light.h"

#define USE_UART_DEBUG    1

#if USE_UART_DEBUG
#include "ewbm_uart.h"
static tUART_HANDLE debugHandle;
#define APP_ALS_DBG(fmt,args...)    UART_Printf(fmt,##args)
#else
#define APP_ALS_DBG(fmt,args...)
#endif


#ifdef __FREE_RTOS__
void APP_ALS_Test(void * pvParam)
{
    uint16_t lux = 0;

    ALS_Init(LIGHT_FREQ_100KHZ);
    ALS_Enable();

    for(;;) {
        ALS_ReadResolution(CONTINUOUSLY_H_RESOLUTION_MODE2, &lux);
        APP_ALS_DBG("[FREERTOS] ALS High Resolution - Lux: (0x%x) %d lx \r\n", lux, lux);
        vTaskDelay(1000);
    }
}
#else
void APP_ALS_Test()
{
    uint16_t lux = 0;

    ALS_Init(LIGHT_FREQ_100KHZ);
    ALS_Enable();

    for(;;) {
        ALS_ReadResolution(CONTINUOUSLY_H_RESOLUTION_MODE2, &lux);
        APP_ALS_DBG("ALS High Resolution - Lux: (0x%x) %d lx \r\n", lux, lux);
        __m_delay(1000);
    }
}
#endif


/*
 * main: initialize and start the system
 */
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

#ifdef __FREE_RTOS__
    xTaskCreate(APP_ALS_Test, "ALS", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL);
    for(;;){
        vTaskDelay(2000);
    }
#else
    APP_ALS_Test();
    for(;;){
        __m_delay(2000);
    }
#endif

    return 0;
}

