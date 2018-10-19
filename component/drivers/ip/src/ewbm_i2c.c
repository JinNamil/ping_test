/**
  ******************************************************************************
  * @file    ewbm_i2c.c
  * @author  eWBM
  * @version v0.1
  * @date    16-06-2016
  * @brief   I2C module driver
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

#include "ewbm_i2c.h"
#include "ewbm_isr.h"
#include "ewbm_delay.h"
#include "ewbm_clock.h"

#define I2C_DBG_ENABLE      0

#if I2C_DBG_ENABLE
    #include "ewbm_uart.h"
    static tUART_HANDLE debugHandle;
    #define I2C_DBG(fmt,args...)    UART_Printf(fmt,##args)
#else
    #define I2C_DBG(fmt,args...)    __io_delay(1);
#endif // I2C_DBG_ENABLE

/** @addtogroup EWBM_Peripheral
  * @{
  */

/** @addtogroup EWBM_I2C
  * @{
  */

/** @defgroup I2C_Internal_Types I2C Intenal Types
  * @{
  */

/**
  * @brief  I2C time out define
  */
#define I2C_TIMEOUT            (0x100)

/**
  * @brief  I2C clock frequency define
  */
#define I2C_CLOCK_FREQUENCY    (48000000)

/**
  * @brief  I2C clock reset bit defines
  */
#define I2C_CLK_RESET(x)       (1<<(20+x))


/**
  * @brief  I2C transfer data max defines in slave mode
  */
#define I2C_SMDATA_MAX           (64)

static uint8_t gSMData[I2C_SMDATA_MAX];  // using data buffer in slave mode
static uint8_t gSMDataCnt=0;             // using data buffer count in slave mode

/**
  * @}
  */

/** @addtogroup I2C_Static_Func
  * @{
  */

/** @addtogroup I2C_Static_Func_G1
  * @{
  */

/**
  * @brief  Get base address about channel of i2c
  * @param  ch: channel number of i2c to use
  * @retval pointer to base address
  */
static tI2C_REG* i2c_get_baseaddr(eI2C_CHANNEL ch)
{
    switch ( ch )
    {
        case 0 :
            return I2C1;

        case 1 :
            return I2C2;

        case 2 :
            return I2C3;

        case 3 :
            return I2C4;

        default :
            break;
    }

    return NULL;
}

/**
  * @brief  Get prescale about i2c frequency
  * @param  freq: i2c frequency
  * @retval prescale value
  */
static uint16_t i2c_get_prescale(eI2C_CLKFREQ freq)
{
    uint16_t pres;
    uint32_t scl;

    /*
    prescale = sys_clk/(5*desired SCL) - 1;
    desired SCL = sys_clk/(5+5*prescale);

    Ex> sys_clk=48MHz, desired SCL = 100KHz
    prescale = 48MHz/(5*100KHz) - 1 = 95 = 0x5f
    */

    scl = (freq+1)*100000;
    pres = (I2C_CLOCK_FREQUENCY/(5*scl)-1);

    return pres;
}

/**
  * @brief  Check device status. if status is not same, wait.
  * @param  reg: pointer to device register
  * @param  stat: device status
  * @retval none
  */
static void i2c_check_devbusy(volatile uint32_t *reg, uint32_t stat)
{
    while((*reg)&stat);    
}

/**
  * @brief  Send a slave address. slave address has a 7bit
  * @param  i2cReg: pointer to a tI2C_REG structure that register set for i2c module
  * @param  slaveAddr: slave address
  * @param  flag: read or write
  * @retval none
  */
static void i2c_send_slaveaddr(tI2C_REG* i2cReg, uint8_t slaveAddr, eI2C_TRANSFERFLAG flag)
{
    // write slave address
    i2cReg->txrx = ((slaveAddr<<1)|flag);
    i2cReg->cmdstate = I2C_CMD_START | I2C_CMD_WRITE;
    i2c_check_devbusy(&i2cReg->cmdstate, I2C_STATE_TIP);
    i2c_check_devbusy(&i2cReg->cmdstate, I2C_STATE_RXACK);    // for humidity_temperature_sensor
}

