/**
  ******************************************************************************
  * @file    ewbm_gpt.h
  * @author  eWBM
  * @version v0.1
  * @date    16-06-2016
  * @brief   Header file of GPT module.
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

#ifndef __EWBM_TIMER_H__
#define __EWBM_TIMER_H__

#include "ewbm_device.h"


/** @addtogroup EWBM_Peripheral
  * @{
  */

/** @addtogroup EWBM_GPT
  * @{
  */

/** @defgroup GPT_External_Types GPT Exported Types
  * @{
  */

/**
  * @brief  GPT callback function type
  */

//GPT callback function type
typedef void (*GPTCB)(void *);

/**
  * @brief  GPT bank enumeration
  */
typedef enum {
    GPT_BANK0 = 0,
    GPT_BANK1
} eGPT_BANK;

/**
  * @brief  GPT channel(timer1/timer2) configuration structure
  */
typedef struct _tGPT_CHANNEL_INIT {
    uint32_t mode;
    uint32_t triger_mode;
    uint32_t triger_polarity;
    uint32_t triger_select;
    uint32_t match_output;
    uint32_t output_mode;
    uint32_t match_value;
    uint32_t mask_value;
} tGPT_CHANNEL_INIT;

/**
  * @brief  GPT base configuration structure
  */
typedef struct _tGPT_BASE_INIT {
    uint32_t count_reset;
    uint32_t master_channel;
    uint32_t link_mode;
    uint32_t prescaler;
} tGPT_BASE_INIT;

/**
  * @brief  GPT mode selection enumeration
  */
typedef enum {
    PERIODIC_MODE    = (0),    //default
    ONESHOT_MODE     = (1<<0), //stop on match
    FREERUNNING_MODE = (1<<1), //free-running
    GPT_MODE_MAX   = PERIODIC_MODE
} eGPT_MODE;

/**
  * @brief  GPT trigger polarity selection enumeration
  */
typedef enum {
    NOT_USED            = (0),    //default
    CAPTURE             = (1<<2), //edge-triggered
    TIMER_CLOCK         = (2<<2), //edge-triggered
    TIMER_ENABLE        = (3<<2), //level-sensitive
    GPT_TRIG_MODE_MAX = NOT_USED
} eGPT_TRIG_MODE;

/**
  * @brief  GPT trigger polarity mode selection enumeration
  */
typedef enum {
    POL_LOW  = (0),    //Falling-edge
    POL_HIGH = (1<<4), //Rising-edge
    POL_MAX  = POL_HIGH
} eGPT_TRIG_POL;

/**
  * @brief  GPT trigger selection enumeration
  */
typedef enum {
    EXTERNAL = (0),    //default
    INTERNAL = (1<<5), //internal
    SEL_MAX  = EXTERNAL
} eGPT_TRIG_SEL;

/**
  * @brief  GPT output mode selection enumeration
  */
typedef enum {
    DO_NOTHING         = (0),    //default
    OUT_MODE_HIGH      = (1<<8), //high mode
    OUT_MODE_LOW       = (2<<8), //low mode
    OUT_MODE_TOGGLE    = (3<<8), //toggle mode
    GPT_OUT_MODE_MAX = DO_NOTHING
} eGPT_OUT_MODE;

/**
  * @brief  GPT timer type selection enumeration
  */
typedef enum {
    GP_TMR1_TYPE = (0), //GPTIMER 1
    GP_TMR2_TYPE,       //GPTIMER 2
    GPT_TYPE_MAX
} eGPT_TYPE_SEL;

/**
  * @brief  GPT link mode selection enumeration
  */
typedef enum {
    DISABLED_SEPARATE             = (0),    //default
    CASCADED                      = (1<<2), //cascaded ?
    COMBINED                      = (2<<2), //combined ?
    GPT_BASE_CTRL_LINK_MODE_MAX = DISABLED_SEPARATE
} eGPT_BASE_CTRL_LINK_MODE;

/**
  * @brief  GPT prescaler selection enumeration
  */
typedef enum {
    TIMER_PRES_0   = (0),          //divided by 1(not divided)
    TIMER_PRES_1   = (1<<4),       //divided by 2
    TIMER_PRES_2   = (2<<4),       //divided by 4
    TIMER_PRES_3   = (3<<4),       //divided by 8
    TIMER_PRES_4   = (4<<4),       //divided by 16
    TIMER_PRES_5   = (5<<4),       //divided by 32
    TIMER_PRES_6   = (6<<4),       //divided by 64
    TIMER_PRES_7   = (7<<4),       //divided by 128
    TIMER_PRES_8   = (8<<4),       //divided by 256
    TIMER_PRES_9   = (9<<4),       //divided by 512
    TIMER_PRES_10  = (10<<4),      //divided by 1024
    TIMER_PRES_MAX = TIMER_PRES_10 //divided by 1024
} eGPT_BASE_PRES;

/**
  * @brief  GPT handle structure definition
  */
typedef struct _tGPT_HANDLE {
    eGPT_BANK     bank;
    eGPT_TYPE_SEL type;
    tGPT_REG*     reg;
} tGPT_HANDLE;

/**
  * @}
  */

/** @addtogroup GPT_External_Func
  * @{
  */

/** @addtogroup GPT_External_Func_G1
  * @{
  */

//init&de-init functions
int GPT_Init(tGPT_HANDLE* handle);
int GPT_DeInit(tGPT_HANDLE* handle);

/**
  * @}
  */

/** @addtogroup GPT_External_Func_G2
  * @{
  */
//operation functions

int GPT_TimeBaseSetConfig(tGPT_HANDLE* handle, tGPT_BASE_INIT* base, tGPT_CHANNEL_INIT* ch);
int GPT_TimeBaseStart(tGPT_HANDLE* handle, GPTCB intrCb, void* intrCbParam);
int GPT_TimeBaseStop(tGPT_HANDLE* handle);
//int    GPT_InterruptEnable(tTIMER_HANDLE* handle);
//uint32_t      GPT_InterruptStatus(tTIMER_HANDLE* handle, uint32_t auto_clear);
//eTIMER_OUT_MODE GPT_SelectMode(tGPT_HANDLE* handle);
//int    GPT_UpConterMode(tTIMER_HANDLE* handle);
//int    GPT_DownConterMode(tTIMER_HANDLE* handle);
//int    GPT_ToggleConterMode(tTIMER_HANDLE* handle);

uint16_t GPT_GetPartNum(tGPT_HANDLE* handle);
uint16_t GPT_GetConfiguration(tGPT_HANDLE* handle);
uint16_t GPT_GetMajRevision(tGPT_HANDLE* handle);
uint16_t GPT_GetMinRevision(tGPT_HANDLE* handle);
uint16_t GPT_GetReservedNum(tGPT_HANDLE* handle);


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
#endif // __EWBM_TIMER_H__
