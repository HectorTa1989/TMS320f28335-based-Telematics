/*
 * EVDO_DATA.c
 *  Author: Hector Ta
 *  Created on: Dec 2021
 *      
 */
#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File
#include "SD.h"
#include "EVDO.h"
#include <string.h>
#include <stdlib.h>
char AerisArray_MDN[24] = "";
char AerisArray_MIN[25] = "";
extern unsigned long Checksum;
extern unsigned char process_Counter;
extern const char *ATCMD_Ptr;
extern unsigned char VehicleStatus, VehicleFault;
extern unsigned int ModuleAddressCkSUM,CommandCK;
extern unsigned long nBMSLostPacket ;
extern unsigned long nCellLostPacket ;
extern unsigned long nVIMLostPacket ;
extern unsigned char year,month,day,hour,min,sec;
extern unsigned int BMS_Data[];
extern unsigned int Cell_Data[];
extern unsigned int Vehicle_Data[];
extern char MEID_number[];
extern char MDNNumber_Array[];
extern char MINNumber_Array[];

void EVDO_TransmitServer(int Command){
	int length;
	unsigned long temp;
	length = strlen(ATCMD_Ptr)+1;
	//for(i=0;i<length;i++)
//	if(process_Counter ==200)*ATCMD_Ptr++;
    SciaRegs.SCITXBUF = *ATCMD_Ptr; // SH_01
	SciaRegs.SCIFFTX.bit.TXFFINTCLR=1;	// Clear SCI Interrupt flag
	*ATCMD_Ptr++;
	if(length>*ATCMD_Ptr){
		switch(Command){
		case 0://send only checksum
			SciaRegs.SCITXBUF = Command;
			Checksum += Command & 0xFF;
			SciaRegs.SCITXBUF = (Checksum & 0xFF00)>>8;
			SciaRegs.SCITXBUF = (Checksum & 0xFF);
			process_Counter++;

			break;
		case 1:
			SciaRegs.SCITXBUF = Command;
			Checksum += Command & 0xFF;
			SciaRegs.SCITXBUF = (Checksum & 0xFF00)>>8;
			SciaRegs.SCITXBUF = (Checksum & 0xFF);
			process_Counter++;
			break;
		case 2:
			SciaRegs.SCITXBUF = Command;
			Checksum += Command & 0xFF;
			SciaRegs.SCITXBUF = VehicleStatus & 0xFF;
			Checksum += VehicleStatus & 0xFF;
			SciaRegs.SCITXBUF = VehicleFault & 0xFF;
			Checksum += VehicleFault & 0xFF;
			SciaRegs.SCITXBUF = 0x4F;
			Checksum += 0x4F;
			SciaRegs.SCITXBUF = 0x4B;
			Checksum += 0x4B;
			SciaRegs.SCITXBUF = 0x2E;
			Checksum += 0x2E;
			SciaRegs.SCITXBUF = 0x2E;
			Checksum += 0x2E;

			SciaRegs.SCITXBUF = (Checksum & 0xFF00)>>8;
			SciaRegs.SCITXBUF = (Checksum & 0xFF);
			process_Counter++;
			break;

		case 3://BMS Driving
			CommandCK = ModuleAddressCkSUM ;
			SciaRegs.SCITXBUF = Command;
			CommandCK += Command;
			SciaRegs.SCITXBUF = VehicleStatus & 0xFF;
			CommandCK += VehicleStatus & 0xFF;
			SciaRegs.SCITXBUF = VehicleFault & 0xFF;
			CommandCK += VehicleFault & 0xFF;

			SciaRegs.SCITXBUF = nBMSLostPacket & 0xFF;
			CommandCK += nBMSLostPacket & 0xFF;
			SciaRegs.SCITXBUF = (nBMSLostPacket & 0xFF00)>>8;
			CommandCK += (nBMSLostPacket & 0xFF00)>>8;
			SciaRegs.SCITXBUF =(long)(nBMSLostPacket & 0xFF0000)>>16;
			CommandCK += (long)(nBMSLostPacket & 0xFF0000)>>16;
			SciaRegs.SCITXBUF = (long)(nBMSLostPacket & 0xFF000000)>>24;
			if(nBMSLostPacket==0xFFFFFFFF)
				CommandCK += 0xFF;//(long)(nBMSLostPacket & 0xFF000000)>>24;
			else
				CommandCK += 0;

			break;
		case 4://Cell Driving
			CommandCK = ModuleAddressCkSUM ;
			SciaRegs.SCITXBUF = Command;
			CommandCK += Command;
			SciaRegs.SCITXBUF = VehicleStatus & 0xFF;
			CommandCK += VehicleStatus & 0xFF;
			SciaRegs.SCITXBUF = VehicleFault & 0xFF;
			CommandCK += VehicleFault & 0xFF;

			SciaRegs.SCITXBUF = nCellLostPacket & 0xFF;
			CommandCK += nCellLostPacket & 0xFF;
			SciaRegs.SCITXBUF = (nCellLostPacket & 0xFF00)>>8;
			CommandCK += (nCellLostPacket & 0xFF00)>>8;
			SciaRegs.SCITXBUF = (long)(nCellLostPacket & 0xFF0000)>>16;
			CommandCK += (long)(nCellLostPacket & 0xFF0000)>>16;
			SciaRegs.SCITXBUF = (long)(nCellLostPacket & 0xFF000000)>>24;
			if(nCellLostPacket == 0xFFFFFFFF)
				CommandCK += 0xFF;//(long)(nVIMLostPacket & 0xFF000000)>>24;
			else
				CommandCK+= 0;
			break;
		case 5://VIM Driving
			CommandCK = ModuleAddressCkSUM ;
			SciaRegs.SCITXBUF = Command;
			CommandCK += Command;
			SciaRegs.SCITXBUF = VehicleStatus & 0xFF;
			CommandCK += VehicleStatus & 0xFF;
			SciaRegs.SCITXBUF = VehicleFault & 0xFF;
			CommandCK += VehicleFault & 0xFF;
			SciaRegs.SCITXBUF = nVIMLostPacket & 0xFF;
			CommandCK += nVIMLostPacket & 0xFF;

			SciaRegs.SCITXBUF = (nVIMLostPacket & 0xFF00)>>8;
			CommandCK += (nVIMLostPacket & 0xFF00)>>8;
			SciaRegs.SCITXBUF = (long)(nVIMLostPacket & 0xFF0000)>>16;
			CommandCK += (long)(nVIMLostPacket & 0xFF0000)>>16;
			SciaRegs.SCITXBUF = (long)(nVIMLostPacket & 0xFF000000)>>24;
			if(nVIMLostPacket == 0xFFFFFFFF)
				CommandCK += 0xFF;//(long)(nVIMLostPacket & 0xFF000000)>>24;
			else
				CommandCK+= 0;
			break;
		case 6://BMS Charging SD
			CommandCK = ModuleAddressCkSUM ;
			SciaRegs.SCITXBUF = Command;
			CommandCK += Command;
			SciaRegs.SCITXBUF = VehicleStatus & 0xFF;
			CommandCK += VehicleStatus & 0xFF;
			SciaRegs.SCITXBUF = VehicleFault & 0xFF;
			CommandCK += VehicleFault & 0xFF;

			SciaRegs.SCITXBUF = nBMSLostPacket & 0xFF;
			CommandCK += nBMSLostPacket & 0xFF;
			SciaRegs.SCITXBUF = (nBMSLostPacket & 0xFF00)>>8;
			CommandCK += (nBMSLostPacket & 0xFF00)>>8;
			SciaRegs.SCITXBUF = (long)(nBMSLostPacket & 0xFF0000)>>16;
			CommandCK += (long)(nBMSLostPacket & 0xFF0000)>>16;
			SciaRegs.SCITXBUF = (long)(nBMSLostPacket & 0xFF000000)>>24;
			if(nBMSLostPacket==0xFFFFFFFF)
				CommandCK += 0xFF;//(long)(nBMSLostPacket & 0xFF000000)>>24;
			else
				CommandCK += 0;

			break;
		case 7://Cell Charging SD
			CommandCK = ModuleAddressCkSUM ;
			SciaRegs.SCITXBUF = Command;
			CommandCK += Command;
			SciaRegs.SCITXBUF = VehicleStatus & 0xFF;
			CommandCK += VehicleStatus & 0xFF;
			SciaRegs.SCITXBUF = VehicleFault & 0xFF;
			CommandCK += VehicleFault & 0xFF;

			SciaRegs.SCITXBUF = nCellLostPacket & 0xFF;
			CommandCK += nCellLostPacket & 0xFF;
			SciaRegs.SCITXBUF = (nCellLostPacket & 0xFF00)>>8;
			CommandCK += (nCellLostPacket & 0xFF00)>>8;
			SciaRegs.SCITXBUF = (long)(nCellLostPacket & 0xFF0000)>>16;
			CommandCK += (long)(nCellLostPacket & 0xFF0000)>>16;
			SciaRegs.SCITXBUF = (long)(nCellLostPacket & 0xFF000000)>>24;
			temp = (nVIMLostPacket & 0xFF000000)>>24;
			CommandCK += (unsigned int)temp; //(long)(nVIMLostPacket & 0xFF000000)>>24;

			break;
		case 8://VIM Charging SD
			CommandCK = ModuleAddressCkSUM ;
			SciaRegs.SCITXBUF = Command;
			CommandCK += Command;
			SciaRegs.SCITXBUF = VehicleStatus & 0xFF;
			CommandCK += VehicleStatus & 0xFF;
			SciaRegs.SCITXBUF = VehicleFault & 0xFF;
			CommandCK += VehicleFault & 0xFF;

			SciaRegs.SCITXBUF = nVIMLostPacket & 0xFF;
			CommandCK += nVIMLostPacket & 0xFF;
			SciaRegs.SCITXBUF = (nVIMLostPacket & 0xFF00)>>8;
			CommandCK += (nVIMLostPacket & 0xFF00)>>8;
			SciaRegs.SCITXBUF = (long)(nVIMLostPacket & 0xFF0000)>>16;
			CommandCK += (long)(nVIMLostPacket & 0xFF0000)>>16;
			SciaRegs.SCITXBUF = (long)(nVIMLostPacket & 0xFF000000)>>24;
			if(nVIMLostPacket == 0xFFFFFFFF)
				CommandCK += 0xFF;//(unsigned long)(nVIMLostPacket & 0xFF000000)>>24;
			else
				CommandCK+= 0;
			break;
		}
		SciaRegs.SCIFFTX.bit.TXFFINTCLR=1;	// Clear SCI Interrupt flag
	//	SciaRegs.SCITXBUF = 13;

	}
}
void ModemPowerDown(){

}
void Set_Aeris_MDN_Array(){
	int i;
	AerisArray_MDN[0] = 'A' ;
	AerisArray_MDN[1] = 'T' ;
	AerisArray_MDN[2] = '$' ;
	AerisArray_MDN[3] = 'M' ;
	AerisArray_MDN[4] = 'D' ;
	AerisArray_MDN[5] = 'N' ;
	AerisArray_MDN[6] = '=' ;
	for(i = 7; i<13; i++)
		AerisArray_MDN[i] = MEID_number[i+2];
	AerisArray_MDN[13] = ',';
	for(i = 14; i<24; i++)
		AerisArray_MDN[i] = MDNNumber_Array[i-14];
}
void Set_Aeris_MIN_Array(){
	int i;
	AerisArray_MIN[0] = 'A' ;
	AerisArray_MIN[1] = 'T' ;
	AerisArray_MIN[2] = '$' ;
	AerisArray_MIN[3] = 'M' ;
	AerisArray_MIN[4] = 'S' ;
	AerisArray_MIN[5] = 'I' ;
	AerisArray_MIN[6] = 'D' ;
	AerisArray_MIN[7] = '=' ;
	for(i = 8; i<14; i++)
		AerisArray_MIN[i] = MEID_number[i+1];
	AerisArray_MIN[14] = ',';
	for(i = 15; i<25; i++)
		AerisArray_MIN[i] = MINNumber_Array[i-15];


}
void update_Clock()
{
	sec++;
	if(sec>=60){
		sec =0;
		min++;
		if(min>=60){
			hour++;
			min = 0;
			if(hour>=24){
				day++;
				sec = 0;
				min = 0;
				hour = 0;
			}
		}
	}
}
void update_ClockArray(char type){
	if(type == 1){//BMS
		BMS_Data [BMSLastData -8] = year;
		BMS_Data [BMSLastData -7] = month;
		BMS_Data [BMSLastData -6] = day;
		BMS_Data [BMSLastData -5] = hour;
		BMS_Data [BMSLastData -4] = min;
		BMS_Data [BMSLastData -3] = sec;

	}
	else if(type == 2){//Cell
		Cell_Data [CellLastData - 8] = year;
		Cell_Data [CellLastData - 7] = month;
		Cell_Data [CellLastData - 6] = day;
		Cell_Data [CellLastData - 5] = hour;
		Cell_Data [CellLastData - 4] = min;
		Cell_Data [CellLastData - 3] = sec;

	}
	else if (type == 3){//VM
		Vehicle_Data [VehicleLastData - 8] = year;
		Vehicle_Data [VehicleLastData - 7] = month;
		Vehicle_Data [VehicleLastData - 6] = day;
		Vehicle_Data [VehicleLastData - 5] = hour;
		Vehicle_Data [VehicleLastData - 4] = min;
		Vehicle_Data [VehicleLastData - 3] = sec;
	}
}
