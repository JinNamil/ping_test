/**
  ******************************************************************************
  * @file    ewbm_delay.h
  * @author  eWBM
  * @version v0.1
  * @date    06-06-2016
  * @brief   Header file of delay
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

#ifndef __DELAY_H__
#define __DELAY_H__

#include "ewbm_device.h"
/** @addtogroup EWBM_Peripheral
 * @{
 */

/** @addtogroup EWBM_DELAY
 * @{
 */

/** @addtogroup DELAY_External_Func
  * @{
  */

void __io_delay(uint32_t delay);
void __m_delay(uint32_t ms);

#endif //__DELAY_H__
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
