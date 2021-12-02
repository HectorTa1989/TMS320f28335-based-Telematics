/*
 * DSP2833x_Sci.c
 *  Author: Hector Ta
 *  Created on: Dec 2021
 *      
 */

#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File
#include <string.h>
#include <stdlib.h>
#define CPU_FREQ 	150E6
#define LSPCLK_FREQ CPU_FREQ/4
#define SCI_FREQ 	(Uint32)115200
#define SCIB_FREQ 	(Uint32)9600
#define SCI_PRD     (LSPCLK_FREQ/(SCI_FREQ*8))-1
#define SCIB_PRD	(LSPCLK_FREQ/(SCIB_FREQ*8))-1
void InitSciaA(void);
void InitSciaB(void);
//---------------------------------------------------------------------------
// InitSci:
//---------------------------------------------------------------------------
// This function initializes the SCI(s) to a known state.
//
void SCI_Reset(){
	SciaRegs.SCICTL1.bit.SWRESET=0;
	SciaRegs.SCICTL1.bit.SLEEP=1;
	SciaRegs.SCICTL1.bit.SWRESET=1;
	SciaRegs.SCIFFTX.bit.SCIRST = 0;
	SciaRegs.SCIFFTX.bit.SCIRST = 1; //SCI FIFO resume transmit or receive.
	InitSciaA();
}


void InitSciaA(void)
{

	// Initialize SCI-A:
	SciaRegs.SCICCR.all = 0x07;			// 8 bit word, idle,par disabled, 1 stop bit
	SciaRegs.SCICTL1.all = 0x03;		// Reset and RX/TX enable, no sleep
//	SciaRegs.SCICTL1.all = 0x23;		// Making sure that RX/TX are enabled

	SciaRegs.SCIHBAUD = (((Uint16)SCI_PRD >> 8) & 0xff);
	SciaRegs.SCILBAUD =  ((Uint16)SCI_PRD & 0xff);

//	SciaRegs.SCICTL2.all = 0x03;		// Transmit/receive ready interrupt enable
	SciaRegs.SCIFFTX.all=0xC028;
	SciaRegs.SCIFFRX.all=0x0028;
	SciaRegs.SCIFFCT.all=0x00;

	SciaRegs.SCICTL1.all =0x0023;     // Relinquish SCI from Reset
	SciaRegs.SCIFFTX.bit.TXFIFOXRESET=1;
	SciaRegs.SCIFFRX.bit.RXFIFORESET=1;



}
void InitSciaB(void)
{
   ScibRegs.SCICCR.all =0x0007;    // 1 stop bit,  No loopback
                                   // No parity,8 char bits,
                                   // async mode, idle-line protocol
   ScibRegs.SCICTL1.all =0x0003;   // enable TX, RX, internal SCICLK,
                                   // Disable RX ERR, SLEEP, TXWAKE
   ScibRegs.SCICTL2.bit.TXINTENA =1;
   ScibRegs.SCICTL2.bit.RXBKINTENA =1;
   ScibRegs.SCIHBAUD = (((Uint16)SCIB_PRD >> 8) & 0xff);
   ScibRegs.SCILBAUD =  ((Uint16)SCIB_PRD & 0xff);
   ScibRegs.SCIFFTX.all=0xC028;
   ScibRegs.SCIFFRX.all=0x0028;
   ScibRegs.SCIFFCT.all=0x00;

   ScibRegs.SCICTL1.all =0x0023;     // Relinquish SCI from Reset
   ScibRegs.SCIFFTX.bit.TXFIFOXRESET=1;
   ScibRegs.SCIFFRX.bit.RXFIFORESET=1;
}
//---------------------------------------------------------------------------
// Example: InitSciGpio:
//---------------------------------------------------------------------------
// This function initializes GPIO pins to function as SCI pins
//
// Each GPIO pin can be configured as a GPIO pin or up to 3 different
// peripheral functional pins. By default all pins come up as GPIO
// inputs after reset.
//
// Caution:
// Only one GPIO pin should be enabled for SCITXDA/B operation.
// Only one GPIO pin shoudl be enabled for SCIRXDA/B operation.
// Comment out other unwanted lines.

void InitSciGpio()
{
   InitSciaGpio();
   InitScibGpio();
#if DSP28_SCIC
   InitScicGpio();
#endif // if DSP28_SCIC
}

void InitSciaGpio()
{
   EALLOW;

/* Enable internal pull-up for the selected pins */
// Pull-ups can be enabled or disabled disabled by the user.  
// This will enable the pullups for the specified pins.

	GpioCtrlRegs.GPAPUD.bit.GPIO28 = 0;    // Enable pull-up for GPIO28 (SCIRXDA)
	GpioCtrlRegs.GPAPUD.bit.GPIO29 = 0;	   // Enable pull-up for GPIO29 (SCITXDA)

/* Set qualification for selected pins to asynch only */
// Inputs are synchronized to SYSCLKOUT by default.  
// This will select asynch (no qualification) for the selected pins.

	GpioCtrlRegs.GPAQSEL2.bit.GPIO28 = 3;  // Asynch input GPIO28 (SCIRXDA)

/* Configure SCI-A pins using GPIO regs*/
// This specifies which of the possible GPIO pins will be SCI functional pins.

	GpioCtrlRegs.GPAMUX2.bit.GPIO28 = 1;   // Configure GPIO28 for SCIRXDA operation
	GpioCtrlRegs.GPAMUX2.bit.GPIO29 = 1;   // Configure GPIO29 for SCITXDA operation
	

	GpioCtrlRegs.GPAMUX1.bit.GPIO12 = 0;  //Modem CTS GPIO12
	GpioCtrlRegs.GPADIR.bit.GPIO12 = 0;   //Modem CTS Input

    EDIS;
}

