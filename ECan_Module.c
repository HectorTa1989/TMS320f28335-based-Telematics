/*
 * ECan_Module.c
 *  Author: Hector Ta
 *  Created on: Dec 2021
 *      
 */
#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File
extern void CollectPackData();
extern void ClearReserved();
/************************************************************************************/
/*		CAN_Init:	This subroutine initializes the CAN Module 				 		*/
/************************************************************************************/
void InitECana(void)		// Initialize eCAN-A module
{		//500bps CAN 1
/* Create a shadow register structure for the CAN control registers. This is
 needed, since only 32-bit access is allowed to these registers. 16-bit access
 to these registers could potentially corrupt the register contents or return
 false data. This is especially true while writing to/reading from a bit
 (or group of bits) among bits 16 - 31 */

	struct ECAN_REGS ECanaShadow;
	EALLOW;
	// EALLOW enables access to protected bits
	/* Configure eCAN RX and TX pins for CAN operation using eCAN regs*/
	ECanaShadow.CANTIOC.all = ECanaRegs.CANTIOC.all;
	ECanaShadow.CANTIOC.bit.TXFUNC = 1;
	ECanaRegs.CANTIOC.all = ECanaShadow.CANTIOC.all;

	ECanaShadow.CANRIOC.all = ECanaRegs.CANRIOC.all;
	ECanaShadow.CANRIOC.bit.RXFUNC = 1;
	ECanaRegs.CANRIOC.all = ECanaShadow.CANRIOC.all;
	/* Configure eCAN for HECC mode - (reqd to access mailboxes 16 thru 31) */
	// HECC mode also enables time-stamping feature
	ECanaShadow.CANMC.all = ECanaRegs.CANMC.all;
	ECanaShadow.CANMC.bit.SCB = 1;
	ECanaRegs.CANMC.all = ECanaShadow.CANMC.all;
	// Configure bit timing parameters for eCANA
	ECanaShadow.CANMC.all = ECanaRegs.CANMC.all;
	ECanaShadow.CANMC.bit.CCR = 1;            // Set CCR = 1
	ECanaRegs.CANMC.all = ECanaShadow.CANMC.all;
	while (ECanaRegs.CANES.bit.CCE != 1) {
	}   // Wait for CCE bit to be set..
	ECanaShadow.CANMC.all = ECanaRegs.CANMC.all;
	//   ECANAShadow.CANMC.bit.SUSP=1;
	ECanaShadow.CANMC.bit.CCR = 0;   // Set Config change request = 0
	ECanaShadow.CANMC.bit.PDR = 0; // set power down mode request = 0(normal operation)
	ECanaShadow.CANMC.bit.WUBA = 0;  // set wake up on bus activity
	ECanaShadow.CANMC.bit.CDR = 0; // set change data field request (normal operation)
	ECanaShadow.CANMC.bit.ABO = 0;   // Auto bus on
	ECanaShadow.CANMC.bit.STM = 0;   // self test mode (normal operation)
	ECanaShadow.CANMC.bit.SRES = 0;  // no effect (0)
	ECanaShadow.CANMC.bit.MBNR = 0;  // # of mailbox

	ECanaRegs.CANMC.all = ECanaShadow.CANMC.all;

	/* Initialize all bits of 'Master Control Field' to zero */
// Some bits of MSGCTRL register come up in an unknown state. For proper operation,
// all bits (including reserved bits) of MSGCTRL must be initialized to zero
	ECanaMboxes.MBOX0.MSGCTRL.all = 0x00000000;
	ECanaMboxes.MBOX1.MSGCTRL.all = 0x00000000;
	ECanaMboxes.MBOX2.MSGCTRL.all = 0x00000000;
	ECanaMboxes.MBOX3.MSGCTRL.all = 0x00000000;
	ECanaMboxes.MBOX4.MSGCTRL.all = 0x00000000;
	ECanaMboxes.MBOX5.MSGCTRL.all = 0x00000000;
	ECanaMboxes.MBOX6.MSGCTRL.all = 0x00000000;
	ECanaMboxes.MBOX7.MSGCTRL.all = 0x00000000;
	ECanaMboxes.MBOX8.MSGCTRL.all = 0x00000000;
	ECanaMboxes.MBOX9.MSGCTRL.all = 0x00000000;
	ECanaMboxes.MBOX10.MSGCTRL.all = 0x00000000;
	ECanaMboxes.MBOX11.MSGCTRL.all = 0x00000000;
	ECanaMboxes.MBOX12.MSGCTRL.all = 0x00000000;
	ECanaMboxes.MBOX13.MSGCTRL.all = 0x00000000;
	ECanaMboxes.MBOX14.MSGCTRL.all = 0x00000000;
	ECanaMboxes.MBOX15.MSGCTRL.all = 0x00000000;
	ECanaMboxes.MBOX16.MSGCTRL.all = 0x00000000;
	ECanaMboxes.MBOX17.MSGCTRL.all = 0x00000000;
	ECanaMboxes.MBOX18.MSGCTRL.all = 0x00000000;
	ECanaMboxes.MBOX19.MSGCTRL.all = 0x00000000;
	ECanaMboxes.MBOX20.MSGCTRL.all = 0x00000000;
	ECanaMboxes.MBOX21.MSGCTRL.all = 0x00000000;
	ECanaMboxes.MBOX22.MSGCTRL.all = 0x00000000;
	ECanaMboxes.MBOX23.MSGCTRL.all = 0x00000000;
	ECanaMboxes.MBOX24.MSGCTRL.all = 0x00000000;
	ECanaMboxes.MBOX25.MSGCTRL.all = 0x00000000;
	ECanaMboxes.MBOX26.MSGCTRL.all = 0x00000000;
	ECanaMboxes.MBOX27.MSGCTRL.all = 0x00000000;
	ECanaMboxes.MBOX28.MSGCTRL.all = 0x00000000;
	ECanaMboxes.MBOX29.MSGCTRL.all = 0x00000000;
	ECanaMboxes.MBOX30.MSGCTRL.all = 0x00000000;
	ECanaMboxes.MBOX31.MSGCTRL.all = 0x00000000;

// TAn, RMPn, GIFn bits are all zero upon reset and are cleared again
//	as a matter of precaution.

	ECanaRegs.CANTA.all = 0xFFFFFFFF; /* Clear all TAn bits */

	ECanaRegs.CANRMP.all = 0xFFFFFFFF; /* Clear all RMPn bits */

	ECanaRegs.CANGIF0.all = 0xFFFFFFFF; /* Clear all interrupt flag bits */
	ECanaRegs.CANGIF1.all = 0xFFFFFFFF;

	/* Configure bit timing parameters for eCANA*/
	ECanaShadow.CANMC.all = ECanaRegs.CANMC.all;
	ECanaShadow.CANMC.bit.CCR = 1;            // Set CCR = 1
	ECanaRegs.CANMC.all = ECanaShadow.CANMC.all;

	ECanaShadow.CANES.all = ECanaRegs.CANES.all;

	do {
		ECanaShadow.CANES.all = ECanaRegs.CANES.all;
	} while (ECanaShadow.CANES.bit.CCE != 1);  	// Wait for CCE bit to be set..

	ECanaShadow.CANBTC.all = 0;

#if (CPU_FRQ_150MHZ)                       // CPU_FRQ_150MHz is defined in DSP2833x_Examples.h
	/* The following block for all 150 MHz SYSCLKOUT (75 MHz CAN clock) - default. Bit rate = 1 Mbps
	 See Note at End of File */
	ECanaShadow.CANBTC.bit.BRPREG = 9; //500kbps
	ECanaShadow.CANBTC.bit.TSEG2REG = 2;
	ECanaShadow.CANBTC.bit.TSEG1REG = 10;
#endif
#if (CPU_FRQ_100MHZ)                       // CPU_FRQ_100MHz is defined in DSP2833x_Examples.h
	/* The following block is only for 100 MHz SYSCLKOUT (50 MHz CAN clock). Bit rate = 1 Mbps
	 See Note at End of File */
	ECanaShadow.CANBTC.bit.BRPREG = 4;
	ECanaShadow.CANBTC.bit.TSEG2REG = 1;
	ECanaShadow.CANBTC.bit.TSEG1REG = 6;
#endif

	ECanaShadow.CANBTC.bit.SAM = 1;
	ECanaRegs.CANBTC.all = ECanaShadow.CANBTC.all;

	ECanaShadow.CANMC.all = ECanaRegs.CANMC.all;
	ECanaShadow.CANMC.bit.CCR = 0;            // Set CCR = 0
	//ECanaShadow.CANMC.bit.DBO = 1;
	ECanaRegs.CANMC.all = ECanaShadow.CANMC.all;

	ECanaShadow.CANES.all = ECanaRegs.CANES.all;

	do {
		ECanaShadow.CANES.all = ECanaRegs.CANES.all;
	} while (ECanaShadow.CANES.bit.CCE != 0); // Wait for CCE bit to be  cleared..

	/* Disable all Mailboxes  */

	ECanaRegs.CANME.all = 0;		// Required before writing the MSGIDs

	ECanaMboxes.MBOX0.MSGID.all = 0X21L << 18;		//Odometer
	ECanaMboxes.MBOX1.MSGID.all  = 0X20L << 18;     //Vehicle Accessory
	ECanaMboxes.MBOX2.MSGID.all = 0X30L <<18;	 //MotorInfo, Motor Current
	ECanaMboxes.MBOX3.MSGID.all = 0X622L <<18;  //BMS Fail Code
	ECanaMboxes.MBOX4.MSGID.all = 0x25L <<18;   //12v Battery
	ECanaMboxes.MBOX5.MSGID.all = 0X610L <<18;  //BMS ChargeState,ContactorState
	ECanaMboxes.MBOX6.MSGID.all = 0X41L <<18;   //AC ChargeState,Diag
	ECanaMboxes.MBOX7.MSGID.all = 0X42L <<18;   //J1772State
	ECanaMboxes.MBOX8.MSGID.all = 0X105L <<18;  //CHAdeMO State
	ECanaMboxes.MBOX9.MSGID.all = 0x605L <<18;  //CTC State
	ECanaMboxes.MBOX10.MSGID.all = 0X31L <<18;   //MCU State
	ECanaMboxes.MBOX11.MSGID.all = 0X200L <<18;  //DCDC State
	ECanaMboxes.MBOX12.MSGID.all = 0X210L <<18;  //DCAC State
	ECanaMboxes.MBOX13.MSGID.all = 0X300L <<18;  //Transmission State
	ECanaMboxes.MBOX14.MSGID.all = 0X440L <<18;  //AC compressor State
	ECanaMboxes.MBOX15.MSGID.all = 0X102L <<18;  //CHAdeMO Target
	ECanaMboxes.MBOX16.MSGID.all = 0X601L <<18;  //CTC Target

	ECanaMboxes.MBOX17.MSGID.all = 0xC1L << 18;		//tx CMD
	ECanaMboxes.MBOX18.MSGID.all = 0xC2L << 18;		//tx DATA
	ECanaMboxes.MBOX19.MSGID.all = 0xB1L << 18;		//rx CMD
	ECanaMboxes.MBOX20.MSGID.all = 0xB2L << 18;		//rx DATA

	ECanaMboxes.MBOX0.MSGCTRL.bit.DLC = 8;
	ECanaMboxes.MBOX1.MSGCTRL.bit.DLC = 8;
	ECanaMboxes.MBOX2.MSGCTRL.bit.DLC = 8;
	ECanaMboxes.MBOX3.MSGCTRL.bit.DLC = 8;
	ECanaMboxes.MBOX4.MSGCTRL.bit.DLC = 8;
	ECanaMboxes.MBOX5.MSGCTRL.bit.DLC = 8;
	ECanaMboxes.MBOX6.MSGCTRL.bit.DLC = 8;
	ECanaMboxes.MBOX7.MSGCTRL.bit.DLC = 8;
	ECanaMboxes.MBOX8.MSGCTRL.bit.DLC = 8;
	ECanaMboxes.MBOX9.MSGCTRL.bit.DLC = 8;
	ECanaMboxes.MBOX10.MSGCTRL.bit.DLC = 8;
	ECanaMboxes.MBOX11.MSGCTRL.bit.DLC = 8;
	ECanaMboxes.MBOX12.MSGCTRL.bit.DLC = 8;
	ECanaMboxes.MBOX13.MSGCTRL.bit.DLC = 8;
	ECanaMboxes.MBOX14.MSGCTRL.bit.DLC = 8;
	ECanaMboxes.MBOX15.MSGCTRL.bit.DLC = 8;
	ECanaMboxes.MBOX16.MSGCTRL.bit.DLC = 8;

	ECanaMboxes.MBOX17.MSGCTRL.bit.DLC = 8;
	ECanaMboxes.MBOX18.MSGCTRL.bit.DLC = 8;
	ECanaMboxes.MBOX19.MSGCTRL.bit.DLC = 8;
	ECanaMboxes.MBOX20.MSGCTRL.bit.DLC = 8;

	ECanaRegs.CANOPC.all = 0x01FFFFF;
	// setup (enable) the I1EN(Rx), I0EN(Tx), GIL and others on Global interrupt mask reg

	ECanaShadow.CANMIM.all = 0;
	ECanaShadow.CANMIM.all  = 0x1FFFFF;  //  Mailbox interrupt is enabled
  	ECanaRegs.CANMIM.all = ECanaShadow.CANMIM.all;

  	ECanaShadow.CANMIL.all = 0;
  	ECanaShadow.CANMIL.all = 0x1FFFFF;
	ECanaRegs.CANMIL.all = ECanaShadow.CANMIL.all;

	ECanaShadow.CANGIM.all = 0;
	ECanaShadow.CANGIM.bit.I1EN = 1;  // enable I1EN
	ECanaShadow.CANGIM.bit.GIL = 1;
	ECanaRegs.CANGIM.all = ECanaShadow.CANGIM.all;
// 	ECanaRegs.CANGIM.all = 0x00002E06;
	/*	Configure Acceptance Mask	*/
	ECanaRegs.CANMD.all = 0x019FFFF;
	/* re-enable all Mailboxes*/
	ECanaRegs.CANME.all = 0x01FFFFF;
	PieCtrlRegs.PIEIER9.bit.INTx6 = 1;
	//	PieCtrlRegs.PIEIER9.all = 1;

	IER |= 0x0100;
	EDIS;
}
/************************************************************************************/
/*		CAN_Init:	This subroutine initializes the CAN Module on the C167CR.		*/
/************************************************************************************/
void InitECanb(void)		// Initialize eCAN-A module
{		//100bps Inner CAN
/* Create a shadow register structure for the CAN control registers. This is
 needed, since only 32-bit access is allowed to these registers. 16-bit access
 to these registers could potentially corrupt the register contents or return
 false data. This is especially true while writing to/reading from a bit
 (or group of bits) among bits 16 - 31 */

	struct ECAN_REGS ECanbShadow;

	EALLOW;
	// EALLOW enables access to protected bits

	/* Configure eCAN RX and TX pins for CAN operation using eCAN regs*/

	ECanbShadow.CANTIOC.all = ECanbRegs.CANTIOC.all;
	ECanbShadow.CANTIOC.bit.TXFUNC = 1;
	ECanbRegs.CANTIOC.all = ECanbShadow.CANTIOC.all;

	ECanbShadow.CANRIOC.all = ECanbRegs.CANRIOC.all;
	ECanbShadow.CANRIOC.bit.RXFUNC = 1;
	ECanbRegs.CANRIOC.all = ECanbShadow.CANRIOC.all;
	// set the eCAN for 32 mailboxes
	ECanbShadow.CANMC.all = ECanbRegs.CANMC.all;
	ECanbShadow.CANMC.bit.SCB = 1; // use all the 32 mailboxes
	//ECanbShadow.CANMC.bit.DBO = 1;  // data is Rx or Tx using LSB first
	ECanbRegs.CANMC.all = ECanbShadow.CANMC.all;

	// Configure bit timing parameters for eCANA
	ECanbShadow.CANMC.all = ECanbRegs.CANMC.all;
	ECanbShadow.CANMC.bit.CCR = 1;            // Set CCR = 1
	ECanbRegs.CANMC.all = ECanbShadow.CANMC.all;
	while (ECanbRegs.CANES.bit.CCE != 1) {
	}   // Wait for CCE bit to be set..

	ECanbShadow.CANMC.all = ECanbRegs.CANMC.all;
	//   ECANAShadow.CANMC.bit.SUSP=1;
	ECanbShadow.CANMC.bit.CCR = 0;   // Set Config change request = 0
	ECanbShadow.CANMC.bit.PDR = 0; // set power down mode request = 0(normal operation)
	ECanbShadow.CANMC.bit.WUBA = 0;  // set wake up on bus activity
	ECanbShadow.CANMC.bit.CDR = 0; // set change data field request (normal operation)
	ECanbShadow.CANMC.bit.ABO = 0;   // Auto bus on
	ECanbShadow.CANMC.bit.STM = 0;   // self test mode (normal operation)
	ECanbShadow.CANMC.bit.SRES = 0;  // no effect (0)
	ECanbShadow.CANMC.bit.MBNR = 0;  // # of mailbox

	ECanbRegs.CANMC.all = ECanbShadow.CANMC.all;

	/* Initialize all bits of 'Master Control Field' to zero */
// Some bits of MSGCTRL register come up in an unknown state. For proper operation,
// all bits (including reserved bits) of MSGCTRL must be initialized to zero
	ECanbMboxes.MBOX0.MSGCTRL.all = 0x00000000;
	ECanbMboxes.MBOX1.MSGCTRL.all = 0x00000000;
	ECanbMboxes.MBOX2.MSGCTRL.all = 0x00000000;
	ECanbMboxes.MBOX3.MSGCTRL.all = 0x00000000;
	ECanbMboxes.MBOX4.MSGCTRL.all = 0x00000000;
	ECanbMboxes.MBOX5.MSGCTRL.all = 0x00000000;
	ECanbMboxes.MBOX6.MSGCTRL.all = 0x00000000;
	ECanbMboxes.MBOX7.MSGCTRL.all = 0x00000000;
	ECanbMboxes.MBOX8.MSGCTRL.all = 0x00000000;
	ECanbMboxes.MBOX9.MSGCTRL.all = 0x00000000;
	ECanbMboxes.MBOX10.MSGCTRL.all = 0x00000000;
	ECanbMboxes.MBOX11.MSGCTRL.all = 0x00000000;
	ECanbMboxes.MBOX12.MSGCTRL.all = 0x00000000;
	ECanbMboxes.MBOX13.MSGCTRL.all = 0x00000000;
	ECanbMboxes.MBOX14.MSGCTRL.all = 0x00000000;
	ECanbMboxes.MBOX15.MSGCTRL.all = 0x00000000;
	ECanbMboxes.MBOX16.MSGCTRL.all = 0x00000000;
	ECanbMboxes.MBOX17.MSGCTRL.all = 0x00000000;
	ECanbMboxes.MBOX18.MSGCTRL.all = 0x00000000;
	ECanbMboxes.MBOX19.MSGCTRL.all = 0x00000000;
	ECanbMboxes.MBOX20.MSGCTRL.all = 0x00000000;
	ECanbMboxes.MBOX21.MSGCTRL.all = 0x00000000;
	ECanbMboxes.MBOX22.MSGCTRL.all = 0x00000000;
	ECanbMboxes.MBOX23.MSGCTRL.all = 0x00000000;
	ECanbMboxes.MBOX24.MSGCTRL.all = 0x00000000;
	ECanbMboxes.MBOX25.MSGCTRL.all = 0x00000000;
	ECanbMboxes.MBOX26.MSGCTRL.all = 0x00000000;
	ECanbMboxes.MBOX27.MSGCTRL.all = 0x00000000;
	ECanbMboxes.MBOX28.MSGCTRL.all = 0x00000000;
	ECanbMboxes.MBOX29.MSGCTRL.all = 0x00000000;
	ECanbMboxes.MBOX30.MSGCTRL.all = 0x00000000;
	ECanbMboxes.MBOX31.MSGCTRL.all = 0x00000000;

// TAn, RMPn, GIFn bits are all zero upon reset and are cleared again
//	as a matter of precaution.

	ECanbRegs.CANTA.all = 0xFFFFFFFF; /* Clear all TAn bits */

	ECanbRegs.CANRMP.all = 0xFFFFFFFF; /* Clear all RMPn bits */

	ECanbRegs.CANGIF0.all = 0xFFFFFFFF; /* Clear all interrupt flag bits */
	ECanbRegs.CANGIF1.all = 0xFFFFFFFF;

	/* Configure bit timing parameters for eCANA*/
	ECanbShadow.CANMC.all = ECanbRegs.CANMC.all;
	ECanbShadow.CANMC.bit.CCR = 1;            // Set CCR = 1
	ECanbRegs.CANMC.all = ECanbShadow.CANMC.all;

	ECanbShadow.CANES.all = ECanbRegs.CANES.all;

	do

	{
		ECanbShadow.CANES.all = ECanbRegs.CANES.all;
	} while (ECanbShadow.CANES.bit.CCE != 1);  	// Wait for CCE bit to be set..

	ECanbShadow.CANBTC.all = 0;

	ECanbShadow.CANBTC.bit.BRPREG = 49; //100kbps
	ECanbShadow.CANBTC.bit.TSEG2REG = 2;
	ECanbShadow.CANBTC.bit.TSEG1REG = 10;
#if (CPU_FRQ_100MHZ)                       // CPU_FRQ_100MHz is defined in DSP2833x_Examples.h
	/* The following block is only for 100 MHz SYSCLKOUT (50 MHz CAN clock). Bit rate = 1 Mbps
	 See Note at End of File */
	ECanbShadow.CANBTC.bit.BRPREG = 4;
	ECanbShadow.CANBTC.bit.TSEG2REG = 1;
	ECanbShadow.CANBTC.bit.TSEG1REG = 6;
#endif

	ECanbShadow.CANBTC.bit.SAM = 1;
	ECanbRegs.CANBTC.all = ECanbShadow.CANBTC.all;

	ECanbShadow.CANMC.all = ECanbRegs.CANMC.all;
	ECanbShadow.CANMC.bit.CCR = 0;            // Set CCR = 0
//	ECanbShadow.CANMC.bit.DBO = 1;
	ECanbRegs.CANMC.all = ECanbShadow.CANMC.all;

	ECanbShadow.CANES.all = ECanbRegs.CANES.all;

	do {
		ECanbShadow.CANES.all = ECanbRegs.CANES.all;
	} while (ECanbShadow.CANES.bit.CCE != 0); // Wait for CCE bit to be  cleared..

	/* Disable all Mailboxes  */
	ECanbRegs.CANTRR.all = 0x03;
	ECanbRegs.CANME.all = 0;		// Required before writing the MSGIDs

	//BMS Data
	ECanbMboxes.MBOX0.MSGID.all = 0x98310301; //tx CMD
	ECanbMboxes.MBOX1.MSGID.all = 0x98210301; //tx CMD
	ECanbMboxes.MBOX2.MSGID.all = 0x98310103; //rx Data (PackVoltage/BusVoltage/Current)
	ECanbMboxes.MBOX3.MSGID.all = 0x98320103; //rx DATA (Residual/Rate Capacity)
	ECanbMboxes.MBOX4.MSGID.all = 0x98340103; //rx DATA (Accumulated Discharge/Charge Capacity)
	ECanbMboxes.MBOX5.MSGID.all = 0x98350103; //rx DATA (SOC)
	ECanbMboxes.MBOX6.MSGID.all = 0x98210103; //rx Data (Relay Status)
	//Cell Data
	ECanbMboxes.MBOX8.MSGID.all = 0xD831030B; //rx Cell_1->10
	/*	0x1831030B
	 0x1832030B
	 0x1833030B
	 0x1834030B
	 0x1835030B*/
	ECanbMboxes.MBOX9.MSGID.all = 0xD831030C; //rx Cell_11->21
	/*	0x1831030C
	 0x1832030C
	 0x1833030C
	 0x1834030C
	 0x1835030C*/
	ECanbMboxes.MBOX10.MSGID.all = 0xD831030D; //rx Cell_22->33
	/*	0x1831030D
	 0x1832030D
	 0x1833030D
	 0x1834030D
	 0x1835030D*/
	ECanbMboxes.MBOX11.MSGID.all = 0xD831030E; //rx Cell_34->43
	/*	0x1831030E
	 0x1832030E
	 0x1833030E
	 0x1834030E
	 0x1835030E*/
	ECanbMboxes.MBOX12.MSGID.all = 0xD831030F; //rx Cell_44->53
	/*	0x1831030F
	 0x1832030F
	 0x1833030F
	 0x1834030F
	 0x1835030F*/
	ECanbMboxes.MBOX13.MSGID.all = 0xD8310310; //rx Cell_54->63
	/*	0x18310310
	 0x18320310
	 0x18330310
	 0x18340310
	 0x18350310*/
	ECanbMboxes.MBOX14.MSGID.all = 0xD8310311; //rx Cell_64->75
	/*	0x18310311
	 0x18320311
	 0x18330311
	 0x18340311
	 0x18350311*/
	ECanbMboxes.MBOX15.MSGID.all = 0xD8310312; //rx Cell_76->85
	/*	0x18310312
	 0x18320312
	 0x18330312
	 0x18340312
	 0x18350312*/
	ECanbMboxes.MBOX16.MSGID.all = 0xD8310313; //rx Cell_86->95
	/*	0x18310313
	 0x18320313
	 0x18330313
	 0x18340313
	 0x18350313*/
	ECanbMboxes.MBOX17.MSGID.all = 0xD8310314; //rx Cell_96->105
	/*	0x18310314
	 0x18320314
	 0x18330314
	 0x18340314
	 0x18350314*/

	ECanbMboxes.MBOX0.MSGCTRL.bit.DLC = 8;
	ECanbMboxes.MBOX1.MSGCTRL.bit.DLC = 8;
	ECanbMboxes.MBOX2.MSGCTRL.bit.DLC = 8;
	ECanbMboxes.MBOX3.MSGCTRL.bit.DLC = 8;
	ECanbMboxes.MBOX4.MSGCTRL.bit.DLC = 8;
	ECanbMboxes.MBOX5.MSGCTRL.bit.DLC = 8;
	ECanbMboxes.MBOX6.MSGCTRL.bit.DLC = 8;
	ECanbMboxes.MBOX7.MSGCTRL.bit.DLC = 8;
	ECanbMboxes.MBOX8.MSGCTRL.bit.DLC = 8;
	ECanbMboxes.MBOX9.MSGCTRL.bit.DLC = 8;
	ECanbMboxes.MBOX10.MSGCTRL.bit.DLC = 8;
	ECanbMboxes.MBOX11.MSGCTRL.bit.DLC = 8;
	ECanbMboxes.MBOX12.MSGCTRL.bit.DLC = 8;
	ECanbMboxes.MBOX13.MSGCTRL.bit.DLC = 8;
	ECanbMboxes.MBOX14.MSGCTRL.bit.DLC = 8;
	ECanbMboxes.MBOX15.MSGCTRL.bit.DLC = 8;
	ECanbMboxes.MBOX16.MSGCTRL.bit.DLC = 8;
	ECanbMboxes.MBOX17.MSGCTRL.bit.DLC = 8;
	ECanbRegs.CANOPC.all = 0xFFFC; //Overwrite Protection

	// setup (enable) the I1EN(Rx), I0EN(Tx), GIL and others on Global interrupt mask reg
	ECanbRegs.CANGIM.all = 0x0;
	ECanbRegs.CANGIM.bit.GIL = 1;
	ECanbRegs.CANGIM.bit.I1EN = 1;
	ECanbRegs.CANMIM.all = 0xFFFFFFFF; //Interrupt enable
	ECanbRegs.CANMIL.all = 0xFFFFFFFF; //interrupt level
// 	ECanbRegs.CANGIM.all = 0x00002E06;
	/*	Configure Acceptance Mask	*/
	ECanbLAMRegs.LAM8.all = 0x00070000; //Don't care
	ECanbLAMRegs.LAM9.all = 0x00070000; //Don't care
	ECanbLAMRegs.LAM10.all = 0x00070000; //Don't care
	ECanbLAMRegs.LAM11.all = 0x00070000; //Don't care
	ECanbLAMRegs.LAM12.all = 0x00070000; //Don't care

	ECanbLAMRegs.LAM13.all = 0x00070000; //Don't care
	ECanbLAMRegs.LAM14.all = 0x00070000; //Don't care
	ECanbLAMRegs.LAM15.all = 0x00070000; //Don't care
	ECanbLAMRegs.LAM16.all = 0x00070000; //Don't care
	ECanbLAMRegs.LAM17.all = 0x00070000; //Don't care

	ECanbRegs.CANMD.all = 0x3FFFC;
	/* re-enable all Mailboxes*/
	ECanbRegs.CANME.all = 0x3FFFF;
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;
	PieCtrlRegs.PIEIER9.bit.INTx8 = 1;
	IER |= 0x0101;
	ECanbRegs.CANGIF1.all & 0x000000FF;
	EDIS;
	CollectPackData();
	ClearReserved();
}


