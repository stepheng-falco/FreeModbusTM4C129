/*
 * FreeModbus Libary: TM4C129 Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id$
 */

#ifndef _PORT_H
#define _PORT_H

/* ----------------------- Platform includes --------------------------------*/

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"

#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"
#include "driverlib/sysctl.h"
#include "driverlib/debug.h"
#include "driverlib/interrupt.h"

extern uint32_t g_ui32SysClock;


#define MODBUS_TIMER_BASE           TIMER0_BASE
#define MODBUS_TIMER_SYSCTL         SYSCTL_PERIPH_TIMER0

#define MODBUS_UART_BASE            UART0_BASE
#define MODBUS_UART_SYSCTL          SYSCTL_PERIPH_UART0

#define MODBUS_UART_RX_BASE         GPIO_PORTA_BASE
#define MODBUS_UART_RX_SYSCTL       SYSCTL_PERIPH_GPIOA
#define MODBUS_UART_RX              GPIO_PIN_0
#define MODBUS_UART_RX_CFG          GPIO_PA0_U0RX

#define MODBUS_UART_TX_BASE         GPIO_PORTA_BASE
#define MODBUS_UART_TX_SYSCTL       SYSCTL_PERIPH_GPIOA
#define MODBUS_UART_TX              GPIO_PIN_1
#define MODBUS_UART_TX_CFG          GPIO_PA1_U0TX

/* */

#define	INLINE                      inline
#define PR_BEGIN_EXTERN_C           extern "C" {
#define	PR_END_EXTERN_C             }

#define ENTER_CRITICAL_SECTION( )   
#define EXIT_CRITICAL_SECTION( )    

typedef uint8_t BOOL;

typedef unsigned char UCHAR;
typedef char CHAR;

typedef uint16_t USHORT;
typedef int16_t SHORT;

typedef uint32_t ULONG;
typedef int32_t LONG;

#ifndef TRUE
#define TRUE            1
#endif

#ifndef FALSE
#define FALSE           0
#endif

#endif
