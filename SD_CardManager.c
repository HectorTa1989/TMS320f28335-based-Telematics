/*
 * SD_Card.c
 *  Author: Hector Ta
 *  Created on: Dec 2021
 *      
 */

#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File
#include "SD.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

unsigned int SD_process = 0;
unsigned int LostPacket[6];
unsigned int TempSD[344];
unsigned int *Buffer;

void CheckSDErr();

extern Uint16 spi_xmit_byte();
extern void update_ClockArray();
extern void sd_data_response();
extern void sd_data_response_Init();
extern void ModemPowerDown();
extern void ModemReset();
extern union SDSTATUS sdstatus;
extern unsigned int Cell_Data[],BMS_Data[];
extern unsigned int Vehicle_Data[];
extern unsigned long nBMSLostPacket ;
extern unsigned long nCellLostPacket ;
extern unsigned long nVIMLostPacket ;
extern unsigned char VehicleStatus, VehicleFault;
extern Uint16 SD_ErrorCount;
extern char Transmit_ID[];
extern unsigned int TelematicsID[];
extern void my_reverse(char str[], int len);
extern char* my_itoa(unsigned int num, char* str, int base);
/**********************************************************************************************/
/*																							  */
/* Author: Sung Han																			  */
/*****************code Start*******************************************************************/

