/**
  ******************************************************************************
  * @file    ewbm_gpio.c
  * @author  eWBM
  * @version v0.1
  * @date    16-06-2016
  * @brief   GPIO module driver
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

#include "ewbm_gpio.h"
#include "ewbm_isr.h"

#ifdef GPIO_DBG_ENABLE
    #include "ewbm_uart.h"
    #define GPIO_DBG(fmt,args...)    UART_Printf(fmt,##args)
#else
    #define GPIO_DBG(fmt,args...)
#endif  // GPIO_DBG_ENABLE

/** @addtogroup EWBM_Peripheral
  * @{
  */

/** @addtogroup EWBM_GPIO
  * @{
  */

/** @defgroup GPIO_Internal_Types GPIO Internal Types
  * @{
  */

//__attribute__((unused)) static uint32_t gn_gpio_lock = MUTEX_UNLOCKED;

/**
  * @brief  ADVT interrupt type enumeration
  */
typedef enum _eGPIO_INTRTYPE {
    GPIO_INTERRUPT_PIN_0 = 0,
    GPIO_INTERRUPT_PIN_1,
    GPIO_INTERRUPT_PIN_2,
    GPIO_INTERRUPT_PIN_3,
    GPIO_INTERRUPT_PIN_4,
    GPIO_INTERRUPT_PIN_5,
    GPIO_INTERRUPT_PIN_6,
    GPIO_INTERRUPT_PIN_7,
    GPIO_INTERRUPT_PIN_8,
    GPIO_INTERRUPT_PIN_9,
    GPIO_INTERRUPT_PIN_10,
    GPIO_INTERRUPT_PIN_11,
    GPIO_INTERRUPT_PIN_12,
    GPIO_INTERRUPT_PIN_13,
    GPIO_INTERRUPT_PIN_14,
    GPIO_INTERRUPT_PIN_15,
    GPIO_INTERRUPT_MAX
} eGPIO_INTRTYPE;

/**
  * @brief  tGPIO_INTRCB structure that use to register interrupt
  */
typedef struct _tGPIO_INTRCB {
    GPIOCB  cb;
    void*   param;
} tGPIO_INTRCB;

/**
  * @brief  Internal global arrangement variables that have a structure of tGPIO_INTRCB
  */
static tGPIO_INTRCB gGPIOIntrArray[GPIO_BANK_MAX][GPIO_INTERRUPT_MAX];

/**
  * @}
  */


/** @addtogroup GPIO_Static_Func
  * @{
  */

/** @addtogroup GPIO_Static_Func_G1
  * @{
  */

/**
  * @brief  Get base address of GPIO
  * @param  bank: bank number of GPIO (input-> 0:GPIO1 ~ 4:GPIO5)
  * @retval tGPIO_REG* pointer to base address
  */
static tGPIO_REG* gpio_get_baseaddr(uint8_t bank)
{
    switch ( bank )
    {
        case GPIO_BANK_1:
                return (tGPIO_REG*)GPIO1;

        case GPIO_BANK_2:
                return (tGPIO_REG*)GPIO2;

        case GPIO_BANK_3:
                return (tGPIO_REG*)GPIO3;

        default :
            break;
    }

    return NULL;
}

/**
  * @brief  GPIO interrupt callback. call the registered callback function according to interrupt type
  * @param  priv: pointer to parameter of interrupt
  * @retval none
  */
static void gpio_intr_cb(void* priv)
{
    tGPIO_HANDLE* handle = (tGPIO_HANDLE*)priv;
    uint32_t status = 0;
    uint32_t i = 0;

    if ( handle == NULL || handle->reg == NULL )
        return;

    // Get interrupt status
    status = handle->reg->int_stsclr;

    for ( i=0; i<GPIO_INTERRUPT_MAX; i++ )
    {
        if ( status&(1<<i) )
        {
            if ( gGPIOIntrArray[handle->bank][i].cb )
            {
                gGPIOIntrArray[handle->bank][i].cb(gGPIOIntrArray[handle->bank][i].param);
            }
        }
    }
}

/**
  * @}
  */

/**
  * @}
  */


/** @addtogroup GPIO_External_Func
  * @{
  */

