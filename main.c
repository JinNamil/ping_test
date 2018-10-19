#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ewbm_global_config.h"

#ifdef __FREE_RTOS__
#include "FreeRTOS.h"
#include "task.h"
#endif

#include "ewbm_delay.h"
#include "ewbm_uart.h"

#include "w5500.h"
#include "socket.h"
#include "wiz_init.h"
#include "ping.h"

#include "ewbm_spi.h"
#include "ewbm_gpio.h"

 tSPI_HANDLE spiHandle;
 tGPIO_HANDLE gpioHandle;
 tUART_HANDLE uartHandle;

 #if 1
    #ifndef _VERBOSE
            #define _VERBOSE
    #endif
    #if 1
        #define __SHOW_HEAP_USAGE
    #endif
#endif

//Switch on to run in TLS server mode, otherwise client mode gets started
#if 1
        #ifndef __TLS_MODE_SERVER
                #define __TLS_MODE_SERVER
        #endif
#endif

// Socket & Port number definition
#define SOCK_ID_TCP       0
#define PORT_TCP          80
#define R 22
wiz_NetInfo pnetinfo2, pnetinfo3;

/*************************************************************************/
char rx;

static wiz_NetInfo gWIZNETINFO = {
                                   { 0x00, 0x08, 0xdc, 0x4f, 0x0f, 0x3c },  // Mac address
                                   { 192, 168, 0, 200 },        // IP address
                                   { 255, 255, 255, 0},            // Subnet mask
                                   { 192, 168, 0, 1},         // Gateway
                                   {0, 0, 0, 0},                        // Google public DNS (8.8.8.8 , 8.8.4.4), OpenDNS (208.67.222.222 , 208.67.220.220)
                                   NETINFO_STATIC
                               };

static void W5500_NetConfiguration()
{
    ctlnetwork(CN_SET_NETINFO, (void*)&gWIZNETINFO);
}

void runClient(uint8_t channel)
{
    int32_t iResult = 0;
    switch(getSn_SR(channel))
    {

         case SOCK_CLOSE_WAIT:
         {
#ifdef _VERBOSE
              UART_Printf("\nPeer has requested to close current connection.\n");
#endif
              if((iResult = disconnect(channel)) != SOCK_OK)
               {
                    return;
               }
               break;
          }

          case SOCK_CLOSED:
          {

              if((iResult = socket(channel, Sn_MR_TCP, PORT_TCP, 0x00)) != SOCK_OK)
              {
                  return;
              }
              break;
             }

             case SOCK_ESTABLISHED :
            {
                // Check interrupt: connection with peer is successful
                if(getSn_IR(channel) & Sn_IR_CON)
                {
                    // Clear corresponding bit
                    setSn_IR(channel,Sn_IR_CON);
                }
#ifdef _VERBOSE
                UART_Printf("\r\nA successful connection has been established...");
#endif

                uint8_t send_request[] = "Start loopback \r\n";
								uint8_t recv_msg[1024] = {0};

								int recv_size = 0;
								int send_size = 0;
								send_size = send(channel, send_request, sizeof(send_request));
								if(send_size != sizeof(send_request))
								{
										UART_Printf("\r\n send hat Error !!! %d ", send_size);
										break;
								}
								
								while (getSn_SR(channel) == SOCK_ESTABLISHED)
								{
									recv_size = recv(channel, recv_msg, sizeof(recv_msg));
									if(recv_size == 0)
									{
											UART_Printf("\r\n ReCV hat Error !!! %d ", recv_size);
											break;
									}
									
									int send_size = send(channel, recv_msg, recv_size);
									if(send_size != recv_size)
									{
											UART_Printf("\r\n send hat Error !!! %d ", send_size);
											break;
									}
							}

                break;
          }

         // Socket is opened with TCP mode
         case SOCK_INIT :
         {

            pnetinfo2.ip[0] = 192;
            pnetinfo2.ip[1] = 168;
            pnetinfo2.ip[2] = 188;
            pnetinfo2.ip[3] = 33;
            // Listen to connection request
            if( connect(channel, (uint8_t *)&pnetinfo2.ip, PORT_TCP)!= SOCK_OK )
             {
#ifdef _VERBOSE
                  UART_Printf("\r\nCould not connect to peer!");
#endif
                return;
             }

            break;
         }

        default:
         {
            break;
         }
     }//switch
}//runTLSClient()


