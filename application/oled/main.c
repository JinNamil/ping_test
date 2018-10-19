/**
  ******************************************************************************
  * @file    main.c
  * @author  eWBM
  * @version v1.0
  * @date    29-07-2016
  * @brief   OLED application
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
#include "ewbm_oled.h"

#define USE_UART_DEBUG    0

#if USE_UART_DEBUG
#include "ewbm_uart.h"
static tUART_HANDLE debugHandle;
#define OLED_DBG(fmt,args...)    UART_Printf(fmt,##args)
#else
#define OLED_DBG(fmt,args...)
#endif

#if USE_UART_DEBUG
// Mobile signal icon
const uint8_t c_chSingal816[16] = {0xFE,0x02,0x92,0x0A,0x54,0x2A,0x38,0xAA,0x12,0xAA,0x12,0xAA,0x12,0xAA,0x12,0xAA};
// Meesage icon
const uint8_t c_chMsg816[16] = {0x1F,0xF8,0x10,0x08,0x18,0x18,0x14,0x28,0x13,0xC8,0x10,0x08,0x10,0x08,0x1F,0xF8};
// Battery icon
const uint8_t c_chBat816[16] = {0x0F,0xFE,0x30,0x02,0x26,0xDA,0x26,0xDA,0x26,0xDA,0x26,0xDA,0x30,0x02,0x0F,0xFE};
// Bluetooth icon
const uint8_t c_chBluetooth88[8] = {0x18,0x54,0x32,0x1C,0x1C,0x32,0x54,0x18};
// GPRS icon
const uint8_t c_chGPRS88[8] = {0xC3,0x99,0x24,0x20,0x2C,0x24,0x99,0xC3};
// Alarm icon
const uint8_t c_chAlarm88[8] = {0xC3,0xBD,0x42,0x52,0x4E,0x42,0x3C,0xC3};


int get_string(tUART_HANDLE* debugHandle, uint8_t* buf, int max)
{
    int cnt = 0;
    char data;

    memset((void *)buf, 0x00, max);
    do {
        UART_Receive(debugHandle, (uint8_t*)&data, 1, 0);
        UART_Transmit(debugHandle, (uint8_t*)&data, 1, 0);
        if(cnt > max) {
            return EWBMERR;
        }
        else {
            if(data == 0x7f) {
                if(cnt > 0) {
                    buf[cnt-1] = buf[cnt] = 0;
                    cnt--;
                }
            }
            else {
                buf[cnt] = data;
                cnt++;
            }
        }
    } while(data != 0x0D);
    buf[cnt-1] = '\0';
    return EWBMOK;
}

void string_test(tUART_HANDLE* debugHandle)
{
    int ret;
    uint8_t input[256];
    uint8_t font[2];
    uint8_t posX[4];
    uint8_t posY[3];
    uint8_t scroll[2];
    uint8_t color[2];
    uint8_t interval[6];
    uint8_t fontSize;
    uint32_t startX, startY;
    uint32_t height = 0;
    uint32_t delay = 0;
    uint32_t i = 0;
    int temp;

    // Input string
    do {
        OLED_DBG("================ OLED String Test ================\r\n");
        OLED_DBG("Please input string (max 256charators) : ");
        ret = get_string(debugHandle, (uint8_t*)&input[0], 257);
        OLED_DBG("\r\n");
    }
    while(ret != EWBMOK);

    // Select font
    do {
        OLED_DBG("==================================================\r\n");
        OLED_DBG("1. 12\r\n2. 16\r\n");
        OLED_DBG("Please select font size : ");
        ret = get_string(debugHandle, (uint8_t *)&font[0], 2);
        OLED_DBG("\r\n");
    }
    while(ret != EWBMOK && (font[0] < 0x31 || font[0] > 0x32));
    fontSize = font[0]==0x31?(OLED_FONT_12):(OLED_FONT_16);


    // Input x coordinate
    do {
        OLED_DBG("==================================================\r\n");
        OLED_DBG("Please input X position (min 0, max 127) : ");
        ret = get_string(debugHandle, (uint8_t *)&posX[0], 4);
        OLED_DBG("\r\n");
    }
    while((ret != EWBMOK) && (-1 < atoi((char*)posX)) && (atoi((char*)posX) < OLED_WIDTH));
    startX = atoi((char*)posX);

    // Input y coordinate
    do {
        OLED_DBG("==================================================\r\n");
        OLED_DBG("Please input Y position (min 0, max 63) : ");
        ret = get_string(debugHandle, (uint8_t *)&posY[0], 3);
        OLED_DBG("\r\n");
    }
    while((ret != EWBMOK) && (-1 < atoi((char*)posY)) && (atoi((char*)posY) < OLED_HEIGHT));
    startY = atoi((char*)posY);

    // Input font color
    do {
        OLED_DBG("==================================================\r\n");
        OLED_DBG("1. black\r\n2. blue\r\n");
        OLED_DBG("Please select string color : ");
        ret = get_string(debugHandle, (uint8_t *)&color[0], 2);
        OLED_DBG("\r\n");
    }
    while(ret != EWBMOK && (color[0] < 0x31 || color[0] > 0x32));

    // Check string can be displayed specific area
    temp = (OLED_WIDTH/(fontSize/2))*((OLED_HEIGHT-startY-fontSize)/fontSize);
    if(temp < 0) {
        temp = 0;
    }

    if(strlen((char*)input) > ((OLED_WIDTH-startX)/(fontSize/2))+temp) {
        // If that can't be select scroll mode
        do {
            OLED_DBG("==================================================\r\n");
            OLED_DBG("1. auto scroll\r\n2. non scroll\r\n");
            OLED_DBG("Please select scroll mode : ");
            ret = get_string(debugHandle, (uint8_t *)&scroll[0], 2);
            OLED_DBG("\r\n");
        }
        while(ret != EWBMOK && (scroll[0] < 0x31 || scroll[0] > 0x32));
        if(scroll[0] == 0x31) {
            do {
                OLED_DBG("==================================================\r\n");
                OLED_DBG("Please input scroll time interval (min 1ms, max 99999ms) : ");
                ret = get_string(debugHandle, (uint8_t *)&interval[0], 6);
                OLED_DBG("\r\n");
            }
            while(ret != EWBMOK && atoi((char*)interval) != 0);
            delay = atoi((char*)interval);
        }
    }

    // Draw strings according to scroll mode
    if(atoi((char*)interval)) {
        height = (strlen((char*)input)-((OLED_WIDTH-startX)/(fontSize/2))-(temp))/(OLED_WIDTH/(fontSize/2));
        height *= fontSize;
        height += fontSize-((OLED_HEIGHT-startY)%fontSize);

        for(i = 0; i <= height; i++) {
            OLED_ClearScreen(0);
            OLED_DisplayString(startX, startY, input, (eOLED_FONT)fontSize, (eOLED_COLOR)(OLED_COLOR_BLACK+color[0]-0x31), OLED_ALIGN_OFF);
            OLED_Invalidate();
#ifdef __FREE_RTOS__
            vTaskDelay(delay);
#else
            __io_delay(delay);
#endif
            startY--;
        }
    } else {
        OLED_ClearScreen(0);
        OLED_DisplayString(startX, startY, input, (eOLED_FONT)fontSize, (eOLED_COLOR)(OLED_COLOR_BLACK+color[0]-0x31), OLED_ALIGN_ON);
        OLED_Invalidate();
    }
}
#endif


int main(void)
{
#if USE_UART_DEBUG
    char mode[2];
    int ret;

    // Set UART1 handle
    debugHandle.bank = EWBM_DEBUG_UART_PORT;
    debugHandle.baudRate = UART_BAUDRATE_115200;
    debugHandle.wordLength = UART_WORDLENGTH_8BIT;

    // Initialize UART1 handle
    UART_Init(&debugHandle);

    // Enable fifo UART1
    UART_EnableFifo(&debugHandle, UART_FIFO_LEVEL_7_OVER_8, UART_FIFO_LEVEL_1_OVER_8);

    // Enable UART1 to TX mode
    UART_EnableUart(&debugHandle, UART_MODE_TX_RX);

    // Initialize debug with the UART1 handle
    UART_PrintInit(&debugHandle);
#else
    char oledTextBuf[15] = "";
    int cntNum = 0;
#endif

    OLED_DBG("==================================================\r\n");

    // Initialize OLED
    OLED_Init();

    OLED_ClearScreen(0);
    OLED_DisplayString(OLED_FONT_12*2, OLED_FONT_12*2, (const unsigned char *)"OLED_Init() OK!", (eOLED_FONT)OLED_FONT_12, (eOLED_COLOR)OLED_COLOR_BLUE, OLED_ALIGN_ON);
    OLED_Invalidate();
    __m_delay(1000);

#if USE_UART_DEBUG
    while(1){
        do {
            OLED_DBG("================ OLED Test ================\r\n");
            OLED_DBG("Please for test 1. Print String\r\n");
            OLED_DBG("Please select test : ");
            ret = get_string(&debugHandle, (uint8_t *)&mode[0], 2);
            OLED_DBG("\r\n");
        }
        while(ret != EWBMOK && (mode[0] < 0x31 || mode[0] > 0x32));

        if(mode[0] == 0x31) {
            string_test(&debugHandle);
        }
    }
#else
    while(1){
        cntNum++;
        OLED_ClearScreen(0);
        sprintf(oledTextBuf, "FONT12/BLACK:%d", cntNum);
        OLED_DisplayString(0, 0, (const unsigned char *)oledTextBuf, (eOLED_FONT)OLED_FONT_12, (eOLED_COLOR)OLED_COLOR_BLACK, OLED_ALIGN_ON);

        sprintf(oledTextBuf, "FONT12/BLUE:%d", cntNum);
        OLED_DisplayString(0, OLED_FONT_12, (const unsigned char *)oledTextBuf, (eOLED_FONT)OLED_FONT_12, (eOLED_COLOR)OLED_COLOR_BLUE, OLED_ALIGN_ON);

        sprintf(oledTextBuf, "FONT16/BLACK:%d", cntNum);
        OLED_DisplayString(0, OLED_FONT_12+OLED_FONT_16, (const unsigned char *)oledTextBuf, (eOLED_FONT)OLED_FONT_16, (eOLED_COLOR)OLED_COLOR_BLACK, OLED_ALIGN_ON);

        sprintf(oledTextBuf, "FONT16/BLUE:%d", cntNum);
        OLED_DisplayString(0, OLED_FONT_12+(OLED_FONT_16*2), (const unsigned char *)oledTextBuf, (eOLED_FONT)OLED_FONT_16, (eOLED_COLOR)OLED_COLOR_BLUE, OLED_ALIGN_ON);
        OLED_Invalidate();

        __m_delay(1000);
    }
#endif

    return 0;
}

