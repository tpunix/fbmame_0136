/*****************************************************************************
 *
 *   mcs51.h
 *   Portable MCS-51 Family Emulator
 *
 *   Chips in the family:
 *   8051 Product Line (8031,8051,8751)
 *   8052 Product Line (8032,8052,8752)
 *   8054 Product Line (8054)
 *   8058 Product Line (8058)
 *
 *   Copyright Steve Ellenoff, all rights reserved.
 *
 *   - This source code is released as freeware for non-commercial purposes.
 *   - You are free to use and redistribute this code in modified or
 *     unmodified form, provided you list me in the credits.
 *   - If you modify this source code, you must add a notice to each modified
 *     source file that it has been changed.  If you're a nice person, you
 *     will clearly mark each change too.  :)
 *   - If you wish to use this for commercial purposes, please contact me at
 *     sellenoff@hotmail.com
 *   - The author of this copywritten work reserves the right to change the
 *     terms of its usage and license at any time, including retroactively
 *   - This entire notice must remain in the source code.
 *
 *  This work is based on:
 *  #1) 'Intel(tm) MC51 Microcontroller Family Users Manual' and
 *  #2) 8051 simulator by Travis Marlatte
 *  #3) Portable UPI-41/8041/8741/8042/8742 emulator V0.1 by Juergen Buchmueller (MAME CORE)
 *
 * 2008, October, Couriersud
 * - Rewrite of timer, interrupt and serial code
 * - addition of CMOS features
 * - internal memory maps
 * - addition of new processor types
 * - full emulation of 8xCx2 processors
 *****************************************************************************/

#pragma once

#ifndef __MCS51_H__
#define __MCS51_H__

#include "cpuintrf.h"

enum
{
	MCS51_PC=1, MCS51_SP, MCS51_PSW, MCS51_ACC, MCS51_B, MCS51_DPH, MCS51_DPL, MCS51_IE,
	MCS51_R0, MCS51_R1, MCS51_R2, MCS51_R3, MCS51_R4, MCS51_R5, MCS51_R6, MCS51_R7, MCS51_RB
};

enum
{
	MCS51_INT0_LINE = 0,	/* P3.2: External Interrupt 0 */
	MCS51_INT1_LINE,		/* P3.3: External Interrupt 1 */
	MCS51_RX_LINE,			/* P3.0: Serial Port Receive Line */
	MCS51_T0_LINE,			/* P3,4: Timer 0 External Input */
	MCS51_T1_LINE,			/* P3.5: Timer 1 External Input */
	MCS51_T2_LINE,			/* P1.0: Timer 2 External Input */
	MCS51_T2EX_LINE,		/* P1.1: Timer 2 Capture Reload Trigger */

	DS5002FP_PFI_LINE,		/* DS5002FP Power fail interrupt */
};

/* special I/O space ports */

enum
{
	MCS51_PORT_P0	= 0x20000,
	MCS51_PORT_P1	= 0x20001,
	MCS51_PORT_P2	= 0x20002,
	MCS51_PORT_P3	= 0x20003,
	MCS51_PORT_TX	= 0x20004,	/* P3.1 */
};

/***************************************************************************
    CONFIGURATION
***************************************************************************/

/* configuration of the DS5002FP */
typedef struct _ds5002fp_config ds5002fp_config;
struct _ds5002fp_config
{
	UINT8	mcon;					/* bootstrap loader MCON register */
	UINT8	rpctl;					/* bootstrap loader RPCTL register */
	UINT8	crc;					/* bootstrap loader CRC register */
};

/***************************************************************************
    FUNCTION PROTOTYPES
***************************************************************************/

typedef void (*mcs51_serial_tx_func)(const device_config *device, int data);
typedef int (*mcs51_serial_rx_func)(const device_config *device);

extern void i8051_set_serial_tx_callback(const device_config *device, mcs51_serial_tx_func tx_func);
extern void i8051_set_serial_rx_callback(const device_config *device, mcs51_serial_rx_func rx_func);

/* variants with no internal rom and 128 byte internal memory */
CPU_GET_INFO( i8031 );
#define CPU_I8031 CPU_GET_INFO_NAME( i8031 )

/* variants with no internal rom and 256 byte internal memory */
CPU_GET_INFO( i8032 );
#define CPU_I8032 CPU_GET_INFO_NAME( i8032 )

/* variants 4k internal rom and 128 byte internal memory */
CPU_GET_INFO( i8051 );
CPU_GET_INFO( i8751 );

#define CPU_I8051 CPU_GET_INFO_NAME( i8051 )
#define CPU_I8751 CPU_GET_INFO_NAME( i8751 )

/* variants 8k internal rom and 256 byte internal memory and more registers */
CPU_GET_INFO( i8052 );
CPU_GET_INFO( i8752 );

#define CPU_I8052 CPU_GET_INFO_NAME( i8052 )
#define CPU_I8752 CPU_GET_INFO_NAME( i8752 )

/* cmos variants */
CPU_GET_INFO( i80c31 );
CPU_GET_INFO( i80c51 );
CPU_GET_INFO( i87c51 );

#define CPU_I80C31 CPU_GET_INFO_NAME( i80c31 )
#define CPU_I80C32 CPU_GET_INFO_NAME( i80c32 )
#define CPU_I80C51 CPU_GET_INFO_NAME( i80c51 )

CPU_GET_INFO( i80c32 );
CPU_GET_INFO( i80c52 );
CPU_GET_INFO( i87c52 );

#define CPU_I80C52 CPU_GET_INFO_NAME( i80c52 )
#define CPU_I87C51 CPU_GET_INFO_NAME( i87c51 )
#define CPU_I87C52 CPU_GET_INFO_NAME( i87c52 )

/* 4k internal perom and 128 internal ram and 2 analog comparators */
CPU_GET_INFO( at89c4051 );
#define CPU_AT89C4051 CPU_GET_INFO_NAME( at89c4051 )

/*
 * The DS5002FP has 2 16 bits data address buses (the byte-wide bus and the expanded bus). The exact memory position accessed depends on the
 * partition mode, the memory range and the expanded bus select. The partition mode and the expanded bus select can be changed at any time.
 *
 * In order to simplify memory mapping to the data address bus, the following address map is assumed for partitioned mode:

 * 0x00000-0x0ffff -> data memory on the expanded bus
 * 0x10000-0x1ffff -> data memory on the byte-wide bus

 * For non-partitioned mode the following memory map is assumed:

 * 0x0000-0xffff -> data memory (the bus used to access it does not matter)
 *
 * Internal ram 128k and security features
 */

CPU_GET_INFO( ds5002fp );
#define CPU_DS5002FP CPU_GET_INFO_NAME( ds5002fp )


/****************************************************************************
 * Disassembler
 ****************************************************************************/

CPU_DISASSEMBLE( i8051 );
CPU_DISASSEMBLE( i80c51 );
CPU_DISASSEMBLE( i8052 );
CPU_DISASSEMBLE( i80c52 );
CPU_DISASSEMBLE( ds5002fp );

#endif /* __MCS51_H__ */
