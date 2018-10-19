//*****************************************************************************
//
//! \file wiz_init.c
//! \brief W5500 initialization.
//! \details SPI function access to register API and W5500 initialization
//! \version 1.0.0
//! \date 2016/04/14
//! \par  Revision history
//!         <2016/05/4>
//!         Moved the DHCP functions here
//! \author RedBeam
//
//*****************************************************************************


#include "wiz_init.h"
#include "wizchip_conf.h"
#include "socket.h"
#include "w5500.h"
#ifdef DHCP
#include "dhcp.h"
#endif

#include "ewbm_spi.h"
#include "ewbm_gpio.h"
#include "ewbm_i2c.h"
#include "ewbm_uart.h"


uint8_t DEFAULT_MAC[6] = {0x00,0x08,0xdc,0xAF,0xFE,0xFF};
uint8_t DEFAULT_SUB[4] = {255,255,255,0};
uint8_t DEFAULT_LIP[4] = {192,168,188,88};
uint8_t DEFAULT_GW[4] =  {192,168,188,1};
/**********************************************************************/
#define SLAVE_DATA_BUF_MAX (512+16)
extern tSPI_HANDLE spiHandle;
extern tGPIO_HANDLE gpioHandle;

static uint8_t gWizDataBuf[SLAVE_DATA_BUF_MAX];

extern wiz_NetInfo pnetinfo2;

/**********************************************************************/

#ifdef DHCP
/**
 * @brief Default write IP address, Gateway address and subnet mask to the wiznet.
 * @param  None
 */
void ip_assign(void)
{
    uint8_t ip[4];

    /* get IP address from DHCP-driver */
    getIPfromDHCP(ip);
    /* set ip in wiznet register */
    setSIPR(ip);

    /* get gateway address from DHCP-driver */
    getGWfromDHCP(ip);
    /* set gateway in wiznet register */
    setGAR(ip);

    /* get subnet mask from DHCP-driver */
    getSNfromDHCP(ip);
    /* set subnet mask in wiznet register */
    setSUBR(ip);
}

/**
 * @brief does a wiznet soft resets reassigns the mac, ip, gateway, subnet
 * @note The default handler of ip changed, the addresses will come from the DHCP driver
 * @param  None
 */
void ip_update(void)
{
    /* WIZchip Software Reset */
    setMR(MR_RST);
    /* small delay*/
    getMR();
    /* assign ip */
    ip_assign();

    /* set the mac address in wiznet chip*/
    setSHAR((uint8_t*)DEFAULT_MAC);

}

/**
 * @brief
 * @note The default handler of ip changed, the addresses will come from the DHCP driver
 * @param  None
 */
void ip_conflict(void)
{
    // WIZchip Software Reset
    setMR(MR_RST);
    getMR(); // for delay
    setSHAR((uint8_t*)DEFAULT_MAC);
}

#else /* DHCP */
#endif // DHCP
/****************************************************************************************/
/**
  * @brief  to be used by the REGISTER_API (w5500.c) for W5500 NSS access
  * @note use reg_wizchip_cris_cbfunc for function integration to W5500 HAL Interface
  * @param  None
  * @retval None
  */
void W5500_Select()
{
    int ret = -10;
    // Pin disable for port x
    ret = GPIO_WritePin(&gpioHandle, GPIO_PIN_1, RESET);
     if ( ret != EWBMOK ){
        UART_Printf("GPIO_WritePin Failed \r\n");
        return;
    }
}

/**
  * @brief  to be used by the REGISTER_API (w5500.c) for W5500 NSS access
  * @note use reg_wizchip_cris_cbfunc for function integration to W5500 HAL Interface
  * @param  None
  * @retval None
  */
void W5500_Deselect()
{
    int ret = -10;
    ret = GPIO_WritePin(&gpioHandle, GPIO_PIN_1, SET);
    if ( ret != EWBMOK ){
        UART_Printf("GPIO_WritePin Failed \r\n");
        return;
    }
}

/**
  * @brief  to be used by the REGISTER_API (w5500.c) for W5500 SPI write access
  * @note use reg_wizchip_spi_cbfunc for function integration to W5500 HAL Interface
  * @param  send_byte: the byte to be written to the W5500
  * @retval None
  */
void WriteSPIbyte(uint8_t send_byte)
{
    int ret = -10;
    ret = SPI_Write(&spiHandle, send_byte);
    if ( ret != EWBMOK ){
        UART_Printf("SPI_Write Failed \r\n");
        return;
    }
    SPI_Read(&spiHandle);
}

/**
  * @brief  to be used by the REGISTER_API (w5500.c) for W5500 SPI read access
  * @note use reg_wizchip_spi_cbfunc for function integration to W5500 HAL Interface
  * @param  None
  * @retval received_byte: the byte send by the W5500 to the MCU
  */
uint8_t ReadSPIbyte(void)
{
    int ret = -10;
    ret = SPI_Write(&spiHandle, 0xFF);
    if ( ret != EWBMOK ){
        UART_Printf("SPI_Write Failed \r\n");
        return -1;
    }
    return SPI_Read(&spiHandle);
}


/**
 * @brief
 * @note set default network
 * @param  None
 */
void set_default_network(void)
{
    setSHAR((uint8_t*)DEFAULT_MAC);
    setSUBR((uint8_t*)DEFAULT_SUB);
    setGAR((uint8_t*)DEFAULT_GW);
    setSIPR((uint8_t*)DEFAULT_LIP);

    setRTR(2000);//200ms
    setRCR(3);
}

//#endif //DHCP
/**
  * @brief initializes the W5500 functionality and the W5500 chip
  * @note
  * @param  None
  * @retval None
  */
void W5500_Init(void)
{
 /* socket mem distribution*/
    uint8_t memsize[2][8] = { { 2, 2, 2, 2, 2, 2, 2, 2 }, { 2, 2, 2, 2, 2, 2, 2, 2 } };
    wiz_PhyConf phyconf;

    /* pointer handover from SPI-API to W5500 core functions */
    reg_wizchip_spi_cbfunc(ReadSPIbyte, WriteSPIbyte);
    reg_wizchip_cs_cbfunc(W5500_Select, W5500_Deselect);

    /* pointer handover  IP assign -and IP update function to DHCP core functions */
#ifdef DHCP
    reg_dhcp_cbfunc(ip_assign, ip_update, ip_conflict);
#endif

    W5500_Deselect();

    /* w5500 physical layer configuration*/
    phyconf.by = PHY_CONFBY_HW;
    phyconf.mode = PHY_MODE_AUTONEGO;
    phyconf.speed = PHY_SPEED_100;
    phyconf.duplex = PHY_DUPLEX_FULL;

    wizphy_setphyconf(&phyconf);

    /* set mac adress */
    setSHAR((uint8_t*)DEFAULT_MAC);

    /* set data sockets to non blocking*/
#ifdef DHCP
    ctlsocket(SOCK_DHCP, CS_SET_IOMODE, (void*)SOCK_IO_NONBLOCK);
#endif

    if (ctlwizchip(CW_INIT_WIZCHIP, (void*) memsize) == -1)
    {
        //Program System Reset here
        UART_Printf("\r\n ctlwizchip (WIZ_init) ERROR.\r\n");
        //while (1);
    }
}

