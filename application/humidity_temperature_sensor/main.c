/**
  ******************************************************************************
  * @file    main.c
  * @author  eWBM
  * @version v1.0
  * @date    29-07-2016
  * @brief   Humidity Temperature Sensor application
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
#include "ewbm_htsensor.h"

#define USE_UART_DEBUG    1

#if USE_UART_DEBUG
#include "ewbm_uart.h"
static tUART_HANDLE debugHandle;
#define APP_HTS_DBG(fmt,args...)    UART_Printf(fmt,##args)
#else
#define APP_HTS_DBG(fmt,args...)
#endif


static void APP_HTS_TempMeasure(void)
{
    uint8_t  recvBuf[2];
    uint16_t readData = 0;

    HTS_ReadTemp(MEASURE_TEMPERATURE_HOLD_MASTER_MODE, recvBuf, &readData);

    APP_HTS_DBG("- Temp buf[0]: (0x%x), buf[1]: (0x%x) \r\n", recvBuf[0], recvBuf[1]);
    APP_HTS_DBG("- Temperature: (%d:%f) \r\n", readData, (float)((((double)readData*175.72)/65536.0)-46.85));
}

static void APP_HTS_HumidityMeasure(void)
{
    uint8_t  recvBuf[2];
    uint16_t readData = 0;

    HTS_ReadHumidity(MEASURE_RELATIVE_HUMIDITY_HOLD_MASTER_MODE, recvBuf, &readData);

    APP_HTS_DBG("- Humi buf[0]: (0x%x), buf[1]: (0x%x) \r\n", recvBuf[0], recvBuf[1]);
    APP_HTS_DBG("- Humidity: (%d:%d) \r\n", readData, ((readData*125)/65536)-6);
}

#ifdef __FREE_RTOS__
void APP_HTS_Test(void * pvParam)
{
    APP_HTS_DBG("** APP Humidity/Temperature Sensor Test Start **\r\n");

    for(;;) {
        APP_HTS_TempMeasure();
        vTaskDelay(1000);
        APP_HTS_HumidityMeasure();
        vTaskDelay(1000);
    }
}
#else
void APP_HTS_Test()
{
    APP_HTS_TempMeasure();
    __m_delay(1000);
    APP_HTS_HumidityMeasure();
    __m_delay(1000);
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

    HTS_Init(HTS_FREQ_100KHZ);

    HTS_Enable();

#ifdef __FREE_RTOS__
    // Create APP_HTS_Test task
    xTaskCreate(APP_HTS_Test, "APP_HTS_Test", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL);

    for(;;){
        vTaskDelay(2000);
    }
#else

    for(;;){
        APP_HTS_Test();
        __m_delay(1000);
    }
#endif

    return 0;
}

