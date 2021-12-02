#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File
#include "SD.h"
#include "EVDO.h"

unsigned int d100ms =0;
unsigned int VersionNumber = 001;
// Prototype statements for functions found within this file.
__interrupt void cpu_timer0_isr(void);
__interrupt void cpu_timer1_isr(void);
__interrupt void cpu_timer2_isr(void);
extern unsigned int Vehicle_Data[];
extern unsigned int BMS_Data[],Cell_Data[];
extern unsigned char VehicleStatus,VehicleFault;
extern unsigned char OfflineError;
extern void PutDataBuffer_innerCAN();
extern void PutDataBuffer_VehicleCAN();
extern int Rtos_Data_Manager();
extern void CANRecieve_Test();
extern void Gpio_setup();
extern void InitSciGpio();
extern void InitSciaA();
extern void InitSciaB();
extern void InitEPwm1Example();
extern void InitEPwm1Example2();
extern void EVDO_Process();
extern void ModemReply();
extern void CollectPackData();
extern void update_Clock();
extern void SendTelematicsStatus();
extern char assigned;
extern unsigned char process_Counter ;
extern void uSD_Process();
extern void InitSpi();
extern void InitSpiGpio();
extern union SDSTATUS sdstatus;
extern unsigned long nBMSLostPacket ;
extern unsigned long nCellLostPacket ;
extern unsigned long nVIMLostPacket ;
extern void update_ClockArray();
extern void ModemReset();
extern void  get_GPS_Data();
extern void SendModemStatus();
//extern unsigned char gps_hour,gps_min,gps_sec;
extern unsigned char hour,min,sec;
extern unsigned int TempSD[];
extern union MODEMSTATUS modemstatus;
void main(void)
{
   Uint16 i;

// Step 1. Initialize System Control:
// PLL, WatchDog, enable Peripheral Clocks
// This example function is found in the DSP2833x_	SysCtrl.c file.
   InitSysCtrl();

// Step 2. Initialize GPIO:
// This example function is found in the DSP2833x_Gpio.c file and
// illustrates how to set the GPIO to it's default state.
// InitGpio();
// Setup only the GP I/O only for SCI-A and SCI-B functionality
// This function is found in DSP2833x_Sci.c
   InitSciGpio();
   Gpio_setup();
   InitSpiGpio();//SDCard
// Step 3. Clear all interrupts and initialize PIE vector table:
// Disable CPU interrupts
   DINT;

// Initialize PIE control registers to their default state.
// The default state is all PIE interrupts disabled and flags
// are cleared.
// This function is found in the DSP2833x_PieCtrl.c file.
   InitPieCtrl();

// Disable CPU interrupts and clear all CPU interrupt flags:
   IER = 0x0000;
   IFR = 0x0000;

// Initialize the PIE vector table with pointers to the shell Interrupt
// Service Routines (ISR).
// This will populate the entire table, even if the interrupt
// is not used in this example.  This is useful for debug purposes.
// The shell ISR routines are found in DSP2833x_DefaultIsr.c.
// This function is found in DSP2833x_PieVect.c.
   InitPieVectTable();

// Interrupts that are used in this example are re-mapped to
// ISR functions found within this file.
   EALLOW;	// This is needed to write to EALLOW protected registers
   PieVectTable.TINT0 = &cpu_timer0_isr;
   PieVectTable.XINT13 = &cpu_timer1_isr;
   PieVectTable.ECAN1INTA = &ECAN1INTA_ISR;
   PieVectTable.ECAN1INTB = &ECAN1INTB_ISR;
   EDIS;   // This is needed to disable write to EALLOW protected registers

   // Step 4. Initialize the Device Peripheral. This function can be
   //         found in DSP2833x_CpuTimers.c
   InitCpuTimers();   // For this example, only initialize the Cpu Timers

// Configure CPU-Timer 0, 1, and 2 to interrupt every second:
// 150MHz CPU Freq, 1 second Period (in uSeconds)

   ConfigCpuTimer(&CpuTimer0, 150, 100000);//100ms
   ConfigCpuTimer(&CpuTimer1, 150, 1000);//1ms
//   ConfigCpuTimer(&CpuTimer2, 150, 100);//100us


#if (CPU_FRQ_100MHZ)
// Configure CPU-Timer 0, 1, and 2 to interrupt every second:
// 100MHz CPU Freq, 1 second Period (in uSeconds)

   ConfigCpuTimer(&CpuTimer0, 100, 1000000);
   ConfigCpuTimer(&CpuTimer1, 100, 1000000);
   ConfigCpuTimer(&CpuTimer2, 100, 1000000);
#endif
// To ensure precise timing, use write-only instructions to write to the entire register. Therefore, if any
// of the configuration bits are changed in ConfigCpuTimer and InitCpuTimers (in DSP2833x_CpuTimers.h), the
// below settings must also be updated.

   CpuTimer0Regs.TCR.all = 0x4000; // Use write-only instruction to set TSS bit = 0
   CpuTimer1Regs.TCR.all = 0x4000; // Use write-only instruction to set TSS bit = 0
//   CpuTimer2Regs.TCR.all = 0x4000; // Use write-only instruction to set TSS bit = 0

// Step 5. User specific code, enable interrupts:

// Enable CPU int1 which is connected to CPU-Timer 0, CPU int13
// which is connected to CPU-Timer 1, and CPU int 14, which is connected
// to CPU-Timer 2:
   IER |= M_INT1;
   IER |= M_INT13;
   IER |= 0x100;
//   IER |= M_INT13;
//   IER |= M_INT14;

// Enable TINT0 in the PIE: Group 1 interrupt 7
//   GpioDataRegs.GPCSET.bit.GPIO82 = 0;   // Load output latch
//   GpioDataRegs.GPCSET.bit.GPIO83 = 0;   // Load output latch

// InitPeripherals(); // Not required for this example

   InitECana();//Init ECANa
   InitECanb();//Init ECANb
   InitSciaA();//Init Scia Modem
   InitSciaB();//Init Scia GPS
   /*********************SPI**********************/
   InitSpi();//SDCard
  
   /*********************************************/
// Step 5. User specific code, enable interrupts:

// Enable interrupts required for this example
   PieCtrlRegs.PIECTRL.bit.ENPIE = 1;   // Enable the PIE block
   PieCtrlRegs.PIEIER1.bit.INTx7 = 1;
   PieCtrlRegs.PIEIER9.bit.INTx6 = 1;     // PIE Group 9, INT6
   PieCtrlRegs.PIEIER9.bit.INTx8 = 1;     // PIE Group 9, INT8
   assigned = 0;
   ModemReset(); // reset Modem active low
   VehicleFault = 0;
   sdstatus.all = 0; //clear sdstatus
   modemstatus.all = 0;
   for(i= 0; i<344; i++){
	   BMS_Data[i] = 0;
	   Cell_Data[i] = 0;
	   Vehicle_Data[i] = 0;
   }
   EINT;
// Step 6. IDLE loop. Just sit and loop forever (optional):
   while(1){
  /////VehicleStatus(Driving or Charging)
	   if(Vehicle_Data[22] == 0x1)
		   VehicleStatus = Driving;
	   else if(Vehicle_Data[22] == 0x2)
		   VehicleStatus = Charging;
	   else if (Vehicle_Data[22] == 0x4)
		   VehicleStatus = PowerDown;
	   /////VehicleFault TRUE or FALSE///
	   if(Vehicle_Data[17] == 0x3)
		   VehicleFault = 1;
	   else
		   VehicleFault = 0;
	}
}
void Offline_Management(){
	static int i,j,k = 0;
	//100ms loop
	//start storing data to sd right after losing network //02262016SH
	if((VehicleStatus == Driving)&&(OfflineError>=OffLineError)&&(!VehicleFault)){
		i++;
		j++;
		k++;
		if(i >= 6){
			sdstatus.bit.bit2_Write_BMS = 1;
			nBMSLostPacket++;
			update_ClockArray(1);
			i = 0;
		}
		if(j >= 5){
			sdstatus.bit.bit3_Write_Cell = 1;
			nCellLostPacket++;
			update_ClockArray(2);
			j = 0;
		}
		if(k >= 2){
			sdstatus.bit.bit4_Write_VIM = 1;
			nVIMLostPacket++;
			update_ClockArray(3);
			k = 0;
		}
	}
	else if ((VehicleStatus == Driving)&&(OfflineError>=OffLineError)&&(VehicleFault)){
		i++;
		j++;
		k++;
		if(i >= 2){
			sdstatus.bit.bit2_Write_BMS = 1;
			nBMSLostPacket++;
			update_ClockArray(1);
			i = 0;
		}
		if(j >= 2){
			sdstatus.bit.bit3_Write_Cell = 1;
			nCellLostPacket++;
			update_ClockArray(2);
			j = 0;
		}
		if(k >= 2){
			sdstatus.bit.bit4_Write_VIM = 1;
			nVIMLostPacket++;
			update_ClockArray(3);
			k = 0;
		}
	}
	else if ((VehicleStatus == Charging)&&(OfflineError>=OffLineError)&&(!VehicleFault)){
		i++;
		j++;
		k++;
		if(i >= 60){
			sdstatus.bit.bit2_Write_BMS = 1;
			nBMSLostPacket++;
			update_ClockArray(1);
			i = 0;
		}
		if(j >= 60){
			sdstatus.bit.bit3_Write_Cell = 1;
			nCellLostPacket++;
			update_ClockArray(2);
			j = 0;
		}
		if(k >= 60){
			sdstatus.bit.bit4_Write_VIM = 1;
			nVIMLostPacket++;
			update_ClockArray(3);
			k = 0;
		}
	}
	else if ((VehicleStatus == Charging)&&(OfflineError>=OffLineError)&&(VehicleFault)){
		i++;
		j++;
		k++;
		if(i >= 2){
			sdstatus.bit.bit2_Write_BMS = 1;
			nBMSLostPacket++;
			update_ClockArray(1);
			i = 0;
		}
		if(j >= 2){
			sdstatus.bit.bit3_Write_Cell = 1;
			nCellLostPacket++;
			update_ClockArray(2);
			j = 0;
		}
		if(k >= 2){
			sdstatus.bit.bit4_Write_VIM = 1;
			nVIMLostPacket++;
			update_ClockArray(3);
			k = 0;
		}
	}
	else{
		i = j = k = 0;
		sdstatus.bit.bit2_Write_BMS = 0;
		sdstatus.bit.bit3_Write_Cell = 0;
		sdstatus.bit.bit4_Write_VIM = 0;
	}
}
void TestTimingLoop(){
	if(GpioDataRegs.GPBDAT.bit.GPIO32)
		GpioDataRegs.GPBCLEAR.bit.GPIO32 = 1;
	else
		GpioDataRegs.GPBSET.bit.GPIO32 = 1;
}
void d1secLoop(){
	update_Clock();
	Offline_Management();
    CollectPackData();
    SendModemStatus();
}
void d100msLoop(){
	EVDO_Process();

	d100ms++;
	if(d100ms>=10){
		d1secLoop();
		d100ms = 0 ;
	}
}
void d1msLoop(){
	get_GPS_Data();
	ModemReply();
	uSD_Process();
	SendTelematicsStatus();
}
__interrupt void cpu_timer0_isr(void)
{
   CpuTimer0.InterruptCount++;
   d100msLoop();
   // Acknowledge this interrupt to receive more interrupts from group 1
   PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

__interrupt void cpu_timer1_isr(void)
{
   CpuTimer1.InterruptCount++;
   // The CPU acknowledges the interrupt.
   d1msLoop();
}
