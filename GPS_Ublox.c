/*
 * GPS_Ublox.c
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
extern unsigned int Vehicle_Data[];
signed char GPS_Data[50];

unsigned char gpsIndex= 0;
unsigned char gps_hour,gps_min,gps_sec;
void get_GPS_Data(){
	unsigned char temp;
	signed char *FilterReply;
	unsigned long temp32;
	if((ScibRegs.SCIFFRX.bit.RXFFST >= 1)){
		while(ScibRegs.SCIFFRX.bit.RXFFST > 0){
			temp = ScibRegs.SCIRXBUF.bit.RXDT;
			if (temp == '$')
				gpsIndex = 1;
			else if ((gpsIndex==1)&&(temp== 'G'))
				GPS_Data[gpsIndex++]=temp;
			else if ((gpsIndex==2)&&(temp== 'P'))
				GPS_Data[gpsIndex++]=temp;
			else if ((gpsIndex==3)&&(temp== 'R'))
				GPS_Data[gpsIndex++]=temp;
			else if ((gpsIndex==4)&&(temp== 'M'))
				GPS_Data[gpsIndex++]= temp;
			else if ((gpsIndex==5)&&(temp== 'C')) {
				GPS_Data[gpsIndex++]= temp;

			}
			else if ((gpsIndex>=6)&&(gpsIndex<66))
				GPS_Data[gpsIndex++]=temp;
			else
				gpsIndex = 0;
			FilterReply = &GPS_Data[9];
			temp32 = atol(FilterReply);
			gps_min = (int)(temp32 & 0xFF00)>>8;
			gps_sec = (temp32 & 0xFF);

			if(GPS_Data [17]=='A'){
				Vehicle_Data[67] = 'A';
				/******************Latitude*************/
				FilterReply = &GPS_Data[19];
				temp32 =atol(FilterReply);
				Vehicle_Data[57] = (temp32 & 0xFF00)>>8;
				Vehicle_Data[56] = (temp32 & 0xFF);
				FilterReply = &GPS_Data[24];
				temp32 =atol(FilterReply);
				Vehicle_Data[60] = (temp32 &0xFF0000)>>16;
				Vehicle_Data[59] = (temp32 &0xFF00)>>8;
				Vehicle_Data[58] = (temp32 & 0xFF);
				Vehicle_Data[55] = GPS_Data[30];
				/******************Longitude************/
				FilterReply = &GPS_Data[32];
				Vehicle_Data[61] = GPS_Data[44];
				temp32 =atol(FilterReply);
				Vehicle_Data[63] = (temp32 & 0xFF00)>>8;
				Vehicle_Data[62] = (temp32 & 0xFF);
				FilterReply = &GPS_Data[38];
				temp32 =atol(FilterReply);
				Vehicle_Data[66] = (temp32 & 0xFF0000)>>16;
				Vehicle_Data[65] = (temp32 & 0xFF00)>>8;
				Vehicle_Data[64] = (temp32 & 0xFF);
			}
			else Vehicle_Data[67] = 'V'; //GPS Status
		}
	ScibRegs.SCIFFRX.all = 0x6040;		// RX FIFO clear & reset
	ScibRegs.SCIFFRX.all = 0x2040;		// RX FIFO enabled
	}
}
