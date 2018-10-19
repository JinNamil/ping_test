/**
  ******************************************************************************
  * @file    main.c
  * @author  eWBM
  * @version v1.0
  * @date    29-07-2016
  * @brief   Gyroscope Sensor application
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
#include "ewbm_gyroscope.h"

#define USE_UART_DEBUG    1

#if USE_UART_DEBUG
    #include "ewbm_uart.h"
    static tUART_HANDLE debugHandle;
    #define APP_GYRO_DBG(fmt,args...)    UART_Printf(fmt,##args)
#else
    #define APP_GYRO_DBG(fmt,args...)
#endif



#ifdef __FREE_RTOS__
void APP_GYRO_Test(void * pvParam)
{
    uint8_t id = 0;
    float x, y, z = 0;

    GYRO_Init();
    GYRO_GetDeviceID(&id);
    GYRO_Control(MPU6050_REG_PWR_MGMT_1, 0);

    for(;;) {
        GYRO_ReadXYZ(&x, &y, &z);
        APP_GYRO_DBG("[FREERTOS] APP_GYRO_Test() - APP GYRO x:%3.1f, y:%3.1f, z:%3.1f\r\n", x, y, z);
        vTaskDelay(1000);
    }
}
#else
void APP_GYRO_Test()
{
    uint8_t id = 0;
    float x, y, z = 0;

    GYRO_Init();
    GYRO_GetDeviceID(&id);
    GYRO_Control(MPU6050_REG_PWR_MGMT_1, 0);

    for(;;) {
        GYRO_ReadXYZ(&x, &y, &z);
        APP_GYRO_DBG("APP_GYRO_Test() - APP GYRO x:%3.1f, y:%3.1f, z:%3.1f\r\n", x, y, z);
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
    xTaskCreate(APP_GYRO_Test, "GYRO", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL);

    for(;;) {
        vTaskDelay(1000);
    }
#else
    APP_GYRO_Test();

    for(;;) {
        __m_delay(1000);
    }
#endif

    return 0;
}
