/**
  ******************************************************************************
  * @file    ewbm_oled.h
  * @author  eWBM
  * @version V0.1
  * @date    29-07-2016
  * @brief   Header file of OLED module
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

#ifndef __EWBM_OLED_H__
#define __EWBM_OLED_H__

#include "ewbm_spi.h"

/** @addtogroup EWBM_Peripheral
  * @{
  */

/** @addtogroup EWBM_OLED
  * @{
  */

/** @defgroup OLED_External_Types OLED Exported Types
  * @{
  */

/**
  * @brief  OLED width definition
  */
#define OLED_WIDTH    128

/**
  * @brief  OLED height definition
  */
#define OLED_HEIGHT   64

/**
  * @brief  OLED font type enumeration
  */
typedef enum {
    OLED_FONT_12 = 12,
    OLED_FONT_16 = 16
} eOLED_FONT;

/**
  * @brief  OLED color type enumeration
  */
typedef enum {
    OLED_COLOR_BLACK = 0,
    OLED_COLOR_BLUE
} eOLED_COLOR;

/**
  * @brief  OLED align mode enumeration
  */
typedef enum {
    OLED_ALIGN_OFF = 0,
    OLED_ALIGN_ON
} eOLED_ALIGN;

/**
  * @}
  */

/** @addtogroup OLED_External_Func
  * @{
  */

/** @addtogroup OLED_External_Func_G1
  * @{
  */

// init/deinit functions
int OLED_Init(void);
int OLED_DeInit(void);

/**
  * @}
  */

/** @addtogroup OLED_External_Func_G2
  * @{
  */

void OLED_DisplayOn(void);
void OLED_DisplayOff(void);
void OLED_EntireDisplayOn(void);
void OLED_EntireDisplayOn(void);
void OLED_SetContrast(uint32_t contrast);
void OLED_ReverseColorOn(void);
void OLED_ReverseColorOff(void);
void OLED_FlipModeOn(void);
void OLED_FlipModeOff(void);
void OLED_Invalidate(void);
void OLED_Fill_Screen(uint8_t posX1, uint8_t posY1, uint8_t posX2, uint8_t posY2, eOLED_COLOR dot);
void OLED_ClearScreen(uint8_t fill);
void OLED_DrawPoint(uint8_t posX, int8_t posY, eOLED_COLOR point);
void OLED_DisplayChar(uint8_t posX, int8_t posY, uint8_t chr, eOLED_FONT font, eOLED_COLOR color);
void OLED_DisplayString(uint8_t posX, int8_t posY, const uint8_t* str, eOLED_FONT font, eOLED_COLOR color, eOLED_ALIGN align);
void OLED_DrawBitmap(uint8_t posX, uint8_t posY, const uint8_t* bmp, uint8_t width, uint8_t height);
void OLED_Draw1616Num(uint8_t posX, uint8_t posY, uint8_t chr, eOLED_COLOR color);
void OLED_Draw3216Num(uint8_t posX, uint8_t posY, uint8_t chr, eOLED_COLOR color);

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#endif /*__EWBM_OLED_H__*/