void runServer(uint8_t channel)
{
    int iResult = 0;
    switch(getSn_SR(channel))
    {

        case SOCK_CLOSE_WAIT:
        {
#ifdef _VERBOSE
             UART_Printf("\nPeer has requested to close current connection.\n");
#endif
             //close(channel);
             if((iResult = close(channel)) != SOCK_OK)
             {
                return;
             }
             break;
         }

         case SOCK_CLOSED:
         {

#ifdef _VERBOSE
             UART_Printf("\r\n SOCK_CLOSED ! \r\n");
#endif
              if((iResult = socket(channel, Sn_MR_TCP, PORT_TCP, 0x00)) != channel)
             {
#ifdef _VERBOSE
                  UART_Printf("\r\n socket has Error!");
#endif
                  return;
              }
              break;
         }

         case SOCK_ESTABLISHED:
         {

#ifdef _VERBOSE
              UART_Printf("\r\n SOCK_ESTABLISHED = %X .\r\n", getSn_SR(channel));
#endif
               // Check interrupt: connection with peer is successful
               if(getSn_IR(channel) & Sn_IR_CON)
               {
                // Clear corresponding bit
                    setSn_IR(channel,Sn_IR_CON);
               }
							 
                int32_t size_msg = 0;
                uint8_t recv_msg[1024] = {0};
								
								size_msg = recv(channel, recv_msg, sizeof(recv_msg));
                if(size_msg == 0)
                {
                    UART_Printf("\r\n ReCV hat Error !!! %d ", size_msg);
									  break;
                }
								
                //__m_delay(5000);
								 char answer[] =    "HTTP/1.1 200 OK\r\n"
                                    "Content-Length: 552\r\n"
                                    "Content-Type: text/html\r\n"
                                    "\r\n"

                                    "<HTML>\r\n"
                                    "<BODY >\r\n"
                                    "\r\n"
                                    "<center><img src=\"https://www.pointblank.de/files/assets/slider/supergirl.jpg\" /></center>"
                                    "<center><a href =\"https://www.pointblank.de/en/\"><img src=\"https://www.pointblank.de/files/theme/icons/apple-touch-icon.png\" /></a></center>"
                                    "<center><br><font size=\"15\" color=\"blue\">pointblank-security</font></br></center>"
                                    "<p><center><br><font size=\"5\">PointBlank Security"
                                    "<br>"
                                    "powered by:"
                                    "<br>"
                                    "Steen Harbach AG"
                                    "<br>"
                                    "Alte Garten 60"
                                    "<br>"
                                    "51371 Leverkusen"
                                    "<br>"
                                    "Germany"
                                    "<br>"
                                    "Fon: +49 214 315212-0"
                                    "\r\n"
                                     "</tr>"
                                    "</BODY>\r\n"
                                    "</HTML>";


                size_msg = send(channel, answer, strlen(answer));
								UART_Printf("%s \r\n", answer);
                if(size_msg != strlen(answer))
                {
                    UART_Printf("\r\n send hat Error !!! %d ", size_msg);
									  break;
                }
								disconnect(channel);
								
								UART_Printf("SOCK_ESTABLISHED finish \r\n");
                break;
             }
             // Socket is opened with TCP mode
             case SOCK_INIT :
             {
#ifdef _VERBOSE
                  UART_Printf("\r\nSOCK_INIT = %X .\r\n", getSn_SR(channel));
#endif
                  // Listen to connection request
                   if( (iResult = listen(channel)) != SOCK_OK)
                  {
                      UART_Printf("\r\n SOCK_INIT != SOCK_OK !\r\n");
                      return;
                  }

             break;
            }

             default:
             {
                  break;
             }
    }//switch(iResult)
}//runTLSServer()

void printf_PNET(wiz_NetInfo *PNET)
{
		UART_Printf(" \r\n NET Info ...");
    UART_Printf("\r\nMAC = %02X:%02X:%02X:%02X:%02X:%02X:",PNET->mac[0] ,PNET->mac[1],PNET->mac[2],PNET->mac[3],PNET->mac[4],PNET->mac[5]);
    UART_Printf("\r\nIP = %d.%d.%d.%d",PNET->ip[0] ,PNET->ip[1],PNET->ip[2],PNET->ip[3]);
    UART_Printf("\r\nsn = %d.%d.%d.%d",PNET->sn[0] ,PNET->sn[1],PNET->sn[2],PNET->sn[3]);
    UART_Printf("\r\ngw = %d.%d.%d.%d\r\n",PNET->gw[0] ,PNET->gw[1],PNET->gw[2],PNET->gw[3]);
}