/** @addtogroup GPIO_External_Func_G1
  *  @brief    Initialization and deInitialization functions
  * @{
  */

/**
  * @brief  Initializes the GPIO peripheral
  * @param  handle: pointer to a tGPIO_HANDLE structure that contains the configuration information for GPIO module
  * @retval ewbm_error if success, return EWBMOK value
  */
int GPIO_Init(tGPIO_HANDLE* handle)
{
    if ( handle == NULL )
        return EWBMINVARG;

    handle->reg = gpio_get_baseaddr(handle->bank);
    if ( handle->reg == NULL )
        return EWBMINVARG;

    // Clear GPIO isr
    external_isr_clear((IRQn_Type)(GPIO1_IRQn+handle->bank));

    return EWBMOK;
}

/**
  * @brief  DeInitializes the GPIO peripheral
  * @param  handle: pointer to a tGPIO_HANDLE structure that contains the configuration information for GPIO module
  * @retval if success, return EWBMOK value
  */
int GPIO_DeInit(tGPIO_HANDLE* handle)
{
    if ( handle == NULL || handle->reg == NULL )
        return EWBMINVARG;

    // Register isr clear
    external_isr_clear((IRQn_Type)(GPIO1_IRQn+handle->bank));

    handle->reg = NULL;

    return EWBMOK;
}

/**
  * @}
  */

/** @addtogroup GPIO_External_Func_G2
  *  @brief    Operation functions
  * @{
  */

/**
  * @brief  Read the port mode from PORT_MODE register of GPIO peripheral
  * @param  handle: pointer to a tGPIO_HANDLE structure that contains the configuration information for GPIO module
  * @retval uint16_t value of PORT_MODE register
  */
uint16_t GPIO_ReadMode(tGPIO_HANDLE* handle)
{
    uint32_t portMode = 0;

    if ( handle == NULL || handle->reg == NULL )
        return ((uint16_t)EWBMINVARG);

    // Port Mode Read from GPIO PORT_MODE register
    portMode = handle->reg->port_mode;

    return (uint16_t)(portMode&0xFFFF);
}

/**
  * @brief  Direction the GPIO pin
  * @param  handle: pointer to a tGPIO_HANDLE structure that contains the configuration information for GPIO module
  * @param  pin:    Specifies the pins
  * @param  type:   Output type for port x
  * @param  enable: Output enable or disable
  * @retval if success, return EWBMOK value
  */
int GPIO_SetDir(tGPIO_HANDLE* handle, uint32_t pin, uint32_t type, uint8_t enable)
{
    uint32_t val = 0;

    if ( handle == NULL || handle->reg == NULL )
        return EWBMINVARG;

    // Port Mode Read from GPIO PORT_MODE register
    val = handle->reg->port_mode;
    if (((val&pin) != pin) )
    {
        val = handle->reg->out_mode;
        switch ( enable )
        {
            case SET:
                {
                    if ( type )
                        val |= pin<<16;
                    val |= pin;
                    handle->reg->out_mode = val;
                }
                break;

            case RESET:
                {
                    val &= ~(pin<<16);
                    val &= ~pin;
                    handle->reg->out_mode = val;
                }
                break;

            default :
                break;
        }

        return EWBMOK;
    }

    return EWBMERR;
}

/**
  * @brief  Read the status of GPIO pins
  * @param  handle: pointer to a tGPIO_HANDLE structure that contains the configuration information for GPIO module
  * @param  pin:  specifies the pins
  * @retval uint8_t status of pin
  */
uint8_t GPIO_ReadPin(tGPIO_HANDLE* handle, uint32_t pin)
{
    uint32_t val = 0;
    uint8_t status = 0;

    if ( handle == NULL || handle->reg == NULL )
        return ((uint8_t)EWBMINVARG);

    val = handle->reg->data;

    if ((val&pin) != pin )
    {
        status = RESET;
    }
    else
    {
        status = SET;
    }

    return status;
}

/**
  * @brief  Write the specified GPIO pins. sets or clears the selected data port bit
  * @param  handle: pointer to a tGPIO_HANDLE structure that contains the configuration information for GPIO module
  * @param  pin:    specifies the pins
  * @param  enable: output enable or disable
  * @retval if success, return EWBMOK value
  */
