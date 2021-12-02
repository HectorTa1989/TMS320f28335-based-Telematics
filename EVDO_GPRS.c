/*
 * EVDO_GPRS.c
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

#define READY 1
#define Server_CMD 2
#define ERRORMAX 1200 //2min

unsigned char year,month,day,hour,min,sec;
/***************************AT COMMAND******************************************/
const char *ATCMD_Ptr;
const char ATCMD_ATE[] = "ATE0";
const char ATCMD_ContextActivation[] = "AT#SGACT=1,1";
const char ATCMD_SocketDial[] = "AT#SD=1,0,10001,\"100.9.99.58\",0,0,0";
const char ATCMD_SocketStatus[] = "AT#SS";
const char ATCMD_SocketConfiguration[] = "AT#SCFG=1,1,450,180,600,10";//180sec timeout
const char ATCMD_GPSTurnON[] = "AT$GPSP=1";//ok
const char ATCMD_GPSData[] = "GPSACP";
const char ATCMD_CSQ[] = "AT+CSQ";
const char ATCMD_Reset[] = "AT$RESET";
const char ATCMD_Clock[] = "AT+CCLK?";
const char ATCMD_SocketClose[] = "";

char Transmit_ID[8] = "MPN9999" ;//Default /*sd card store*/
unsigned int Transmit_ID16[4];
char MDNNumber_Array[] = "0000000000";
char MINNumber_Array[] = "0000000000";
const char ATTCP_SERV[] = "AT#TCPSERV=1,\"100.9.99.58\"";
const char ATTCP_PORT[] = "AT#TCPPORT=1,10001";
const char ATTCP_ContextActivation[] = "AT#CONNECTIONSTART";
const char ATTCP_Clock[] = "AT#NITZ";
const char ATSuspend[] = "+++";
const char ATCMD_SocketRestore[] = "AT#SO=1";
//const char ATS25[] = "ATS25=0";
//const char PING[] = "AT#PING=google.com,4,32,600,128";
/*Activation*/
const char ATRegister[] = "AT+CREG?";
const char ATActivate[] = "AT+CDV*22899;";
const char ATMEID[] =	"AT#MEID?";
const char ATMSID[] = "AT$MSID?";
const char ATMDN[] = "AT$MDN=";
const char ATCNUM[] = "AT+CNUM";
/********************************************************************************/

char GPSModem = 0;

const char OK = 1;
const char Connect = 2;
const char GPSACCP = 3;
const char CSQ 	= 4;
const char GPSOK = 5;
const char Clock_Reply = 6;
const char ReturnOK = 7;
const char NOCARRIER = 8;
const char Meid = 9;
const char CNUM = 10;

unsigned char reset_counter = 200;
unsigned char error_counter = 0;
unsigned char OfflineError = 0;
unsigned char process_Counter = 0;
unsigned char ReplybufferC = 0;
char ReplyModemBuffer[40];
char MEID_number[15];
char PhoneNum[10];
char *FilterReply;
unsigned char assigned = 0;
unsigned char DATA_Counter = 0;
unsigned long Checksum =0;
unsigned int BMS_Data[BMSLastData];
unsigned int Cell_Data[CellLastData];
unsigned int Vehicle_Data[VehicleLastData];
unsigned char VehicleStatus ,VehicleFault;
unsigned int ModuleAddressCkSUM,CommandCK = 0;
unsigned long nBMSLostPacket = 0;
unsigned long nCellLostPacket = 0;
unsigned long nVIMLostPacket = 0;
unsigned long Calculate_ChkSUM(const char *Start, unsigned int length);
extern unsigned char array_set;
extern void SCI_Reset();
extern unsigned int TempSD[];
extern char AerisArray_MDN[];
extern char AerisArray_MIN[];
extern unsigned int TelematicsID[];
extern void Set_Aeris_MDN_Array();
extern void Set_Aeris_MIN_Array();
extern void uSD_Process();
extern void update_ClockArray();
extern void EVDO_TransmitServer();
union MODEMSTATUS modemstatus;
union SDSTATUS sdstatus;
/**********************************************************************************************/
/*																							  */
/* Author: Sung Han																			  */
/*****************code Start*******************************************************************/

