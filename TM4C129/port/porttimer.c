/*
 * FreeModbus Libary: BARE Port
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

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- static functions ---------------------------------*/
void prvvTIMERExpiredISR( void );


/* test function */
#define TIMER_TEST_BASE        GPIO_PORTN_BASE
#define TIMER_TEST_SYSCTL      SYSCTL_PERIPH_GPION
#define TIMER_TEST             GPIO_PIN_4

static uint32_t timer_period = 0;


/* ----------------------- Start implementation -----------------------------*/
BOOL
xMBPortTimersInit( USHORT usTim1Timerout50us )
{
    timer_period = (g_ui32SysClock/20000)*usTim1Timerout50us;

    SysCtlPeripheralEnable(MODBUS_TIMER_SYSCTL);
    SysCtlPeripheralReset(MODBUS_TIMER_SYSCTL);
    while(!SysCtlPeripheralReady(MODBUS_TIMER_SYSCTL))
        ;
    TimerConfigure(MODBUS_TIMER_BASE, TIMER_CFG_PERIODIC);
    TimerLoadSet(MODBUS_TIMER_BASE, TIMER_A, timer_period);
    TimerIntEnable(MODBUS_TIMER_BASE, TIMER_TIMA_TIMEOUT);
    IntEnable(INT_TIMER0A);

    SysCtlPeripheralEnable(TIMER_TEST_SYSCTL);
    while(!SysCtlPeripheralReady(TIMER_TEST_SYSCTL))
        ;
    GPIOPinTypeGPIOOutput(TIMER_TEST_BASE, TIMER_TEST);
    GPIOPinWrite(TIMER_TEST_BASE, TIMER_TEST, 0);

    return TRUE;
}


inline void
vMBPortTimersEnable(  )
{
    /* Enable the timer with the timeout passed to xMBPortTimersInit( ) */
    TimerLoadSet(MODBUS_TIMER_BASE, TIMER_A, timer_period);
    GPIOPinWrite(TIMER_TEST_BASE, TIMER_TEST, 0xff);
    TimerEnable(MODBUS_TIMER_BASE, TIMER_A);
}

inline void
vMBPortTimersDisable(  )
{
    /* Disable any pending timers. */
    TimerDisable(MODBUS_TIMER_BASE, TIMER_A);
}

/* Create an ISR which is calTIMER_TEST whenever the timer has expired. This function
 * must then call pxMBPortCBTimerExpired( ) to notify the protocol stack that
 * the timer has expired.
 */
void prvvTIMERExpiredISR( void )
{
    GPIOPinWrite(TIMER_TEST_BASE, TIMER_TEST, 0);


    TimerIntClear(MODBUS_TIMER_BASE, TIMER_TIMA_TIMEOUT);
    ( void )pxMBPortCBTimerExpired(  );
}