int GPIO_WritePin(tGPIO_HANDLE* handle, uint32_t pin, uint8_t enable)
{
    uint32_t val = 0;

    if ( handle == NULL || handle->reg == NULL )
        return EWBMINVARG;

    handle->reg->set_reset = val;
    val = handle->reg->data_out;
    switch ( enable )
    {
        case SET:
            val |= pin;
            handle->reg->data_out = val;
            break;

        case RESET:
            val &= ~pin;
            handle->reg->data_out = val;
            break;

        default :
            break;
    }

    return EWBMOK;
}

/**
  * @brief  Read the GPIO port data
  * @param  handle: pointer to a tGPIO_HANDLE structure that contains the configuration information for GPIO module
  * @retval uint16_t value of GPIO_DATA register
  */
uint16_t GPIO_ReadData(tGPIO_HANDLE* handle)
{
    uint32_t data = 0;

    if ( handle == NULL || handle->reg == NULL )
        return ((uint16_t)EWBMINVARG);

    // Port Data Read from GPIO GPIO_DATA register
    data = handle->reg->data;
    return (uint16_t)(data&0xFFFF);
}

/**
  * @brief  Toggles the specified GPIO pins
  * @param  handle: pointer to a tGPIO_HANDLE structure that contains the configuration information for GPIO module
  * @param  pin:  specifies the pins to be toggled
  * @retval if success, return EWBMOK value
  */
int GPIO_TogglePin(tGPIO_HANDLE* handle, uint32_t pin)
{
    uint32_t val = 0;

    if ( handle == NULL || handle->reg == NULL )
        return EWBMINVARG;

    handle->reg->set_reset = val;
    val = handle->reg->data_out;
    if ( (val&pin) == pin )
    {
        val ^= pin;
        handle->reg->data_out = val;
    }
    else
    {
        val |= pin;
        handle->reg->data_out = val;
    }

    return EWBMOK;
}

/**
  * @brief  Read the raw status register of GPIO interrupt
  * @param  handle: pointer to a tGPIO_HANDLE structure that contains
  *                 the configuration information for GPIO
  * @retval uint16_t value of GPIO_RAW_STS register
  */
uint16_t GPIO_InterruptReadRawStatus(tGPIO_HANDLE* handle)
{
    uint32_t status = 0;

    if ( handle == NULL || handle->reg == NULL )
        return ((uint16_t)EWBMINVARG);

    // GPIO Interrupt Status
    status = handle->reg->raw_sts;
    return (uint16_t)(status&0xFFFF);
}

/**
  * @brief  Read the enable register of GPIO interrupt
  * @param  handle: pointer to a tGPIO_HANDLE structure that contains the configuration information for GPIO module
  * @retval uint32_t value of GPIO_INT_EN register
  */
uint32_t GPIO_InterruptReadEnable(tGPIO_HANDLE* handle)
{
    uint32_t status = 0;

    if ( handle == NULL || handle->reg == NULL )
        return ((uint32_t)EWBMINVARG);

    // GPIO Interrupt Enable Status
    status = handle->reg->int_en;
    return status;
}

/**
  * @brief  Set the enable register of GPIO interrupt
  * @param  handle: pointer to a tGPIO_HANDLE structure that contains the configuration information for GPIO module
  * @param  intrPin:      specifies the pins for interrupt
  * @param  intrCombMode: interrupt combining mode (0: OR-ed, 1: AND-ed)
  * @retval if success, return EWBMOK value
  */
int GPIO_InterruptSetEnable(tGPIO_HANDLE* handle, uint32_t intrPin, uint32_t intrCombMode)
{
    uint32_t status = 0;

    if ( handle == NULL || handle->reg == NULL )
        return EWBMINVARG;

    // Set GPIO Interrupt Enable
    status = intrCombMode<<16;
    status |= intrPin;
    handle->reg->int_en |= status;
    return EWBMOK;
}

/**
  * @brief  Read the status register of GPIO interrupt
  * @param  handle: pointer to a tGPIO_HANDLE structure that contains the configuration information for GPIO module
  * @retval uint16_t value of GPIO_INT_STS register
  */