void uSD_Process(){
	Uint16 i ;
	char tempchar;
	unsigned int ID;
	Uint16 temp[1];
	switch(SD_process){
		case POWERUPINIT:
			sd_card_insertion();
			CheckSDErr();
			break;
		case CARDINIT:
			sd_initialization();
			CheckSDErr();
			break;
		case READ_REGISTER1:
			sd_read_register(SEND_CSD);
			CheckSDErr();
			break;
		case READ_REGISTER2:
			sd_read_register(READ_OCR);
			CheckSDErr();
			break;
		case READ_REGISTER3:
			sd_read_register(SEND_CID);
			CheckSDErr();
			break;
		case READLOSTPACKET:
			sd_read_block(SECTOR_ZERO_LOSTPACKET,LostPacket);
			nBMSLostPacket = LostPacket[1] | LostPacket[0]>>8;
			nCellLostPacket = LostPacket[3] | LostPacket[2]>>8;
			nVIMLostPacket = LostPacket[5] | LostPacket[4]>>8;
			SD_process++;
			break;
		case READMEIDNUMBER:
			sd_read_block(SECTOR_TELEMATICSID,TelematicsID);
			ID = (TelematicsID[0]&0xFF00) >>8;
			sd_read_block(SECTOR_TELEMATICSID+2,TelematicsID);
			ID |= (TelematicsID[0]);
			my_itoa(ID,Transmit_ID,10);
			tempchar = Transmit_ID[3];
			for(i=0;i<3;i++)
				Transmit_ID[i+3] = Transmit_ID[i];
			Transmit_ID[6] = tempchar;
			Transmit_ID[0] = 'M';
			Transmit_ID[1] = 'P';
			Transmit_ID[2] = 'N';
			SD_process = CARDACTIVE;
			break;
		case CARDACTIVE:
			sdstatus.bit.bit1_InitNotComplete =0;
			if((( sdstatus.all & 0x1E0 )== 0x00)&&(Vehicle_Data[22]== 0x05))
				SD_process = STORELOSTPACKET;
			if(sdstatus.bit.bit2_Write_BMS){
				update_ClockArray(1);
				sd_write_block(SECTOR_ZERO_BMS+nBMSLostPacket, BMS_Data);
				SD_process = WRITING_BMS;
				sdstatus.bit.bit2_Write_BMS = 0;
				sdstatus.bit.bit5_WritingBlock = 1;
			}
			else if(sdstatus.bit.bit3_Write_Cell){
				update_ClockArray(2);
				sd_write_block(SECTOR_ZERO_CELL+nCellLostPacket, Cell_Data);
				SD_process = WRITING_CELL;
				sdstatus.bit.bit3_Write_Cell = 0;
				sdstatus.bit.bit5_WritingBlock = 1;
			}
			else if(sdstatus.bit.bit4_Write_VIM){
				update_ClockArray(3);
				sd_write_block(SECTOR_ZERO_VIM+nVIMLostPacket, Vehicle_Data);
				SD_process = WRITING_VIM;
				sdstatus.bit.bit4_Write_VIM = 0;
				sdstatus.bit.bit5_WritingBlock = 1;
			}

			if(sdstatus.bit.bit6_Read_BMS){
				sd_read_block(SECTOR_ZERO_BMS+nBMSLostPacket, TempSD);
				sd_erase_block(SECTOR_ZERO_BMS+nBMSLostPacket, 1);
			//	SD_process = READING;
				if(nBMSLostPacket != 0xFFFFFFFF)
					nBMSLostPacket--;
			}
			else if(sdstatus.bit.bit7_Read_Cell){
				sd_read_block(SECTOR_ZERO_CELL+nCellLostPacket, TempSD);
				sd_erase_block(SECTOR_ZERO_CELL+nCellLostPacket,1);
			//	SD_process = READING;
				if(nCellLostPacket != 0xFFFFFFFF)
					nCellLostPacket--;
			}
			else if(sdstatus.bit.bit8_Read_VIM){
				sd_read_block(SECTOR_ZERO_VIM+nVIMLostPacket, TempSD);
				sd_erase_block(SECTOR_ZERO_VIM+nVIMLostPacket,1);
			//	SD_process = READING;
				if(nVIMLostPacket != 0xFFFFFFFF)
					nVIMLostPacket--;
			}
			break;
		case WRITING_BMS:
			sd_write_data(BMS_Data, SINGLE_SECTOR, 0);
			break;
		case WRITING_CELL:
			sd_write_data(Cell_Data, SINGLE_SECTOR, 0);
			break;
		case WRITING_VIM:
			sd_write_data(Vehicle_Data, SINGLE_SECTOR, 0);
			break;
		case READING:
			//Call sd_data_response to read specified sector
		//	sd_data_response(TempSD, SINGLE_SECTOR);
			break;
		case STORELOSTPACKET:
			if(nBMSLostPacket == 0xFFFFFFFF)nBMSLostPacket = 0;
			if(nCellLostPacket == 0xFFFFFFFF)nCellLostPacket = 0;
			if(nVIMLostPacket == 0xFFFFFFFF)nVIMLostPacket = 0;
			LostPacket[0] = nBMSLostPacket & 0xFF;
			LostPacket[1] = (nBMSLostPacket & 0xFF00)>>8;
			LostPacket[2] = nCellLostPacket & 0xFF;
			LostPacket[3] = (nCellLostPacket & 0xFF00)>>8;
			LostPacket[4] = nVIMLostPacket & 0xFF;
			LostPacket[5] = (nVIMLostPacket & 0xFF00)>>8;
			sd_write_block_PwrDown(SECTOR_ZERO_LOSTPACKET, LostPacket);
			SD_process = POWERDOWN;
			break;
		case STORETELEMATICID:
			temp[0] = TelematicsID[0] & 0xFF;
			sd_write_block(SECTOR_TELEMATICSID,temp);
			sd_write_data(temp,SINGLE_SECTOR,0);
			temp[0]= (TelematicsID[0]&0xFF00)>>8;
			sd_write_block(SECTOR_TELEMATICSID+2,temp);
			sd_write_data(temp,SINGLE_SECTOR,0);
			sdstatus.bit.bit13_IDstoredSuccess = 1;
			break;
		case POWERDOWN:
			ModemReset();
			ModemPowerDown();
			for(i = 0 ;i<100; i++);
			//initiate shutdown itself GPIO.set?
			break;
	}
}
void CheckSDErr(){
	SD_ErrorCount++;
	if(SD_ErrorCount >= SDERR_CNT){
		SD_process = 0;
		SD_ErrorCount = 0;
	}
}
