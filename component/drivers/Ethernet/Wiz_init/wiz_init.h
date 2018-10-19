//*****************************************************************************
//
//! \file wiz_init.h
//! \brief W5500 initialization header .
//! \details SPI function access to register API and W5500 initialization
//! \version 1.0.0
//! \date 2016/04/14
//! \par  Revision history
//!       <2016/04/14>
//! \author RedBeam
//
//*****************************************************************************

#ifndef _WIZ_INIT_H_INCLUDED
#define _WIZ_INIT_H_INCLUDED

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

//#define DHCP

#define SOCK_DHCP           1

void set_default_network(void);
void W5500_Init(void);

#endif
/* WIZ_INIT_H_INCLUDED */