void spi_gpio_init(void)
{
    int ret;

    memset((void *)&spiHandle, 0, sizeof spiHandle);
    // Configuration for SPI1 Master
    spiHandle.bank = SPI_BANK_2;
    spiHandle.clockPhase = 0;
    spiHandle.clockPolarity = 0;
    spiHandle.dataFormat = SPI_DSS_08_BIT_DATA;
    spiHandle.dmaMode = SPI_DMA_MODE_TXRX;
    spiHandle.frameFormat = SPI_MOTOROLA_SPI_FRAME;
    spiHandle.intrMode = SPI_INTR_MODE_TXRX;
    spiHandle.mode = SPI_MASTER_MODE;
    spiHandle.prescale = 0x2;
    spiHandle.reg = (void *)(0x40004000);

    SPI_ModeSelect(&spiHandle);
    SPI_PreScale(&spiHandle);
    SPI_ClockPolarity(&spiHandle);
    SPI_ClockPhase(&spiHandle);
    SPI_DataSize(&spiHandle);
    SPI_FrameFormat(&spiHandle);
    SPI_Enable(&spiHandle);
    SPI_DmaEnable(&spiHandle);

    // Initialize for GPIO port x
    gpioHandle.bank = GPIO_BANK_2; // B.
    GPIO_Init(&gpioHandle);
    GPIO_SetDir(&gpioHandle, GPIO_PIN_1, 0, SET);  // .1
}

static void uart_isr_rx_handler(void *param)
{
    tUART_HANDLE* handle = (tUART_HANDLE*)param;
    // Get RX data
		rx = UART_ReceiveChar(handle);
		UART_Printf("Test Start\r\n");
}

void network_init(void)
{
   uint8_t tmpstr[6];
	ctlnetwork(CN_SET_NETINFO, (void*)&gWIZNETINFO);
	ctlnetwork(CN_GET_NETINFO, (void*)&gWIZNETINFO);

	// Display Network Information
	ctlwizchip(CW_GET_ID,(void*)tmpstr);
	UART_Printf("\r\n=== %s NET CONF ===\r\n",(char*)tmpstr);
	UART_Printf("MAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n",gWIZNETINFO.mac[0],gWIZNETINFO.mac[1],gWIZNETINFO.mac[2],
		  gWIZNETINFO.mac[3],gWIZNETINFO.mac[4],gWIZNETINFO.mac[5]);
	UART_Printf("SIP: %d.%d.%d.%d\r\n", gWIZNETINFO.ip[0],gWIZNETINFO.ip[1],gWIZNETINFO.ip[2],gWIZNETINFO.ip[3]);
	UART_Printf("GAR: %d.%d.%d.%d\r\n", gWIZNETINFO.gw[0],gWIZNETINFO.gw[1],gWIZNETINFO.gw[2],gWIZNETINFO.gw[3]);
	UART_Printf("SUB: %d.%d.%d.%d\r\n", gWIZNETINFO.sn[0],gWIZNETINFO.sn[1],gWIZNETINFO.sn[2],gWIZNETINFO.sn[3]);
	UART_Printf("DNS: %d.%d.%d.%d\r\n", gWIZNETINFO.dns[0],gWIZNETINFO.dns[1],gWIZNETINFO.dns[2],gWIZNETINFO.dns[3]);
	UART_Printf("======================\r\n");
}

void gpio_all_low(void)
{
		uint8_t gpioNum;
		uint16_t gpio_t =  0x0000000000000001;
	
		tGPIO_HANDLE gpioA;
		tGPIO_HANDLE gpioB;
		tGPIO_HANDLE gpioC;
	
	//GPIO Setting
		gpioA.bank = GPIO_BANK_1; // A
		gpioB.bank = GPIO_BANK_2; // B
		gpioC.bank = GPIO_BANK_3; // C
	
		GPIO_Init(&gpioA);
		GPIO_Init(&gpioB);
		GPIO_Init(&gpioC);
		
		GPIO_SetDir(&gpioA, GPIO_PIN_ALL, 0, SET);
		GPIO_SetDir(&gpioB, GPIO_PIN_4, 0, SET);
		GPIO_SetDir(&gpioB, GPIO_PIN_5, 0, SET);
		GPIO_SetDir(&gpioB, GPIO_PIN_6, 0, SET);
		GPIO_SetDir(&gpioB, GPIO_PIN_7, 0, SET);
		GPIO_SetDir(&gpioB, GPIO_PIN_10, 0, SET);
		GPIO_SetDir(&gpioB, GPIO_PIN_11, 0, SET);
		GPIO_SetDir(&gpioB, GPIO_PIN_12, 0, SET);
		GPIO_SetDir(&gpioB, GPIO_PIN_13, 0, SET);
		GPIO_SetDir(&gpioB, GPIO_PIN_14, 0, SET);
		GPIO_SetDir(&gpioB, GPIO_PIN_15, 0, SET);
		GPIO_SetDir(&gpioC, GPIO_PIN_ALL, 0, SET);
		
//		GPIO_WritePin(&gpioA, GPIO_PIN_ALL, 0);
//		GPIO_WritePin(&gpioB, GPIO_PIN_ALL, 0);
//		GPIO_WritePin(&gpioC, GPIO_PIN_ALL, 0);
		
		//GPIO ALL LOW
//		UART_Printf("-----------------------GPIO1-------------------------\r\n");
//		for(int i = 0; i<=15; i++)
//		{
//			gpioNum = GPIO_ReadPin(&gpioA, gpio_t);
//			UART_Printf("gpioA_%d_pin : %X, status : %d\r\n", i, gpio_t, gpioNum);
//			gpio_t <<= 1;
//		}
//		UART_Printf("----------------------------------------------------\r\n");
//		gpio_t = 0x0000000000000001;
//		
//		UART_Printf("-----------------------GPIO2-------------------------\r\n");
//		for(int i = 0; i<=15; i++)
//		{
//			gpioNum = GPIO_ReadPin(&gpioB, gpio_t);
//			UART_Printf("gpioB_%d_pin : %X, status : %d\r\n", i, gpio_t, gpioNum);
//			gpio_t <<= 1;
//		}
//		UART_Printf("----------------------------------------------------\r\n");
//		gpio_t = 0x0000000000000001;
//		
//		UART_Printf("-----------------------GPIO3-------------------------\r\n");
//		for(int i = 0; i<=15; i++)
//		{
//			gpioNum = GPIO_ReadPin(&gpioC, gpio_t);
//			UART_Printf("gpioC_%d_pin : %X, status : %d\r\n", i, gpio_t, gpioNum);
//			gpio_t <<= 1;
//		}
//		UART_Printf("----------------------------------------------------\r\n");
//		uint8_t gpioPC9 = GPIO_ReadPin(&gpioC, GPIO_PIN_9);
//		if(gpioPC9 == 0)
//			{
//				UART_Printf("PC9 is Low\r\n");
//			}
//		else
//		{
//			UART_Printf("PC9 is High\r\n");
//		}
}



