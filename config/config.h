
/***************************************************************************/
#ifndef _CONFIG_H_
#define _CONFIG_H_

/***************************************************************************
 * 1. INCLUDES                                                             *
 ***************************************************************************/

#ifdef  __cplusplus
/* *INDENT-OFF* */
extern "C" {
 /* *INDENT-ON* */
#endif

/***************************************************************************
 * 2. DEFINES                                                              *
 ***************************************************************************/

#ifndef UART1_Write_Text
    #define UART1_Write_Text UART_Printf
#endif


#    ifndef SINT64
/** 64 bit signed integer. Not MISRA compatible. */
typedef signed long long SINT64;
#    endif
#endif

#ifndef UINT32
/** 32 bit unsigned integer. */
typedef unsigned long UINT32;
#endif

#ifndef UINT16
/** 16 bit unsigned integer. */
typedef unsigned short UINT16;
#endif

#ifndef UINT8
/** 8 bit unsigned integer. */
typedef unsigned char UINT8;
#endif

#ifndef SINT32
/** 32 bit signed integer. */
typedef signed long SINT32;
#endif

#ifndef SINT16
/** 16 bit signed integer. */
typedef signed short SINT16;
#endif

#ifndef SINT8
/** 8 bit signed integer. */
typedef signed char SINT8;
#endif

#ifndef CHAR
/** Single character of a C-Style string. */
typedef char CHAR;
#endif

#ifndef BOOL
/** Type can only be TRUE (Non-zero) or FALSE (Zero). Boolean operations deliver the expected results. */
typedef UINT8 BOOL;
#endif

#ifndef TRUE
/** BOOL value true. */
#    define TRUE        ( (BOOL)1 )
#endif

#ifndef FALSE
/** BOOL value false. */
#    define FALSE       ( (BOOL)0 )
#endif

/***************************************************************************
 * 3. DECLARATIONS                                                         *
 ***************************************************************************/

/***************************************************************************
 * 4. CONSTANTS                                                            *
 ***************************************************************************/

/***************************************************************************
 * 5. FUNCTION PROTOTYPES                                                  *
 ***************************************************************************/

/***************************************************************************
 * 6. MACRO FUNCTIONS                                                      *
 ***************************************************************************/

#ifdef  __cplusplus
/* *INDENT-OFF* */
}
/* *INDENT-ON* */
#endif
/***************************************************************************
 * 7. END                                                                  *
 ***************************************************************************/