/**
  * @brief  Transfer i2c message to a slave device
  * @param  i2cReg: pointer to the tI2C_REG structure that register set for i2c module
  * @param  i2cMsg: pointer to a tI2C_MSG structure that contains a slave address, a data buffer pointer and a data count
  * @param  flag: read or write
  * @retval if success, return EWBMOK value
  */
static int i2c_transfer_msg(tI2C_REG* i2cReg, tI2C_MSG* i2cMsg, eI2C_TRANSFERFLAG flag)
{
    uint8_t  len = 0;
    uint8_t  cnt = 0;

    len = i2cMsg->len;
    I2C_DBG("transfer msg len- %d \r\n", len);

    // write device address
    while((len > 0) && (i2cMsg->buf != NULL)) {
        if(flag == I2C_TRANS_WRITE) {
            // write a data
            i2cReg->txrx = i2cMsg->buf[cnt];
            i2cReg->cmdstate = I2C_CMD_WRITE;
            i2c_check_devbusy(&i2cReg->cmdstate, I2C_STATE_TIP);
            i2c_check_devbusy(&i2cReg->cmdstate, I2C_STATE_RXACK);    // for humidity_temperature_sensor
            cnt++;
        } else {
            // read a data
            if(i2cMsg->len == (cnt+1)) {
                i2cReg->cmdstate = (I2C_CMD_READ|I2C_CMD_SENDNACK);
                i2c_check_devbusy(&i2cReg->cmdstate, I2C_STATE_TIP);
            }
            else {
                i2cReg->cmdstate = I2C_CMD_READ;
                i2c_check_devbusy(&i2cReg->cmdstate, I2C_STATE_TIP);
                i2c_check_devbusy(&i2cReg->cmdstate, I2C_STATE_RXACK);    // for humidity_temperature_sensor
            }
            i2cMsg->buf[cnt] = i2cReg->txrx;
            cnt++;
        }
        len--;
    }

    // i2c stop
    i2cReg->cmdstate = I2C_CMD_STOP;
    __io_delay(1);
    i2c_check_devbusy(&i2cReg->cmdstate, I2C_STATE_TIP|I2C_STATE_BUSBUSY);

    if(i2cMsg->len != cnt) {
        I2C_DBG("i2c transfer msg error!\n");
        return EWBMERR;
    }

    return EWBMOK;
}

/**
  * @brief  I2C interrupt callback. call this CB when data transfer completed.
               data that received from master saves to buffers
  * @param  priv: pointer to parameter of interrupt
  * @retval none
  */
static void i2c_slaverecv_cb(void *priv)
{
    tI2C_HANDLE* handle = (tI2C_HANDLE*)priv;

    if ( handle == NULL || handle->reg == NULL )
        return;

    uint8_t sts = handle->reg->int_stsclr;

    if ( sts&I2C_TRAN_INT_STS )
        gSMData[gSMDataCnt++]= handle->reg->txrx;

    handle->reg->int_stsclr |= sts;
}

/**
  * @brief  I2C interrupt callback. call this CB when data transfer completed.
               data that send to master writes to a i2c transmit register
  * @param  priv: pointer to parameter of interrupt
  * @retval none
  */
static void i2c_slavesnd_cb(void *priv)
{
    tI2C_HANDLE* handle = (tI2C_HANDLE*)priv;

    if ( handle == NULL || handle->reg == NULL )
        return;

    uint8_t sts = handle->reg->int_stsclr;

    if(sts&I2C_TRAN_INT_STS)
        handle->reg->txrx = gSMData[gSMDataCnt++];

    handle->reg->int_stsclr |= sts;
}

/**
  * @}
  */

/**
  * @}
  */


/** @addtogroup I2C_External_Func
  * @{
  */

/** @addtogroup I2C_External_Func_G1
  *  @brief    Initialization and Configuration functions
  * @{
  */

/**
  * @brief  Initializes the I2C according to the specified parameters
  * @param  handle: pointer to a tI2C_HANDLE structure that contains the configuration information for I2C module
  * @retval  if success, return EWBMOK value
  */