void ping_client_test(uint8_t *destip)
{
	 int x, y;
   extern uint8_t ping_auto(uint8_t s, uint8_t *addr);
	 uint8_t tmp=0;
	 uint8_t stxBuf[] = "ss";
	 uint8_t ftxBuf[] = "ff";
   UART_Printf( "Ping Client Test\r\n" );
   while(1) {
		 
	 while(1)
	 {
			 if(rx == 'a')
			 {
				 __m_delay(2000);
				 tmp = ping_auto( 0, destip );
				 break;
			 }
			 else
				 continue;
	 }
	 
	 
	 if(tmp == 3)
	 {
		 __m_delay(1000);
		 UART_Transmit(&uartHandle, stxBuf, sizeof(stxBuf), 100);
	 }
	 else
	 {
		 __m_delay(1000);
		 UART_Transmit(&uartHandle, ftxBuf, sizeof(ftxBuf), 100);
	 }
		 break;
	 __m_delay(1000);
 }
}

uint8_t rxBuf[5];
int main(void)
{
    int ret;
		uint8_t pDestaddr[4] = {192,168,0,5};
		uint8_t txBuf[] = "*";
		
    // Set UART1 handle
    uartHandle.bank = EWBM_DEBUG_UART_PORT;
    uartHandle.baudRate = UART_BAUDRATE_115200;
    uartHandle.wordLength = UART_WORDLENGTH_8BIT;

    // Initialize UART1
    ret = UART_Init(&uartHandle);
    if ( ret != EWBMOK )
        return ret;

    // Enable FIFO
    ret = UART_EnableFifo(&uartHandle, UART_FIFO_LEVEL_7_OVER_8, UART_FIFO_LEVEL_1_OVER_8);
    if ( ret != EWBMOK )
        return ret;

    UART_DisableFifo(&uartHandle);

    // Enable UART1
    ret = UART_EnableUart(&uartHandle, UART_MODE_TX_RX);
    if ( ret != EWBMOK )
        return ret;
		
	  // Enable UART1 Interrupt TX, RX
    UART_EnableInterrupt(&uartHandle, UART_MODE_RX, NULL, NULL, uart_isr_rx_handler, &uartHandle);
		
    // Initialize Debug with the UART1 handle
    UART_PrintInit(&uartHandle);
		
		// Disable UART1 FIFO for TX, RX interrupt test
    ret = UART_DisableFifo(&uartHandle);
    if ( ret != EWBMOK )
        return ret;

    // Initialize RX buffer
    memset(rxBuf, 0x00, sizeof(rxBuf));
		
		gpio_all_low();
		
    spi_gpio_init();

    W5500_Init();
		
		W5500_NetConfiguration();
		
		wizchip_getnetinfo(&pnetinfo2);
		
    //printf_PNET(&pnetinfo2);
		
		ping_client_test(pDestaddr);
		
    while(1) {
#ifdef __TLS_MODE_SERVER
			__m_delay(1000);
			
        runServer(SOCK_ID_TCP);
#else
        runClient(SOCK_ID_TCP);
#endif
    }
}