char CANGlobalMaskReceive( unsigned int MASK_num, unsigned int incomingData[],unsigned int dlc)
{
	volatile struct MBOX *Mailbox;
	unsigned long mask;
	struct ECAN_REGS ECanbShadow;
	unsigned int i = 0;
	Mailbox = &ECanbMboxes.MBOX0 + MASK_num;
	mask = (long)(0x01) << MASK_num;

	EALLOW;
	if((ECanaRegs.CANRMP.all  & mask) == 0)
		ECanaRegs.CANRMP.all  &=mask;

	EDIS;
	// Rx the data
	while(i < dlc){
		incomingData[i++] = ((Mailbox->MDL.word.HI_WORD & 0xFF00) >> 8);
		if(i >= dlc)break;
		incomingData[i++] = Mailbox->MDL.word.HI_WORD & 0x00FF;
		if(i >= dlc)break;
		incomingData[i++] = ((Mailbox->MDL.word.LOW_WORD & 0xFF00) >> 8);
		if(i >= dlc)break;
		incomingData[i++] = Mailbox->MDL.word.LOW_WORD & 0x00FF;
		if(i >= dlc)break;
		incomingData[i++] = ((Mailbox->MDH.word.HI_WORD & 0xFF00) >> 8);
		if(i >= dlc)break;
		incomingData[i++] = Mailbox->MDH.word.HI_WORD & 0x00FF;
		if(i >= dlc)break;
		incomingData[i++] = ((Mailbox->MDH.word.LOW_WORD & 0xFF00) >> 8);
		if(i >= dlc)break;
		incomingData[i++] = Mailbox->MDH.word.LOW_WORD & 0x00FF;
	}

	// clear the mailbox RMP bit
	ECanbShadow.CANRMP.all = ECanbRegs.CANRMP.all;
	ECanbShadow.CANRMP.all |= 0xFFFFFFFF;
	ECanbRegs.CANRMP.all=ECanbShadow.CANRMP.all;
	return (0);

}