uint16_t GPIO_InterruptReadStatus(tGPIO_HANDLE* handle)
{
    uint32_t status = 0;

    if ( handle == NULL || handle->reg == NULL )
        return ((uint16_t)EWBMINVARG);

    // GPIO Interrupt Status
    status = handle->reg->int_stsclr;
    return (uint16_t)(status&0xFFFF);
}

/**
  * @brief  Clear the clear register of GPIO interrupt
  * @param  handle: pointer to a tGPIO_HANDLE structure that contains
  *                 the configuration information for GPIO
  * @param  intrPin: specifies the pins for interrupt
  * @retval if success, return EWBMOK value
  */
int GPIO_InterruptClear(tGPIO_HANDLE* handle, uint16_t intrPin)
{
    if ( handle == NULL || handle->reg == NULL )
        return EWBMINVARG;

    // GPIO Interrupt Clear
    handle->reg->int_stsclr = intrPin;
    return EWBMOK;
}

/**
  * @brief  Start the GPIO interrupt
  * @param  handle:        pointer to a tGPIO_HANDLE structure that contains the configuration information for GPIO module
  * @param  intrType:      the type to interrupt (0:Edge-trigged, 1:Level sensitive)
  * @param  intrPol:       interrupt polarity for port x, input specifies the pins for enable(Rising-edge or High)
  *                        for edge-triggered mode (0:Falling-edge, 1:Rising-edge), for level sensitive mode (0: Low, 1:High)
  * @param  intrBothEdge : both edge enable for port x, input specifies the pins for enable (if GPIO_INT_TYPE is zero)
  * @param  intrPin:       specifies the pins for interrupt
  * @param  intrCombMode : interrupt combining mode (0: OR-ed, 1: AND-ed)
  * @param  intrCb:        callback
  * @param  intrCbParam:   callback parameter
  * @retval if success, return EWBMOK value
  */
int GPIO_InterruptStart(tGPIO_HANDLE* handle, uint16_t intrType, uint16_t intrPol, uint32_t intrBothEdge, uint16_t intrPin, uint32_t intrCombMode, GPIOCB intrCb, void* intrCbParam)
{
    uint32_t i = 0;
    uint32_t intrEn = 0;
    uint32_t intrBEdge = 0;
    uint32_t val = 0;
    uint32_t result = 0;

    if ( handle == NULL || handle->reg == NULL )
        return EWBMINVARG;

    // Set GPIO interrupt clear
    handle->reg->int_stsclr = intrPin;

    // Set GPIO interrupt type
    handle->reg->int_type |= intrType;

    // Set GPIO interrupt polarity
    intrBEdge = intrBothEdge << 16;
    intrBEdge |= intrPol;
    handle->reg->int_pol |= intrBEdge;

    // Set GPIO interrupt enable
    intrEn = intrCombMode<<16;
    intrEn |= intrPin;
    val = handle->reg->int_en&0xFFFF;
    handle->reg->int_en = val|intrEn;

    // Read GPIO output mode settings
    result = handle->reg->out_mode;
    result &= ~intrPin;
    // Set GPIO output mode settings
    handle->reg->out_mode = result;

    if ( intrCb )
    {
        for ( i=0; i<GPIO_INTERRUPT_MAX; i++ )
        {
            if ( intrPin&(1<<i) )
            {
                // Register callback func to array of gpio interrupt
                gGPIOIntrArray[handle->bank][i].cb = intrCb;
                gGPIOIntrArray[handle->bank][i].param = intrCbParam;
            }
        }

        // Register isr handle
        external_isr_set((IRQn_Type)(GPIO1_IRQn+handle->bank), gpio_intr_cb, (void*)handle);
    }

    return EWBMOK;
}

/**
  * @brief  Stop the GPIO interrupt
  * @param  handle: pointer to a tGPIO_HANDLE structure that contains the configuration information for GPIO module
  * @param  intrPin:  specifies the pins for interrupt
  * @retval if success, return EWBMOK value
  */