void ModemReset(){
	int i;
	for(i=0;i<5;i++){
		GpioDataRegs.GPACLEAR.bit.GPIO2 = 1;
	}
	GpioDataRegs.GPASET.bit.GPIO2  =1;

}
unsigned long Calculate_ChkSUM(const char *Start, unsigned int length){
  int    i;
  unsigned long sum = 0;

  for (i = 0; i < length; i++){
	  sum += *Start++;
  }
  if(process_Counter == Transmit)
		ModuleAddressCkSUM= sum;
  return sum;
}

void DeleteBufferArray(){
	int i;
	for(i=0;i<30;i++){
		ReplyModemBuffer[i]  = 0 ;
	}

}
void EVDO_Transmit(){
	int length;
	length = strlen(ATCMD_Ptr);
	SciaRegs.SCITXBUF = *ATCMD_Ptr; // SH_01
	SciaRegs.SCIFFTX.bit.TXFFINTCLR=1;	// Clear SCI Interrupt flag
	*ATCMD_Ptr++;
	if(length>*ATCMD_Ptr){
		SciaRegs.SCITXBUF = 13;
		process_Counter++;
	}
}
void EVDO_EnterSuspend(){
	int length;
	length = strlen(ATCMD_Ptr);
	SciaRegs.SCITXBUF = *ATCMD_Ptr; // SH_01
	SciaRegs.SCIFFTX.bit.TXFFINTCLR=1;	// Clear SCI Interrupt flag
	*ATCMD_Ptr++;
	if(length>*ATCMD_Ptr)process_Counter++;
}
void ModemReply(){
	unsigned int temp;
	int j;
	if((SciaRegs.SCIFFRX.bit.RXFFST >= 1)){
		while(SciaRegs.SCIFFRX.bit.RXFFST > 0){
			temp = SciaRegs.SCIRXBUF.bit.RXDT;
			ReplyModemBuffer[ReplybufferC] = temp;
			ReplybufferC++;
		}
	SciaRegs.SCIFFRX.all = 0x6040;		// RX FIFO clear & reset
	for(j=0;j<5;j++){}
	SciaRegs.SCIFFRX.all = 0x2040;		// RX FIFO enabled
	}
}
void ModemReply_Process(char ReplyCMD){
	int temp,i;
	assigned = 0;
	FilterReply = strstr(ReplyModemBuffer, "ERROR " );
	if(FilterReply){
		process_Counter = Reset;
		return;
	}
	switch(ReplyCMD){
		case 1://OK
			FilterReply = strstr(ReplyModemBuffer, "OK");
			if(FilterReply){
				if(process_Counter == SuspendMode_Reply )
					process_Counter= ATE;
				else
					process_Counter++;
				ReplybufferC = 0;
				DeleteBufferArray();
			}
			else{
				error_counter++;
				if((error_counter>=100)&&(process_Counter != SuspendMode_Reply)){
					if(process_Counter == ATE)process_Counter++;
					else{
						process_Counter = Reset;
						error_counter = 0;
					}
				}
			}
			break;
		case 2://CONNECT
			FilterReply = strstr(ReplyModemBuffer, "CONNECT");
			if(FilterReply){
				process_Counter = 54;
				ReplybufferC = 0;
				DeleteBufferArray();
			}
			else{
				error_counter++;
				if(error_counter>=100){
					process_Counter = Reset;
					error_counter = 0;
				}
			}
			break;
		case 3://GPSACCP
			FilterReply = strstr(ReplyModemBuffer, "$GPSACP");
			if(FilterReply){
				process_Counter++;
				ReplybufferC = 0;
				DeleteBufferArray();
			}
			else{
				error_counter++;
				if(error_counter>=100){
					process_Counter = Reset;
					error_counter = 0;
				}
			}
			break;
		case 4://CSQ
			FilterReply = strstr(ReplyModemBuffer, "+CSQ");
			if(FilterReply){
				*FilterReply ++;
				*FilterReply ++;
				*FilterReply ++;
				*FilterReply ++;
				*FilterReply ++;
				temp =	 *FilterReply++;
				if((temp<99)&&(temp>0))
					process_Counter++;
				else
					process_Counter = 0;
				ReplybufferC = 0;
				DeleteBufferArray();
			}
			else{
				error_counter++;
				if(error_counter>=100){
					process_Counter = Reset;
					error_counter = 0;
				}
			}
			break;
		case 5://GPSOK
			FilterReply = strstr(ReplyModemBuffer, "STATUS");
			if(FilterReply){
				process_Counter++;
				ReplybufferC = 0;
				DeleteBufferArray();

			}
			else{
				process_Counter++;
				ReplybufferC = 0;
				DeleteBufferArray();
			}
			break;
		case 6://CCLK
			FilterReply = strstr(ReplyModemBuffer, "+CCLK");
			if(FilterReply){
				*FilterReply ++ ;
				*FilterReply ++ ;
				*FilterReply ++ ;
				*FilterReply ++ ;
				*FilterReply ++ ;
				*FilterReply ++ ;
				*FilterReply ++ ;
				*FilterReply ++ ;
				year =atoi(FilterReply);
				*FilterReply ++ ;
				*FilterReply ++ ;
				*FilterReply ++ ;
				month =atoi(FilterReply);
				*FilterReply ++ ;
				*FilterReply ++ ;
				*FilterReply ++ ;
				day =atoi(FilterReply);
				*FilterReply ++ ;
				*FilterReply ++ ;
				*FilterReply ++ ;
				hour =atoi(FilterReply);
				*FilterReply++;
				*FilterReply++;
				*FilterReply++;
				min =atoi(FilterReply);
				*FilterReply++;
				*FilterReply++;
				*FilterReply++;
				sec =atoi(FilterReply);
			}
			process_Counter++;
			break;
		case 7://ReturnOK
			FilterReply = strstr(ReplyModemBuffer, "OK");
			if(FilterReply){
				process_Counter = Reply_CMD;
				ReplybufferC = 0;
				DeleteBufferArray();
			}
			else{
				error_counter++;
				if(error_counter>=100){
					process_Counter = Reset;
					error_counter = 0;
				}
			}
			break;
		case 8://NOCARRIER
			FilterReply = strstr(ReplyModemBuffer, "NO");
			if(FilterReply){
				process_Counter = ACTIVATIONOK;
				ReplybufferC = 0;
				DeleteBufferArray();
			}
			else{
				error_counter++;
				if(error_counter>=100){
					process_Counter = ACTIVATIONFAIL;
					error_counter = 0;
				}
			}
			break;
		case 9://MEIDHEX
			FilterReply = strstr(ReplyModemBuffer, "A10000,");
			if(FilterReply){
				for(i = 0; i<15; i++)
					MEID_number[i]= *FilterReply++;
				if(modemstatus.bit.bit4_Verizon)
					process_Counter = REGISTERVERIZON;
				else
					process_Counter = REGISTERAERIS_MDN;
				ReplybufferC = 0;
				DeleteBufferArray();
			}
			break;
		case 10://CNUM
			FilterReply = strstr(ReplyModemBuffer, "CNUM");
			if(FilterReply){
				for(i =0; i<10;i++)
					PhoneNum[i] = ReplyModemBuffer[i+20];
				process_Counter++;
			}
	}
}
void ServerReply_Process(char ReplyCMD){
	static unsigned long temp = 0;
	static unsigned int PacketID,i;
	int j=0;
	FilterReply = strstr(ReplyModemBuffer, "NO CARRIER" );
	if(FilterReply){
		assigned = 0;
		process_Counter = Reset;
		error_counter = 0;
		return;
	}
	switch(ReplyCMD){
    	case READY://READY
    		FilterReply = strstr(ReplyModemBuffer, "READY");
			if(FilterReply){
				assigned = 0;
				process_Counter = Transmit2;
				ReplybufferC = 0;
				DeleteBufferArray();
				OfflineError = 0;
			}
			else{//	else error
				error_counter++;
				if(error_counter>=(ERRORMAX-60)){//120sec
					ModemReset();
					assigned = 0;
					process_Counter = ATE;
					error_counter = 0;
				}
			}
		break;
    	case Server_CMD://CMD
    		if((ReplyModemBuffer[0] == 0x53)&&(ReplyModemBuffer[1] == Transmit_ID [1])&&(ReplyModemBuffer[2] == Transmit_ID [2])&&(ReplyModemBuffer[3] == Transmit_ID [3])&&(ReplyModemBuffer[4] == Transmit_ID [4])&&(ReplyModemBuffer[5] == Transmit_ID [5])&&(ReplyModemBuffer[6] == Transmit_ID [6])){
				PacketID = ReplyModemBuffer[7];
				ATCMD_Ptr =Transmit_ID;
    		}
    		else {
   				DeleteBufferArray();
   				PacketID = 0;
   				i = 0;
   				ReplybufferC = 0;
   				array_set = 0;
   				temp = 0;
    		}
			if(PacketID == 0x3){//BMS
				error_counter = 0;
				temp =0;
				for(j = 0; j<7; j++){
					EVDO_TransmitServer(PacketID);
				}
				for(i = 0; i<(BMSLastData);){
					if((!GpioDataRegs.GPADAT.bit.GPIO12)&&(SciaRegs.SCICTL2.bit.TXRDY)){
						if(i < (BMSLastData -8)){//data
							temp += (BMS_Data[i] & 0xFF);
							SciaRegs.SCITXBUF = BMS_Data[i];
							SciaRegs.SCIFFTX.bit.TXFFINTCLR=1;	// Clear SCI Interrupt flag
							i++;
						}
						else if(i < (BMSLastData -2)){//Clock
							update_ClockArray(1);
							temp += (BMS_Data[i] & 0xFF);
							SciaRegs.SCITXBUF = BMS_Data[i];
							SciaRegs.SCIFFTX.bit.TXFFINTCLR=1;	// Clear SCI Interrupt flag
							i++;
						}
						else{//checksum
							temp += CommandCK;
							CommandCK=0;
							assigned = 0;
							BMS_Data[(BMSLastData - 2)] = ((unsigned long)(temp & 0xFF00))>>8;
							BMS_Data[(BMSLastData - 1)] = ((unsigned long)(temp & 0xFF));
							SciaRegs.SCITXBUF = BMS_Data[i];
							SciaRegs.SCIFFTX.bit.TXFFINTCLR=1;	// Clear SCI Interrupt flag
							i++;
						}
					}
				}
				if(i == (BMSLastData)){
					DeleteBufferArray();
					PacketID = 0;
					i = 0;
					ReplybufferC = 0;
					array_set = 0;
					temp = 0;
				}
			}
			else if (PacketID == 0x4){//Cell
				temp =0;
				error_counter =0;
				for(j = 0; j<7; j++){
					EVDO_TransmitServer(PacketID);
				}
				for(i = 0 ; i<(CellLastData); ){
					if((!GpioDataRegs.GPADAT.bit.GPIO12)&&(SciaRegs.SCICTL2.bit.TXRDY)){
						if(i < (CellLastData - 8)){//data
							temp += (Cell_Data[i] &  0xFF);
							SciaRegs.SCITXBUF = Cell_Data[i];
							SciaRegs.SCIFFTX.bit.TXFFINTCLR=1;	// Clear SCI Interrupt flag
							i++;
						}
						else if(i < (CellLastData - 2)){//Clock
							update_ClockArray(2);
							temp += (Cell_Data[i] &  0xFF);
							SciaRegs.SCITXBUF = Cell_Data[i];
							SciaRegs.SCIFFTX.bit.TXFFINTCLR=1;	// Clear SCI Interrupt flag
							i++;

						}
						else{//checksum
							temp += CommandCK;
							CommandCK=0;
							assigned = 0;
							Cell_Data[(CellLastData - 2)] = ((unsigned long)(temp & 0xFF00))>>8;
							Cell_Data[(CellLastData - 1)] = ((unsigned long)(temp & 0xFF));
							SciaRegs.SCITXBUF = Cell_Data[i];
							SciaRegs.SCIFFTX.bit.TXFFINTCLR=1;	// Clear SCI Interrupt flag
							i++;
						}
					}
				}
				if(i == (CellLastData)){
					DeleteBufferArray();
					PacketID = 0;
					i = 0;
					ReplybufferC = 0;
					temp = 0;
					array_set = 0;
				}
			}
			else if (PacketID == 0x5){//VIM
				error_counter =0;
				temp =0;
				for(j = 0; j<7; j++){
					EVDO_TransmitServer(PacketID);
				}
				for(i =0; i<(VehicleLastData);){
					if((!GpioDataRegs.GPADAT.bit.GPIO12)&&(SciaRegs.SCICTL2.bit.TXRDY)){
						if(i < (VehicleLastData - 8)){//data
							temp += (Vehicle_Data[i] & 0xFF);
							SciaRegs.SCITXBUF = Vehicle_Data[i];
							SciaRegs.SCIFFTX.bit.TXFFINTCLR=1;	// Clear SCI Interrupt flag
							i++;
						}
						else if(i < (VehicleLastData - 2)){//Clock
							update_ClockArray(3);
							temp += (Vehicle_Data[i] & 0xFF);
							SciaRegs.SCITXBUF = Vehicle_Data[i];
							SciaRegs.SCIFFTX.bit.TXFFINTCLR=1;	// Clear SCI Interrupt flag
							i++;
						}
						else{//checksum
							temp += CommandCK;
							CommandCK=0;
							assigned = 0;
							Vehicle_Data[(VehicleLastData - 2)] = ((unsigned long)(temp & 0xFF00))>>8;
							Vehicle_Data[(VehicleLastData - 1)] = ((unsigned long)(temp & 0xFF));
							SciaRegs.SCITXBUF = Vehicle_Data[i];
							SciaRegs.SCIFFTX.bit.TXFFINTCLR=1;	// Clear SCI Interrupt flag
							i++;
						}
					}
				}
				if(i == (VehicleLastData)){
					DeleteBufferArray();
					PacketID = 0;
					i = 0;
					ReplybufferC = 0;
					temp = 0;
					array_set = 0;
				}
			}
			/*************************SD CARD RECALL FUNCTION *********************************************/
			else if(PacketID == 0x6){//BMS
				error_counter = 0;
				sdstatus.bit.bit6_Read_BMS = 1;
				temp= 0;
				for(j = 0; j<7; j++){
					EVDO_TransmitServer(PacketID);
				}
				uSD_Process();
				for(i = 0; i<(BMSLastData);){
					if((!GpioDataRegs.GPADAT.bit.GPIO12)&&(SciaRegs.SCICTL2.bit.TXRDY)){
						if(i < (BMSLastData - 2)){
							temp += ((TempSD[i] & 0xFF00)>>8);
							SciaRegs.SCITXBUF = TempSD[i]>>8;
							SciaRegs.SCIFFTX.bit.TXFFINTCLR=1;	// Clear SCI Interrupt flag
							i++;
						}
						else{
							temp += CommandCK;
							CommandCK=0;
							TempSD[(BMSLastData - 2)] = ((unsigned long)(temp & 0xFF00))>>8;
							TempSD[(BMSLastData - 1)] = ((unsigned long)(temp & 0xFF));
							SciaRegs.SCITXBUF = TempSD[i];
							SciaRegs.SCIFFTX.bit.TXFFINTCLR=1;	// Clear SCI Interrupt flag
							i++;
						}
					}
				}
				if(i == (BMSLastData)){
					DeleteBufferArray();
					PacketID = 0;
					i = 0;
					ReplybufferC = 0;
					array_set = 0;
					sdstatus.bit.bit6_Read_BMS = 0;
				}
			}
			else if (PacketID == 0x7){//Cell
				error_counter =0;
				sdstatus.bit.bit7_Read_Cell = 1;
				temp  = 0;
				for(j = 0; j<7; j++){
					EVDO_TransmitServer(PacketID);
				}
				uSD_Process();
				for(i = 0; i<(CellLastData);){
					if((!GpioDataRegs.GPADAT.bit.GPIO12)&&(SciaRegs.SCICTL2.bit.TXRDY)){
						if(i < (CellLastData - 2)){
							temp += ((TempSD[i] & 0xFF00)>>8);
							SciaRegs.SCITXBUF = TempSD[i]>>8;
							SciaRegs.SCIFFTX.bit.TXFFINTCLR=1;	// Clear SCI Interrupt flag
							i++;
						}
						else{
							temp += CommandCK;
							CommandCK=0;
							TempSD[(CellLastData - 2)] = ((unsigned long)(temp & 0xFF00))>>8;
							TempSD[(CellLastData - 1)] = ((unsigned long)(temp & 0xFF));
							SciaRegs.SCITXBUF = TempSD[i];
							SciaRegs.SCIFFTX.bit.TXFFINTCLR=1;	// Clear SCI Interrupt flag
							i++;
						}
					}
				}
				if(i == (CellLastData)){
					DeleteBufferArray();
					PacketID = 0;
					i = 0;
					ReplybufferC = 0;
					array_set = 0;
					sdstatus.bit.bit7_Read_Cell = 0;
				}
			}
			else if (PacketID == 0x8){//VIM
				sdstatus.bit.bit8_Read_VIM = 1;
				error_counter =0;
				temp = 0;
				for(j = 0; j<7; j++){
					EVDO_TransmitServer(PacketID);
				}
				uSD_Process();
				for(i = 0; i<(VehicleLastData);){
					if((!GpioDataRegs.GPADAT.bit.GPIO12)&&(SciaRegs.SCICTL2.bit.TXRDY)){
						if(i < (VehicleLastData - 2)){
							temp += ((TempSD[i] & 0xFF00)>>8);
							SciaRegs.SCITXBUF = TempSD[i]>>8;
							SciaRegs.SCIFFTX.bit.TXFFINTCLR=1;	// Clear SCI Interrupt flag
							i++;
						}
						else{
							temp += CommandCK;
							CommandCK=0;
							TempSD[(VehicleLastData - 2)] = ((unsigned long)(temp & 0xFF00))>>8;
							TempSD[(VehicleLastData - 1)] = ((unsigned long)(temp & 0xFF));
							SciaRegs.SCITXBUF = TempSD[i];
							SciaRegs.SCIFFTX.bit.TXFFINTCLR=1;	// Clear SCI Interrupt flag
							i++;
						}
					}
				}
				if(i == (VehicleLastData)){
					DeleteBufferArray();
					PacketID = 0;
					i = 0;
					ReplybufferC = 0;
					temp = 0;
					array_set = 0;
					sdstatus.bit.bit8_Read_VIM = 0;
				}
			}

			else{//	else error
				error_counter++;
				if(error_counter>=ERRORMAX){//120sec

					ModemReset();
					assigned = 0;
					process_Counter = ATE;
					error_counter = 0;
				}
			}
    	break;
		}
//	if(!FilterReply)error_cnt++;
//	else error_cnt = 0 ;
//	if(error_cnt >= 1000)process_Counter = Reset;
}