void InitScibGpio()
{
   EALLOW;

/* Enable internal pull-up for the selected pins */
// Pull-ups can be enabled or disabled disabled by the user.
// This will enable the pullups for the specified pins.
// Comment out other unwanted lines.

//  GpioCtrlRegs.GPAPUD.bit.GPIO9 = 0;     // Enable pull-up for GPIO9  (SCITXDB)
//  GpioCtrlRegs.GPAPUD.bit.GPIO14 = 0;    // Enable pull-up for GPIO14 (SCITXDB)
	GpioCtrlRegs.GPAPUD.bit.GPIO18 = 0;	   // Enable pull-up for GPIO18 (SCITXDB)
//  GpioCtrlRegs.GPAPUD.bit.GPIO22 = 0;    // Enable pull-up for GPIO22 (SCITXDB)


//  GpioCtrlRegs.GPAPUD.bit.GPIO11 = 0;    // Enable pull-up for GPIO11 (SCIRXDB)
//  GpioCtrlRegs.GPAPUD.bit.GPIO15 = 0;    // Enable pull-up for GPIO15 (SCIRXDB)
    GpioCtrlRegs.GPAPUD.bit.GPIO19 = 0;	   // Enable pull-up for GPIO19 (SCIRXDB)
//  GpioCtrlRegs.GPAPUD.bit.GPIO23 = 0;    // Enable pull-up for GPIO23 (SCIRXDB)

/* Set qualification for selected pins to asynch only */
// This will select asynch (no qualification) for the selected pins.
// Comment out other unwanted lines.

//  GpioCtrlRegs.GPAQSEL1.bit.GPIO11 = 3;  // Asynch input GPIO11 (SCIRXDB)
//  GpioCtrlRegs.GPAQSEL1.bit.GPIO15 = 3;  // Asynch input GPIO15 (SCIRXDB)
	GpioCtrlRegs.GPAQSEL2.bit.GPIO19 = 3;  // Asynch input GPIO19 (SCIRXDB)
//  GpioCtrlRegs.GPAQSEL2.bit.GPIO23 = 3;  // Asynch input GPIO23 (SCIRXDB)

/* Configure SCI-B pins using GPIO regs*/
// This specifies which of the possible GPIO pins will be SCI functional pins.
// Comment out other unwanted lines.

//  GpioCtrlRegs.GPAMUX1.bit.GPIO9 = 2;    // Configure GPIO9 for SCITXDB operation
//  GpioCtrlRegs.GPAMUX1.bit.GPIO14 = 2;   // Configure GPIO14 for SCITXDB operation
	GpioCtrlRegs.GPAMUX2.bit.GPIO18 = 2;   // Configure GPIO18 for SCITXDB operation
//  GpioCtrlRegs.GPAMUX2.bit.GPIO22 = 3;   // Configure GPIO22 for SCITXDB operation
	
//  GpioCtrlRegs.GPAMUX1.bit.GPIO11 = 2;   // Configure GPIO11 for SCIRXDB operation
//  GpioCtrlRegs.GPAMUX1.bit.GPIO15 = 2;   // Configure GPIO15 for SCIRXDB operation
    GpioCtrlRegs.GPAMUX2.bit.GPIO19 = 2;   // Configure GPIO19 for SCIRXDB operation
//  GpioCtrlRegs.GPAMUX2.bit.GPIO23 = 3;   // Configure GPIO23 for SCIRXDB operation
	
    EDIS;
}


#if DSP28_SCIC
void InitScicGpio()
{
   EALLOW;

/* Enable internal pull-up for the selected pins */
// Pull-ups can be enabled or disabled disabled by the user.
// This will enable the pullups for the specified pins.

	GpioCtrlRegs.GPBPUD.bit.GPIO62 = 0;    // Enable pull-up for GPIO62 (SCIRXDC)
	GpioCtrlRegs.GPBPUD.bit.GPIO63 = 0;	   // Enable pull-up for GPIO63 (SCITXDC)

/* Set qualification for selected pins to asynch only */
// Inputs are synchronized to SYSCLKOUT by default.
// This will select asynch (no qualification) for the selected pins.

	GpioCtrlRegs.GPBQSEL2.bit.GPIO62 = 3;  // Asynch input GPIO62 (SCIRXDC)

/* Configure SCI-C pins using GPIO regs*/
// This specifies which of the possible GPIO pins will be SCI functional pins.

	GpioCtrlRegs.GPBMUX2.bit.GPIO62 = 1;   // Configure GPIO62 for SCIRXDC operation
	GpioCtrlRegs.GPBMUX2.bit.GPIO63 = 1;   // Configure GPIO63 for SCITXDC operation

    EDIS;
}
#endif // if DSP28_SCIC


//===========================================================================
// End of file.
//===========================================================================
