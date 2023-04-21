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

#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- static functions ---------------------------------*/
static void prvvUARTTxReadyISR( void );
static void prvvUARTRxISR( void );

#define UART_TEST_BASE        GPIO_PORTN_BASE
#define UART_TEST_SYSCTL      SYSCTL_PERIPH_GPION
#define UART_TEST             GPIO_PIN_5

uint8_t uart_state = 0x00;

/* ----------------------- Start implementation -----------------------------*/
void
vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{
    /* If xRXEnable enable serial receive interrupts. If xTxENable enable
     * transmitter empty interrupts.
     */
    if( xRxEnable )
    {
        UARTIntEnable(MODBUS_UART_BASE, UART_INT_RX);
    }
    else
    {
        UARTIntDisable(MODBUS_UART_BASE, UART_INT_RX);
    }
    if( xTxEnable )
    {
        UARTIntEnable(MODBUS_UART_BASE, UART_INT_TX);
    }
    else
    {
        UARTIntDisable(MODBUS_UART_BASE, UART_INT_TX);
    }
}

BOOL
xMBPortSerialInit( UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity )
{
    BOOL bInitialized = TRUE;
    LONG UARTcfg = 0;

    SysCtlPeripheralEnable(MODBUS_UART_SYSCTL);
    SysCtlPeripheralReset(MODBUS_UART_SYSCTL);

    SysCtlPeripheralEnable(MODBUS_UART_RX_SYSCTL);
    SysCtlPeripheralEnable(MODBUS_UART_TX_SYSCTL);

    while( !SysCtlPeripheralReady(MODBUS_UART_SYSCTL) ||
           !SysCtlPeripheralReady(MODBUS_UART_RX_SYSCTL) ||
           !SysCtlPeripheralReady(MODBUS_UART_TX_SYSCTL))
    {
        ;
    }

    GPIOPinConfigure(MODBUS_UART_RX_CFG);
    GPIOPinConfigure(MODBUS_UART_TX_CFG);

    GPIOPinTypeUART(MODBUS_UART_RX_BASE, MODBUS_UART_RX);
    GPIOPinTypeUART(MODBUS_UART_TX_BASE, MODBUS_UART_TX);

    //NOTE: Cases 5 and 6 are not needed per examples
    switch(ucDataBits){
    case 5:
        UARTcfg |= UART_CONFIG_WLEN_5;
        break;
    case 6:
        UARTcfg |= UART_CONFIG_WLEN_6;
        break;
    case 7:
        UARTcfg |= UART_CONFIG_WLEN_7;
        break;
    case 8:
        UARTcfg |= UART_CONFIG_WLEN_8;
        break;
    default:
        bInitialized = FALSE;
        break;
    }

    switch(eParity){
    case MB_PAR_NONE:
        UARTcfg |= UART_CONFIG_PAR_NONE;
        UARTcfg |= UART_CONFIG_STOP_TWO;
        break;
    case MB_PAR_ODD:
        UARTcfg |= UART_CONFIG_PAR_ODD;
        UARTcfg |= UART_CONFIG_STOP_ONE;
        break;
    case MB_PAR_EVEN:
        UARTcfg |= UART_CONFIG_PAR_EVEN;
        UARTcfg |= UART_CONFIG_STOP_ONE;
        break;
    default:
        bInitialized = FALSE;
        break;
    }

    uint32_t txF;
    uint32_t rxF;
    //NOTE: Stop bit not configured in this segment
    UARTFIFOLevelSet(MODBUS_UART_BASE, UART_FIFO_TX1_8, UART_FIFO_RX1_8);
    UARTFIFOLevelGet(MODBUS_UART_BASE, &txF, &rxF);
    IntEnable(INT_UART0);
    UARTConfigSetExpClk(MODBUS_UART_BASE, g_ui32SysClock, ulBaudRate, UARTcfg);

    SysCtlPeripheralEnable(UART_TEST_SYSCTL);
    GPIOPinTypeGPIOOutput(UART_TEST_BASE, UART_TEST);
    GPIOPinWrite(UART_TEST_BASE,UART_TEST,uart_state);

    return bInitialized;
}

BOOL
xMBPortSerialPutByte( CHAR ucByte )
{
    /* Put a byte in the UARTs transmit buffer. This function is called
     * by the protocol stack if pxMBFrameCBTransmitterEmpty( ) has been
     * called. */
    UARTCharPut(MODBUS_UART_BASE, ucByte);
    return TRUE;
}

BOOL
xMBPortSerialGetByte( CHAR * pucByte )
{
    /* Return the byte in the UARTs receive buffer. This function is called
     * by the protocol stack after pxMBFrameCBByteReceived( ) has been called.
     */
    *pucByte = (CHAR)UARTCharGet(MODBUS_UART_BASE);
    return TRUE;
}

/* Create an interrupt handler for the transmit buffer empty interrupt
 * (or an equivalent) for your target processor. This function should then
 * call pxMBFrameCBTransmitterEmpty( ) which tells the protocol stack that
 * a new character can be sent. The protocol stack will then call 
 * xMBPortSerialPutByte( ) to send the character.
 */
static void prvvUARTTxReadyISR( void )
{
    pxMBFrameCBTransmitterEmpty(  );
}

/* Create an interrupt handler for the receive interrupt for your target
 * processor. This function should then call pxMBFrameCBByteReceived( ). The
 * protocol stack will then call xMBPortSerialGetByte( ) to retrieve the
 * character.
 */
static void prvvUARTRxISR( void )
{
    pxMBFrameCBByteReceived(  );
}

/**
 * Check the Interrupt handler for RX and TX interrupts generated by the
 * peripheral, then determine the correct clearing course of action to
 * follow with the pre-built interrupt service request for each.
 */
void
prvvUARTISR( void ){
    uint32_t ui32Status;

    uart_state = uart_state ^ 0xff;
    GPIOPinWrite(UART_TEST_BASE, UART_TEST, uart_state);

    ui32Status = UARTIntStatus(MODBUS_UART_BASE, TRUE);
    UARTIntClear(MODBUS_UART_BASE, ui32Status);

    if(ui32Status & UART_INT_TX){
        prvvUARTTxReadyISR();
    }
    if(ui32Status & UART_INT_RX){
        prvvUARTRxISR();
    }
}