int I2C_Init(tI2C_HANDLE* handle)
{
    if ( handle == NULL )
        return EWBMINVARG;

    handle->reg = i2c_get_baseaddr(handle->channel);
    if ( handle->reg == NULL )
        return EWBMINVARG;
        
    // I2C clk reset
    CLKRST->peri_rst = I2C_CLK_RESET(handle->channel);
    while ( !(CLKRST->peri_rst & I2C_CLK_RESET(handle->channel)) );

#if 0 // master mode only
    pres = i2c_get_prescale(handle->freq);
    handle->reg->pres_lo = (pres&0xff);
    handle->reg->pres_hi = (pres&0xff00) >> 8;
#endif

#if I2C_DBG_ENABLE
    // Set UART1 handle
    debugHandle.bank = UART_BANK_1;
    debugHandle.baudRate = UART_BAUDRATE_115200;
    debugHandle.wordLength = UART_WORDLENGTH_8BIT;

    // Initialize UART1 handle
    UART_Init(&debugHandle);

    // Enable fifo UART
    UART_EnableFifo(&debugHandle, UART_FIFO_LEVEL_7_OVER_8, UART_FIFO_LEVEL_1_OVER_8);

    // Enable UART1 to TX mode
    UART_EnableUart(&debugHandle, UART_MODE_TX);

    // Initialize debug with the UART1 handle
    UART_PrintInit(&debugHandle);
#endif

    return EWBMOK;
}

/**
  * @brief  DeInitializes the I2C
  * @param  handle: pointer to a tI2C_HANDLE structure that contains the configuration information for I2C module
  * @retval  if success, return EWBMOK value
  */
int I2C_DeInit(tI2C_HANDLE* handle)
{
   if ( handle == NULL || handle->reg == NULL )
        return EWBMINVARG;

    // i2c disable
    handle->reg->ctrl = 0;

    // prescale reset value
    handle->reg->pres_lo = 0xff;
    handle->reg->pres_hi = 0xff;
    handle->reg = NULL;

    return EWBMOK;
}
/**
  * @}
  */

/** @addtogroup I2C_External_Func_G2
  *  @brief    Operation functions
  * @{
  */

/**
  * @brief  Enable the i2c core and select master core
  * @param  handle: pointer to a tI2C_HANDLE structure that contains the configuration information for I2C module
  * @retval  if success, return EWBMOK value
  */
int I2C_MasterModeEnable(tI2C_HANDLE* handle)
{
    uint16_t pres;

    if ( handle == NULL || handle->reg == NULL )
        return EWBMINVARG;

    // set i2c prescaler
    pres = i2c_get_prescale(handle->freq);
    handle->reg->pres_lo = (pres&0xff);
    handle->reg->pres_hi = (pres&0xff00) >> 8;

    handle->reg->ctrl |= I2C_CTRL_EN;
    return EWBMOK;
}

/**
  * @brief  Disable the i2c core and clear core select bit
  * @param  handle: pointer to a tI2C_HANDLE structure that contains the configuration information for I2C module
  * @retval  if success, return EWBMOK value
  */
int I2C_MasterModeDisable(tI2C_HANDLE* handle)
{
    if ( handle == NULL || handle->reg == NULL )
        return EWBMINVARG;

    handle->reg->ctrl &= ~I2C_CTRL_EN;

    // prescale reset value
    handle->reg->pres_lo = 0xff;
    handle->reg->pres_hi = 0xff;

    return EWBMOK;
}

/**
  * @brief  Enable the i2c core and select slave core
  * @param  handle: pointer to a tI2C_HANDLE structure that contains the configuration information for I2C module
  * @retval if success, return EWBMOK value
  */
int I2C_SlaveModeEnable(tI2C_HANDLE* handle)
{
    if ( handle == NULL || handle->reg == NULL )
        return EWBMINVARG;

    // set addr in slave mode
    handle->reg->dev_id = (I2C_SLAVE_ADDRESS<<1);

    // i2c core slave select and enable
    handle->reg->ctrl |= (I2C_CTRL_EN|I2C_CTRL_SLVMODE);

    return EWBMOK;
}

/**
  * @brief  Disable the i2c core and clear core select bit
  * @param  handle: pointer to a tI2C_HANDLE structure that contains the configuration information for I2C module
  * @retval  if success, return EWBMOK value
  */
