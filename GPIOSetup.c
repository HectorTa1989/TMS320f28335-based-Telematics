/*
 * GPIOSetup.c
 *  Author: Hector Ta
 *  Created on: Dec 2021
 *      
 */
#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File

void Gpio_setup(void)
{
   // Example 1:
   // Basic Pinout.
   // This basic pinout includes:
   // PWM1-3, ECAP1, ECAP2, TZ1-TZ4, SPI-A, EQEP1, SCI-A, I2C
   // and a number of I/O pins

   // These can be combined into single statements for improved
   // code efficiency.

   // Enable PWM1-3 on GPIO0-GPIO5
   EALLOW;
   GpioCtrlRegs.GPAPUD.bit.GPIO0 = 0;   // Enable pullup on GPIO0
   GpioCtrlRegs.GPAPUD.bit.GPIO1 = 0;   // Enable pullup on GPIO1
   GpioCtrlRegs.GPAPUD.bit.GPIO2 = 0;   // Enable pullup on GPIO2
   GpioCtrlRegs.GPAPUD.bit.GPIO3 = 0;   // Enable pullup on GPIO3
   GpioCtrlRegs.GPAPUD.bit.GPIO4 = 0;   // Enable pullup on GPIO4
   GpioCtrlRegs.GPAPUD.bit.GPIO5 = 0;   // Enable pullup on GPIO5
   GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 0;  // GPIO0 = GPIO0
   GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 1;  // GPIO1 = PWM1B
   GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 0;  // GPIO2 = GPIO
   GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 1;  // GPIO3 = PWM2B
//   GpioCtrlRegs.GPAMUX1.bit.GPIO4 = 1;  // GPIO4 = PWM3A
   GpioCtrlRegs.GPAMUX1.bit.GPIO5 = 1;  // GPIO5 = PWM3B


   GpioCtrlRegs.GPADIR.bit.GPIO0 = 1;
   GpioCtrlRegs.GPADIR.bit.GPIO2 = 1;
   // Enable an GPIO output on GPIO6, set it high
   GpioCtrlRegs.GPAPUD.bit.GPIO6 = 0;   // Enable pullup on GPIO6
   GpioDataRegs.GPASET.bit.GPIO6 = 1;   // Load output latch
   GpioCtrlRegs.GPAMUX1.bit.GPIO6 = 0;  // GPIO6 = GPIO6
   GpioCtrlRegs.GPADIR.bit.GPIO6 = 1;   // GPIO6 = output

   // Enable eCAP1 on GPIO7
   GpioCtrlRegs.GPAPUD.bit.GPIO7 = 0;   // Enable pullup on GPIO7
   GpioCtrlRegs.GPAQSEL1.bit.GPIO7 = 0; // Synch to SYSCLOUT
   GpioCtrlRegs.GPAMUX1.bit.GPIO7 = 3;  // GPIO7 = ECAP2

   // Enable GPIO outputs on GPIO8 - GPIO11, set it high
   GpioCtrlRegs.GPAPUD.bit.GPIO8 = 0;   // Enable pullup on GPIO8
   GpioCtrlRegs.GPAMUX1.bit.GPIO8 = 2;  // GPIO8 = CANTXB

   GpioCtrlRegs.GPAPUD.bit.GPIO9 = 0;   // Enable pullup on GPIO9
   GpioDataRegs.GPASET.bit.GPIO9 = 1;   // Load output latch
   GpioCtrlRegs.GPAMUX1.bit.GPIO9 = 0;  // GPIO9 = GPIO9
   GpioCtrlRegs.GPADIR.bit.GPIO9 = 1;   // GPIO9 = output

   GpioCtrlRegs.GPAPUD.bit.GPIO10 = 0;  // Enable pullup on GPIO10
   GpioCtrlRegs.GPAQSEL1.bit.GPIO10 = 3;  // Asynch input
   GpioCtrlRegs.GPAMUX1.bit.GPIO10 = 2; // GPIO10 = CANRXB

   GpioCtrlRegs.GPAPUD.bit.GPIO11 = 0;  // Enable pullup on GPIO11
   GpioCtrlRegs.GPAMUX1.bit.GPIO11 = 0; // GPIO11 = GPIO11
   GpioCtrlRegs.GPADIR.bit.GPIO11 = 1;  // GPIO11 = output

   // Enable Trip Zone inputs on GPIO12 - GPIO15
//   GpioCtrlRegs.GPAPUD.bit.GPIO12 = 0;   // Enable pullup on GPIO12
   GpioCtrlRegs.GPAPUD.bit.GPIO13 = 0;   // Enable pullup on GPIO13
   GpioCtrlRegs.GPAPUD.bit.GPIO14 = 0;   // Enable pullup on GPIO14
   GpioCtrlRegs.GPAPUD.bit.GPIO15 = 0;   // Enable pullup on GPIO15
//   GpioCtrlRegs.GPAQSEL1.bit.GPIO12 = 3; // asynch input
   GpioCtrlRegs.GPAQSEL1.bit.GPIO13 = 3; // asynch input
   GpioCtrlRegs.GPAQSEL1.bit.GPIO14 = 3; // asynch input
   GpioCtrlRegs.GPAQSEL1.bit.GPIO15 = 3; // asynch input
//   GpioCtrlRegs.GPAMUX1.bit.GPIO12 = 1;  // GPIO12 = TZ1
   GpioCtrlRegs.GPAMUX1.bit.GPIO13 = 1;  // GPIO13 = TZ2
   GpioCtrlRegs.GPAMUX1.bit.GPIO14 = 1;  // GPIO14 = TZ3
   GpioCtrlRegs.GPAMUX1.bit.GPIO15 = 1;  // GPIO15 = TZ4

   // Enable SPI-A on GPIO16 - GPIO19
//   GpioCtrlRegs.GPAPUD.bit.GPIO16 = 0;   // Enable pullup on GPIO16
//  GpioCtrlRegs.GPAPUD.bit.GPIO17 = 0;   // Enable pullup on GPIO17
//   GpioCtrlRegs.GPAPUD.bit.GPIO18 = 0;   // Enable pullup on GPIO18
//  GpioCtrlRegs.GPAPUD.bit.GPIO19 = 0;   // Enable pullup on GPIO19
//   GpioCtrlRegs.GPAQSEL2.bit.GPIO16 = 3; // asynch input
//   GpioCtrlRegs.GPAQSEL2.bit.GPIO17 = 3; // asynch input
//   GpioCtrlRegs.GPAQSEL2.bit.GPIO18 = 3; // asynch input
//   GpioCtrlRegs.GPAQSEL2.bit.GPIO19 = 3; // asynch input
//   GpioCtrlRegs.GPAMUX2.bit.GPIO16 = 1;  // GPIO16 = SPICLKA
//   GpioCtrlRegs.GPAMUX2.bit.GPIO17 = 1;  // GPIO17 = SPIS0MIA
//   GpioCtrlRegs.GPAMUX2.bit.GPIO18 = 1;  // GPIO18 = SPICLKA
//   GpioCtrlRegs.GPAMUX2.bit.GPIO19 = 1;  // GPIO19 = SPISTEA

   // Enable EQEP1 on GPIO20 - GPIO23
   GpioCtrlRegs.GPAPUD.bit.GPIO20 = 0;   // Enable pullup on GPIO20
   GpioCtrlRegs.GPAPUD.bit.GPIO21 = 0;   // Enable pullup on GPIO21
   GpioCtrlRegs.GPAPUD.bit.GPIO22 = 0;   // Enable pullup on GPIO22
   GpioCtrlRegs.GPAPUD.bit.GPIO23 = 0;   // Enable pullup on GPIO23
   GpioCtrlRegs.GPAQSEL2.bit.GPIO20 = 0; // Synch to SYSCLKOUT
   GpioCtrlRegs.GPAQSEL2.bit.GPIO21 = 0; // Synch to SYSCLKOUT
   GpioCtrlRegs.GPAQSEL2.bit.GPIO22 = 0; // Synch to SYSCLKOUT
   GpioCtrlRegs.GPAQSEL2.bit.GPIO23 = 0; // Synch to SYSCLKOUT
   GpioCtrlRegs.GPAMUX2.bit.GPIO20 = 0;  // GPIO20 = GPIO20
   GpioCtrlRegs.GPAMUX2.bit.GPIO21 = 1;  // GPIO21 = EQEP1B
   GpioCtrlRegs.GPAMUX2.bit.GPIO22 = 0;  // GPIO22 = SCIBTX // USE GPS TX SCIB
   GpioCtrlRegs.GPAMUX2.bit.GPIO23 = 0;  // GPIO23 = SCIBRX // Use GPS RX SICB
   GpioCtrlRegs.GPADIR.bit.GPIO20 = 1; //GPIO20 = Output

   // Enable eCAP1 on GPIO24
   GpioCtrlRegs.GPAPUD.bit.GPIO24 = 0;   // Enable pullup on GPIO24
   GpioCtrlRegs.GPAQSEL2.bit.GPIO24 = 0; // Synch to SYSCLKOUT
   GpioCtrlRegs.GPAMUX2.bit.GPIO24 = 1;  // GPIO24 = ECAP1

   // Set input qualifcation period for GPIO25 & GPIO26
   GpioCtrlRegs.GPACTRL.bit.QUALPRD3=1;  // Qual period = SYSCLKOUT/2
   GpioCtrlRegs.GPAQSEL2.bit.GPIO25=2;   // 6 samples
//   GpioCtrlRegs.GPAQSEL2.bit.GPIO26=2;   // 6 samples

   // Make GPIO25 the input source for Xint1
   GpioCtrlRegs.GPAMUX2.bit.GPIO25 = 0;  // GPIO25 = GPIO25
   GpioCtrlRegs.GPADIR.bit.GPIO25 = 1;   // GPIO25 = input
   GpioDataRegs.GPACLEAR.bit.GPIO25 = 1;
   // Make GPIO26 the output source for LED1
   GpioCtrlRegs.GPAMUX2.bit.GPIO26 = 0;  // GPIO26 = GPIO26
   GpioCtrlRegs.GPADIR.bit.GPIO26 = 1;   // GPIO26 = Output
//   GpioIntRegs.GPIOXINT2SEL.all = 26;    // XINT2 connected to GPIO26

   // Make GPIO27 wakeup from HALT/STANDBY Low Power Modes
   GpioCtrlRegs.GPAMUX2.bit.GPIO27 = 0; // GPIO27 = GPIO27
   GpioCtrlRegs.GPADIR.bit.GPIO27 = 0;  // GPIO27 = input
   GpioIntRegs.GPIOLPMSEL.bit.GPIO27=1; // GPIO27 will wake the device
   SysCtrlRegs.LPMCR0.bit.QUALSTDBY=2;  // Qualify GPIO27 by 2 OSCCLK
                                        // cycles before waking the device
                                        // from STANDBY

   // Enable SCI-A on GPIO28 - GPIO29
   GpioCtrlRegs.GPAPUD.bit.GPIO28 = 0;   // Enable pullup on GPIO28
   GpioCtrlRegs.GPAQSEL2.bit.GPIO28 = 3; // Asynch input
   GpioCtrlRegs.GPAMUX2.bit.GPIO28 = 1;  // GPIO28 = SCIRXDA
   GpioCtrlRegs.GPAPUD.bit.GPIO29 = 0;   // Enable pullup on GPIO29
   GpioCtrlRegs.GPAMUX2.bit.GPIO29 = 1;  // GPIO29 = SCITXDA

   // Enable CAN-A on GPIO30 - GPIO31
   GpioCtrlRegs.GPAPUD.bit.GPIO30 = 0;   // Enable pullup on GPIO30
   GpioCtrlRegs.GPAMUX2.bit.GPIO30 = 1;  // GPIO30 = CANTXA

   GpioCtrlRegs.GPAPUD.bit.GPIO31 = 0;   // Enable pullup on GPIO31
   GpioCtrlRegs.GPAQSEL2.bit.GPIO31 = 3; // Asynch input
   GpioCtrlRegs.GPAMUX2.bit.GPIO31 = 1;  // GPIO31 = CANRXA


   // Enable I2C-A on GPIO32 - GPIO33
   GpioCtrlRegs.GPBPUD.bit.GPIO32 = 0;   // Enable pullup on GPIO32
   GpioCtrlRegs.GPBMUX1.bit.GPIO32 = 0;  // GPIO32 = GPIO
   GpioCtrlRegs.GPBDIR.bit.GPIO32 = 1;   // GPIO8 = output


   GpioCtrlRegs.GPBQSEL1.bit.GPIO33 = 3; // Asynch input
   GpioCtrlRegs.GPBPUD.bit.GPIO33 = 0;   // Enable pullup on GPIO33
   GpioCtrlRegs.GPBQSEL1.bit.GPIO33 = 3; // Asynch input
   GpioCtrlRegs.GPBMUX1.bit.GPIO33 = 1;  // GPIO33 = SCLA

   // Make GPIO34 an input
   GpioCtrlRegs.GPBPUD.bit.GPIO34 = 0;  // Enable pullup on GPIO34
   GpioCtrlRegs.GPBMUX1.bit.GPIO34 = 0; // GPIO34 = GPIO34
   GpioCtrlRegs.GPBDIR.bit.GPIO34 = 0;  // GPIO34 = input

   //Make GPIO BOOTCAN,BOOT_SCI LOW
 /*  GpioCtrlRegs.GPCPUD.bit.GPIO82 = 0;   // Enable pullup on GPIO9
   GpioDataRegs.GPCSET.bit.GPIO82 = 0;   // Load output latch
   GpioCtrlRegs.GPCMUX2.bit.GPIO82 = 0;  // GPIO82 = GPIO9
   GpioCtrlRegs.GPCDIR.bit.GPIO82 = 1;   // GPIO82 = output
   GpioCtrlRegs.GPCPUD.bit.GPIO83 = 0;   // Enable pullup on GPIO9
   GpioDataRegs.GPCSET.bit.GPIO83 = 0;   // Load output latch
   GpioCtrlRegs.GPCMUX2.bit.GPIO83 = 0;  // GPIO83 = GPIO9
   GpioCtrlRegs.GPCDIR.bit.GPIO83 = 1;   // GPIO83 = output
*/
   EDIS;
}