int GPIO_InterruptStop(tGPIO_HANDLE* handle, uint16_t intrPin)
{
    uint32_t i = 0;
    uint32_t intrBEdge = 0;

    if ( handle == NULL || handle->reg == NULL )
        return EWBMINVARG;

    // Set GPIO interrupt type
    handle->reg->int_type &= ~intrPin;

    // Set GPIO interrupt polarity
    intrBEdge = intrPin << 16;
    intrBEdge |= intrPin;
    handle->reg->int_pol &= ~intrBEdge;

    // GPIO interrupt disable
    handle->reg->int_en &= ~intrPin;

    for ( i=0; i<GPIO_INTERRUPT_MAX; i++ )
    {
        if ( intrPin&(1<<i) )
        {
            // Register callback function to array of GPIO interrupt
            gGPIOIntrArray[handle->bank][i].cb = 0;
            gGPIOIntrArray[handle->bank][i].param = 0;
        }
    }

    return EWBMOK;
}

/**
  * @brief  Simultaneously set and clear for different GPIO pins
  * @param  handle: pointer to a tGPIO_HANDLE structure that contains the configuration information for GPIO module
  * @param  resetPin:  specifies the pins to be reset
  * @param  setPin:  specifies the pins to be set
  * @retval if success, return EWBMOK value
  */
int GPIO_SetReset(tGPIO_HANDLE* handle, uint32_t resetPin, uint32_t setPin)
{
    uint32_t clear = (setPin<<16) | resetPin;

    if ( handle == NULL || handle->reg == NULL )
        return (EWBMINVARG);

    handle->reg->set_reset = clear;
    return (EWBMOK);
}

/**
 * @brief  Get part number of GPIO
 * @param  handle: pointer to a tGPIO_HANDLE structure that contains the configuration information for GPIO module
 * @retval  part number
 */
uint16_t GPIO_GetPartNum(tGPIO_HANDLE* handle)
{
    if ( handle == NULL )
        return ((uint16_t)EWBMINVARG);

    tGPIO_REG* baseaddr = gpio_get_baseaddr(handle->bank);

    return baseaddr->peri_id & GPIO_PERI_ID_PART_NUM;
}

/**
 * @brief  Get configuration number of GPIO
 * @param  handle: pointer to a tGPIO_HANDLE structure that contains the configuration information for GPIO module
 * @retval  configuration number
 */
uint16_t GPIO_GetConfiguration(tGPIO_HANDLE* handle)
{
    if ( handle == NULL )
        return ((uint16_t)EWBMINVARG);

    tGPIO_REG* baseaddr = gpio_get_baseaddr(handle->bank);

    return ((baseaddr->peri_id >> 12) & GPIO_PERI_ID_CONFIG_NUM);
}

/**
 * @brief  Get major revision of watchdog
 * @param  handle: pointer to a tGPIO_HANDLE structure that contains the configuration information for GPIO module
 * @retval  major revision
 */
uint16_t GPIO_GetMajRevision(tGPIO_HANDLE* handle)
{
    if ( handle == NULL )
        return ((uint16_t)EWBMINVARG);

    tGPIO_REG* baseaddr = gpio_get_baseaddr(handle->bank);

    return ((baseaddr->peri_id >> 16) & GPIO_PERI_ID_MAJ_REV);
}

/**
 * @brief  Get minor revision of watchdog
 * @param  handle: pointer to a tGPIO_HANDLE structure that contains the configuration information for GPIO module
 * @retval  minor revision
 */
uint16_t GPIO_GetMinRevision(tGPIO_HANDLE* handle)
{
    if ( handle == NULL )
        return ((uint16_t)EWBMINVARG);

    tGPIO_REG* baseaddr = gpio_get_baseaddr(handle->bank);

    return ((baseaddr->peri_id >> 20) & GPIO_PERI_ID_MIN_REV);
}

/**
 * @brief  Get reserved number of watchdog
 * @param  handle: pointer to a tGPIO_HANDLE structure that contains the configuration information for GPIO module
 * @retval  reserved number
 */
uint16_t GPIO_GetReservedNum(tGPIO_HANDLE* handle)
{
    if ( handle == NULL )
        return ((uint16_t)EWBMINVARG);

    tGPIO_REG* baseaddr = gpio_get_baseaddr(handle->bank);


    return ((baseaddr->peri_id >> 24) & GPIO_PERI_ID_RSVD_NUM);
}


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