int I2C_SlaveModeDisable(tI2C_HANDLE* handle)
{
    if ( handle == NULL || handle->reg == NULL )
        return EWBMINVARG;

    handle->reg->ctrl &= ~(I2C_CTRL_EN|I2C_CTRL_SLVMODE);
    return EWBMOK;
}

/**
  * @brief  Sends a data in master mode. The data sent as much as the data count of i2c message
  * @param  handle: pointer to a tI2C_HANDLE structure that contains the configuration information for I2C module
  * @param  msg: pointer to a tI2C_MSG structure that contains a slave address, a data buffer pointer and a data count
  * @retval  if success, return EWBMOK value
  */
int I2C_MasterSend(tI2C_HANDLE* handle, tI2C_MSG* msg)
{
    if ( handle == NULL || handle->reg == NULL )
        return EWBMINVARG;

    if ( (msg == NULL) || (msg->buf == NULL) )
        return EWBMINVARG;

    // send a slave address
    i2c_send_slaveaddr(handle->reg, msg->slaveAddr, I2C_TRANS_WRITE);

    // send a data
    i2c_transfer_msg(handle->reg, msg, I2C_TRANS_WRITE);

    return EWBMOK;
}

/**
  * @brief  Receives a data in master mode. The data received as much as the data count of i2c message
  * @param  handle: pointer to a tI2C_HANDLE structure that contains the configuration information for I2C module
  * @param  msg: pointer to a tI2C_MSG structure that contains a slave address, a data buffer pointer and a data count
  * @retval  if success, return EWBMOK value
  */
int I2C_MasterRecv(tI2C_HANDLE* handle, tI2C_MSG* msg)
{
    if ( handle == NULL || handle->reg == NULL )
        return EWBMINVARG;

    if ( (msg == NULL) || (msg->buf == NULL) )
        return EWBMINVARG;

    // send a slave address
    i2c_send_slaveaddr(handle->reg, msg->slaveAddr, I2C_TRANS_READ);

    // receives a data
    i2c_transfer_msg(handle->reg, msg, I2C_TRANS_READ);

    return EWBMOK;
}


/**
  * @brief  Sends a data(writes to a i2c transmit register) in slave mode. The data sent as much as the data count of i2c message
  * @param  handle: pointer to a tI2C_HANDLE structure that contains the configuration information for I2C module
  * @param  msg: pointer to a tI2C_MSG structure that contains a slave address, a data buffer pointer and a data count
  * @retval  if success, return EWBMOK value
  */
int I2C_SlaveSend(tI2C_HANDLE* handle, tI2C_MSG* msg)
{
    uint8_t  len = 0;
    uint8_t  cnt = 0;

    if ( handle == NULL || handle->reg == NULL )
        return EWBMINVARG;

    if ( (msg == NULL) || (msg->buf == NULL) )
        return EWBMINVARG;

    gSMDataCnt = 0;

    len = msg->len;

    while ( (len > 0) && (msg->buf != NULL) )
    {
        // write a data
        gSMData[cnt] = msg->buf[cnt];
        cnt++;
        len--;
    }

    handle->reg->int_en |= (I2C_TRAN_INT_EN);
    external_isr_set((IRQn_Type)(I2C1_IRQn+handle->channel), i2c_slavesnd_cb, (void*)handle);

    while ( gSMDataCnt < msg->len )
    {
        __io_delay(1);
    }

    handle->reg->int_en &= ~(I2C_TRAN_INT_EN);
    external_isr_clear((IRQn_Type)(I2C1_IRQn+handle->channel));

    return EWBMOK;
}


/**
  * @brief  Receives a data in slave mode. The data received as much as the data count of i2c message
  * @param  handle: pointer to a tI2C_HANDLE structure that contains the configuration information for I2C module
  * @param  msg: pointer to a tI2C_MSG structure that contains a slave address, a data buffer pointer and a data count
  * @retval  if success, return EWBMOK value
  */