void EVDO_Process()
{
	switch(process_Counter)
	{
		case ATE://0
			if(!assigned){
				DeleteBufferArray();
				ATCMD_Ptr = ATCMD_ATE;
				assigned = 1;
				ReplybufferC =  0;
				FilterReply = 0;
			}
			EVDO_Transmit();
			break;
		case Reply0://1
			ModemReply_Process(OK);
			break;
		case SIGNALSTRENGTH://2
			if(!assigned){
				DeleteBufferArray();
				ATCMD_Ptr = ATCMD_CSQ;
				assigned = 1;
				ReplybufferC =  0;
				FilterReply = 0;
			}
			EVDO_Transmit();
			break;
		case Reply1://3
			ModemReply_Process(CSQ);
			break;
		case ClockManagement://4
			if(!assigned){
				DeleteBufferArray();
				ATCMD_Ptr = ATCMD_Clock;
				assigned = 1;
				ReplybufferC = 0;
				FilterReply = 0;
			}
			EVDO_Transmit();
			break;
		case ClockManagement_Reply://5
			ModemReply_Process(Clock_Reply);
			break;
		case SocketConfiguration://6
			if(!assigned){
				DeleteBufferArray();
				ATCMD_Ptr = ATCMD_SocketConfiguration;
				assigned = 1;
				ReplybufferC = 0;
				FilterReply = 0;
			}
			EVDO_Transmit();
			break;
		case Reply2://7
			ModemReply_Process(OK);
			break;
		case GPSTurnON://8
			if(!assigned){
				DeleteBufferArray();
				ATCMD_Ptr = ATCMD_GPSTurnON;
				assigned = 1;
				ReplybufferC = 0;
				FilterReply = 0;
			}
			EVDO_Transmit();
			break;
		case GPS_Reply: //9
			ModemReply_Process(GPSOK);
			break;
		case ContextActivation://10
			if(!assigned){
				DeleteBufferArray();
				ATCMD_Ptr = ATCMD_ContextActivation;
				assigned = 1;
				ReplybufferC =  0;
				FilterReply = 0;
			}
			EVDO_Transmit();
			break;
		case Reply3://11
			ModemReply_Process(OK);
			break;
		case SocketDial://12
			if(!assigned){
				DeleteBufferArray();
				ATCMD_Ptr = ATCMD_SocketDial;
				assigned = 1;
				ReplybufferC = 0;
				FilterReply = 0;
			}
			EVDO_Transmit();
			break;
		case Reply4://13
			ModemReply_Process(Connect);
			break;
		/* Init Done*/

		/* Transmitting Data */
		case Transmit://54
			modemstatus.bit.bit2_ModemActivated = 1;
			if(!assigned){
				DeleteBufferArray();
				ATCMD_Ptr = Transmit_ID;
				assigned = 1;
				ReplybufferC = 0;
				FilterReply = 0;
				Checksum = Calculate_ChkSUM(ATCMD_Ptr,strlen(ATCMD_Ptr));
			}
			EVDO_TransmitServer(1);
			break;
		case Reply55:
			ServerReply_Process(READY);
			ATCMD_Ptr = Transmit_ID;
			DeleteBufferArray();
		//	ATCMD_Ptr = Transmit_OK;
			assigned = 1;
			ReplybufferC = 0;
			FilterReply = 0;
			Checksum = Calculate_ChkSUM(ATCMD_Ptr,strlen(ATCMD_Ptr));
			break;
		case Transmit2://200
			if(!assigned){
				DeleteBufferArray();
			//	ATCMD_Ptr = Transmit_OK;
				assigned = 1;
				ReplybufferC = 0;
				FilterReply = 0;
				Checksum = Calculate_ChkSUM(ATCMD_Ptr,strlen(ATCMD_Ptr));
			}
			EVDO_TransmitServer(2);
			break;

		case Reset://100
			SCI_Reset();
			if(!assigned){
				DeleteBufferArray();
				ATCMD_Ptr = ATCMD_Reset;
				assigned = 1;
				ReplybufferC = 0;
				FilterReply = 0;
				ModemReset();
			}
			EVDO_Transmit();
			break;
		case Reset2:
			reset_counter--;
	        if(!reset_counter){
            	SCI_Reset();
            	if(modemstatus.bit.bit3_ModemConfiguring){
            		process_Counter = MEID;
            		modemstatus.bit.bit3_ModemConfiguring = 0;
            	}
            	else if(modemstatus.bit.bit6_ModemRecall){
            		process_Counter = MODEMRECALL;
            		modemstatus.bit.bit6_ModemRecall = 0;
            	}
            	else
            		process_Counter = ATE;
				reset_counter=100;
				assigned = 0;
				OfflineError++;
			}
			break;
/********************************Server Handshake Completed*************************************/
		case Reply_CMD://201
			modemstatus.bit.bit1_ConnectedServer = 1;
			ServerReply_Process(Server_CMD);
			break;
/********************************SuspendMode***************************************************/
		case SuspendMode: //250
			if(!assigned){
				DeleteBufferArray();
				ATCMD_Ptr = ATSuspend;
				assigned = 1;
				ReplybufferC = 0;
				FilterReply = 0;
			}
			EVDO_EnterSuspend();
			break;
		case SuspendMode_Reply: //251
			ModemReply_Process(OK);
			break;
		case GPSData:
			if(!assigned){
				DeleteBufferArray();
				ATCMD_Ptr = ATCMD_GPSData;
				assigned = 1;
				ReplybufferC = 0;
				FilterReply = 0;
			}
			EVDO_Transmit();
			break;
		case GPSData_Reply:
			ModemReply_Process(GPSACCP);
			break;
		case RestoreSocket://300
			if(!assigned){
				DeleteBufferArray();
				ATCMD_Ptr = ATCMD_SocketRestore;
				assigned = 1;
				ReplybufferC = 0;
				FilterReply = 0;
			}
			EVDO_Transmit();
			break;
		case ReturnToTransmit://301
			ModemReply_Process(ReturnOK);
			break;
	/*****************************Activation START******************/

		case MEID://398
		if(!assigned){
			DeleteBufferArray();
			ATCMD_Ptr = ATMEID ;
			assigned = 1;
			ReplybufferC =  0;
			FilterReply = 0;
		}
		EVDO_Transmit();
		break;
		case MEIDReply://399
			ModemReply_Process(Meid);
		break;
	/*****************************Verizon Activation******************/
		case REGISTERVERIZON://400
			if(!assigned){
			DeleteBufferArray();
			ATCMD_Ptr = ATRegister;
			assigned = 1;
			ReplybufferC =  0;
			FilterReply = 0;
		}
		EVDO_Transmit();
		break;
		case REGISTERVERIZONREPLY://401
			ModemReply_Process(OK);
		break;
		case ACTIVATIONVERIZON://402
			if(!assigned){
			DeleteBufferArray();
			ATCMD_Ptr = ATActivate;
			assigned = 1;
			ReplybufferC =  0;
			FilterReply = 0;
		}
		EVDO_Transmit();
		break;
		case ACTIVATIONVERIZONREPLY://403
			ModemReply_Process(NOCARRIER);
			modemstatus.bit.bit2_ModemActivated = 1;
		//	process_Counter = 0;
			//ModemReset();
		break;
	/*****************************Aeris Activation******************/
		case REGISTERAERIS_MDN://450
			Set_Aeris_MDN_Array();
			if(!assigned){
				DeleteBufferArray();
				ATCMD_Ptr = AerisArray_MDN;
				assigned = 1;
				ReplybufferC =  0;
				FilterReply = 0;
			}
			EVDO_Transmit();
		break;
		case REGISTERAERIS_REPLY://451
			ModemReply_Process(OK);
		break;
		case REGISTERAERIS_MIN://452
			Set_Aeris_MIN_Array();
			if(!assigned){
				DeleteBufferArray();
				ATCMD_Ptr = AerisArray_MDN;
				assigned = 1;
				ReplybufferC =  0;
				FilterReply = 0;
			}
			EVDO_Transmit();
		break;
		case REGISTERAERIS_REPLY2://453
			ModemReply_Process(OK);
			modemstatus.bit.bit2_ModemActivated = 1;
		break;
		/**************************MODEM RECALL*********************/
		case MODEMRECALL://460
			if(!assigned){
			DeleteBufferArray();
			ATCMD_Ptr = ATCNUM;
			assigned = 1;
			ReplybufferC =  0;
			FilterReply = 0;
		}
		EVDO_Transmit();
		break;
		case MODEMRECALL_REPLY://461
			ModemReply_Process(CNUM);
		break;
		case MODEMRECALL1://462
		if(!assigned){
			DeleteBufferArray();
			ATCMD_Ptr = ATMEID ;
			assigned = 1;
			ReplybufferC =  0;
			FilterReply = 0;
		}
		EVDO_Transmit();
		break;
		case MODEMRECALL_REPLY1://463
			ModemReply_Process(Meid);
			modemstatus.bit.bit7_SendTelematicON = 1;
		break;

		case ACTIVATIONOK://498
			modemstatus.bit.bit2_ModemActivated = 1;
			modemstatus.bit.bit8_ReceivedSuccessful = 1;
			process_Counter = HOLD;

		break;
		case ACTIVATIONFAIL://499

		break;
		case HOLD://500



		//case Register_A:

	}
}