int I2C_SlaveRecv(tI2C_HANDLE* handle, tI2C_MSG* msg)
{
    uint8_t  len = 0;
    uint8_t  cnt = 0;

    if ( handle == NULL || handle->reg == NULL )
        return EWBMINVARG;

    if ( (msg == NULL) || (msg->buf == NULL) )
        return EWBMINVARG;

    len = msg->len;

    gSMDataCnt = 0;
    handle->reg->int_en |= (I2C_TRAN_INT_EN);
    external_isr_set((IRQn_Type)(I2C1_IRQn+handle->channel), i2c_slaverecv_cb, (void*)handle);

    while ( len > 0 )
    {
        if ( gSMDataCnt == len )
        {
            while ( (len > 0) && (msg->buf != NULL) )
            {
                // read a data
                msg->buf[cnt] = gSMData[cnt];
                cnt++;
                len--;
            }
            gSMDataCnt = 0;
        }
        else
        {
            __io_delay(1);
        }
    }

    handle->reg->int_en &= ~(I2C_TRAN_INT_EN);
    external_isr_clear((IRQn_Type)(I2C1_IRQn+handle->channel));

    return EWBMOK;
}

/**
 * @brief  Get part number of I2C ID0
 * @param  handle: pointer to a tI2C_HANDLE structure that contains the configuration information for I2C module
 * @retval  part number
 */
uint16_t I2C_GetPartNumId0(tI2C_HANDLE* handle)
{
    if ( handle == NULL )
        return ((uint16_t)EWBMINVARG);

    tI2C_REG* baseaddr = i2c_get_baseaddr(handle->channel);

    return baseaddr->peri_id0;
}

/**
 * @brief  Get part number of I2C ID1
 * @param  handle: pointer to a tI2C_HANDLE structure that contains the configuration information for I2C module
 * @retval  part number
 */
uint16_t I2C_GetPartNumId1(tI2C_HANDLE* handle)
{
    if ( handle == NULL )
        return ((uint16_t)EWBMINVARG);

    tI2C_REG* baseaddr = i2c_get_baseaddr(handle->channel);

    return baseaddr->peri_id1 & I2C_PERI_ID1_PART_NUM_HI;
}

/**
 * @brief  Get configuration number of I2C
 * @param  handle: pointer to a tI2C_HANDLE structure that contains the configuration information for I2C module
 * @retval  part number
 */
uint16_t I2C_GetConfiguration(tI2C_HANDLE* handle)
{
    if ( handle == NULL )
        return ((uint16_t)EWBMINVARG);

    tI2C_REG* baseaddr = i2c_get_baseaddr(handle->channel);

    return ((baseaddr->peri_id1 >> 4) & I2C_PERI_ID1_CONFIG_NUM);
}

/**
 * @brief  Get major revision of I2C
 * @param  handle: pointer to a tI2C_HANDLE structure that contains the configuration information for I2C module
 * @retval  part number
 */
uint16_t I2C_GetMajRevision(tI2C_HANDLE* handle)
{
    if ( handle == NULL )
        return ((uint16_t)EWBMINVARG);

    tI2C_REG* baseaddr = i2c_get_baseaddr(handle->channel);

    return (baseaddr->peri_id2 & I2C_PERI_ID2_MAJ_REV);
}

/**
 * @brief  Get minor revision of I2C
 * @param  handle: pointer to a tI2C_HANDLE structure that contains the configuration information for I2C module
 * @retval  part number
 */
uint16_t I2C_GetMinRevision(tI2C_HANDLE* handle)
{
    if ( handle == NULL )
        return ((uint16_t)EWBMINVARG);

    tI2C_REG* baseaddr = i2c_get_baseaddr(handle->channel);

    return ((baseaddr->peri_id2 >> 4) & I2C_PERI_ID2_MIN_REV);
}

/**
 * @brief  Get Reserved number of I2C
 * @param  handle: pointer to a tI2C_HANDLE structure that contains the configuration information for I2C module
 * @retval  part number
 */
uint16_t I2C_GetReservedNum(tI2C_HANDLE* handle)
{
    if ( handle == NULL )
        return ((uint16_t)EWBMINVARG);

    tI2C_REG* baseaddr = i2c_get_baseaddr(handle->channel);

    return (baseaddr->peri_id3);
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

