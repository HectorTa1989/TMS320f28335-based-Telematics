/*
 * ECan_Transmit.c
 *  Author: Hector Ta
 *  Created on: Dec 2021
 *      
 */
#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File
#include "GlobalVariables.h"
#include "EVDO.h"
#include "SD.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#define TELEMATICSIDSET 10
#define TELEMATICSMDNSET 11
#define TELEMATICSMINSET 12
#define TELEMATICSRECALL 20
#define TELEMATICSSTARTACTIVE 15
#define TELEMATICSCONNECTOSERVER   21
#define TELEMATICSRECEIVEDRX  22

#define TELEMATICSIDRX 30
#define TEMATICSPHONENUMRXL 31
#define TEMATICSPHONENUMRXH 32

#define TELEMATICSMEIDNUMRXL 33
#define TELEMATICSMEIDNUMRXH 34

#define TELEMATICSMSIDNUMRXL 35
#define TELEMATICSMSIDNUMRXH 36
#define TELEMATICSMINNUMRXL 37
#define TELEMATICSMINNUMRXH 38

#define TELEMATICSSIGNALRX 39
#define TELEMATICSMODEMSTATUSRX 40
#define TELEMATICSVEHICLESTATUSRX 41
#define	TELEMATICSSDSTATUSRX 42

#define TELEMATICSOFTWAREVERSION 50
#define TELEMATICSSTORESDID 52
unsigned int array_set = 0;
unsigned int TelematicsID[1];
extern char MDNNumber_Array[];
extern char MINNumber_Array[];
extern void ModemReset();
extern unsigned int VersionNumber;
extern unsigned char process_Counter;
extern char DATA_Counter;
extern unsigned int BMS_Data[];
extern unsigned int Cell_Data[];
extern unsigned int Vehicle_Data[];
extern char MEID_number[];
extern char Transmit_ID[];
extern char PhoneNum[];
extern char CANGlobalMaskReceive( unsigned int MASK_num, unsigned int incomingData[], unsigned int dlc);
extern unsigned char PacketID;
extern union MODEMSTATUS modemstatus;
void ReceivingCalibration(void);
void CollectPackData();
void ClearReserved();
/*
* function to reverse a string
*/
void my_reverse(char str[], int len)
{
    int start, end;
    char temp;
    for(start=0, end=len-1; start < end; start++, end--) {
        temp = *(str+start);
        *(str+start) = *(str+end);
        *(str+end) = temp;
    }
}

char* my_itoa(unsigned int num, char* str, int base)
{
    int i = 0;
    char isNegative = 0;

    /* A zero is same "0" string in all base */
 /*   if (num == 0) {
        str[i] = '0';
        str[i + 1] = '\0';
        return str;
    }
*/
    /* negative numbers are only handled if base is 10
       otherwise considered unsigned number */
 /*   if (num < 0 && base == 10) {
        isNegative = 1;
        num = -num;
    }
*/
    while (i<4) {
        int rem = num % base;
        str[i++] = (rem > 9)? (rem-10) + 'A' : rem + '0';
        num = num/base;
    }

    /* Append negative sign for negative numbers */
    if (isNegative){
        str[i++] = '-';
    }

    str[i] = '\0';

    my_reverse(str, i);

    return str;
}
void PutDataBuffer_VehicleCAN(){
	Uint16 MASK;
	int i;
	unsigned int ID;
	unsigned long long MDNNumber = 0;
	unsigned long long MINNumber = 0;
	char tempchar;
	struct ECAN_REGS ECanaShadow;
	EALLOW;
	MASK = ECanaRegs.CANGIF1.all & 0xFF ;
	EDIS;
	if (MASK == 0x0) {//Odometer //0x21
		// clear the mailbox RMP bit
		Vehicle_Data[7] = ECanaMboxes.MBOX0.MDH.byte.BYTE4; //Mileage
		Vehicle_Data[6] = ECanaMboxes.MBOX0.MDH.byte.BYTE5; //Mileage
		Vehicle_Data[5] = ECanaMboxes.MBOX0.MDH.byte.BYTE6; //Mileage
		Vehicle_Data[4] = ECanaMboxes.MBOX0.MDH.byte.BYTE7; //Mileage
	}
	else if (MASK == 0x01) {//Accessory //0x20
		// clear the mailbox RMP bit
		Vehicle_Data[30] = ECanaMboxes.MBOX1.MDL.byte.BYTE0; //Key Status
		Vehicle_Data[2] = ECanaMboxes.MBOX1.MDL.byte.BYTE1; //AccelPedal
		Vehicle_Data[3] = ECanaMboxes.MBOX1.MDL.byte.BYTE2; //Brake
		Vehicle_Data[16] = ECanaMboxes.MBOX1.MDL.byte.BYTE3; //VehicleSpeed
		Vehicle_Data[17] = ECanaMboxes.MBOX1.MDH.byte.BYTE5; //VehicleStatus
		Vehicle_Data[22] = ECanaMboxes.MBOX1.MDH.byte.BYTE6;  //OperatingMode
		Vehicle_Data[23] = ECanaMboxes.MBOX1.MDH.byte.BYTE7; //Gear Position
		// clear the mailbox RMP bit

	}
	else if (MASK == 0x02) {//MotorInfo //0x30
		// clear the mailbox RMP bit
		Vehicle_Data[19] = ECanaMboxes.MBOX2.MDL.byte.BYTE0;//Trq
		Vehicle_Data[18] = ECanaMboxes.MBOX2.MDL.byte.BYTE1;//Trq
		Vehicle_Data[21] = ECanaMboxes.MBOX2.MDL.byte.BYTE2;//MotorSpeed
        Vehicle_Data[20] = ECanaMboxes.MBOX2.MDL.byte.BYTE3;//MotorSpeed
		Vehicle_Data[15] = ECanaMboxes.MBOX2.MDH.byte.BYTE4; //SvcnMotTemp
		Vehicle_Data[14] = ECanaMboxes.MBOX2.MDH.byte.BYTE5; //SvcnInverterTemp
		Vehicle_Data[13] = ECanaMboxes.MBOX2.MDH.byte.BYTE6; //SvcnCurrent
		Vehicle_Data[12] = ECanaMboxes.MBOX2.MDH.byte.BYTE7; //SvcnCurrent
	}
	else if (MASK == 0x03) {//BMSFailCode //0x622
		// clear the mailbox RMP bit
		Vehicle_Data[35] = ECanaMboxes.MBOX3.MDH.byte.BYTE4;//BMSFaultCode

	}
	else if (MASK == 0x04) {//12v //0x25
		// clear the mailbox RMP bit
		Vehicle_Data[1] = ECanaMboxes.MBOX4.MDL.byte.BYTE0; //12v
		Vehicle_Data[0] = ECanaMboxes.MBOX4.MDL.byte.BYTE1; //12v

	}
	else if (MASK == 0x05) {//BMSState //0x610
		// clear the mailbox RMP bit
	   // Vehicle_Data[35] = ECanaMboxes.MBOX5.MDL.byte.BYTE0; //BMSChargerState
		Vehicle_Data[33] = ECanaMboxes.MBOX5.MDL.byte.BYTE1; //BMSDiag
		Vehicle_Data[32] = ECanaMboxes.MBOX5.MDL.byte.BYTE2; //BMSDiag
		Vehicle_Data[31] = ECanaMboxes.MBOX5.MDL.byte.BYTE3;//Contactor State
		if(Vehicle_Data[31] == DRIVE){
			Vehicle_Data[8] = 0; //ChargeTargetCurrent
			Vehicle_Data[11] = 0; //ChargeTargetVoltage
			Vehicle_Data[10] = 0; //ChargeTargetVoltage
		}
	}
	else if (MASK == 0x06) {//AC ChargeState,Diag//0x41
		// clear the mailbox RMP bit
		Vehicle_Data[25] = ECanaMboxes.MBOX6.MDL.byte.BYTE0; //ACChargeState
		Vehicle_Data[39] = ECanaMboxes.MBOX6.MDL.byte.BYTE1; //ACChargeDiag
		Vehicle_Data[38] = ECanaMboxes.MBOX6.MDL.byte.BYTE2; //ACChargeDiag
		if(Vehicle_Data[31]==ACChrg){
			Vehicle_Data[8] = ECanaMboxes.MBOX6.MDL.byte.BYTE3; //ChargeTargetCurrent
			Vehicle_Data[11] = ECanaMboxes.MBOX6.MDH.byte.BYTE4; //ChargeTargetVoltage
			Vehicle_Data[10] = ECanaMboxes.MBOX6.MDH.byte.BYTE5; //ChargeTargetVoltage
			/*		Vehicle_Data[8] = ECanaMboxes.MBOX6.MDL.byte.BYTE3; //ChargeTargetCurrent
			temp16 = ((ECanaMboxes.MBOX6.MDH.byte.BYTE4 & 0xFF )<<8 + (ECanaMboxes.MBOX6.MDH.byte.BYTE5 &0xFF))/10;
			Vehicle_Data[11] = (temp16 & 0xFF00)>>8; //ChargeTargetVoltage
			Vehicle_Data[10] = temp16 & 0xFF; //ChargeTargetVoltage*/
		}
	}
	else if (MASK == 0x07) {//J1172 State //0x42
		// clear the mailbox RMP bit
	    Vehicle_Data[28] = ECanaMboxes.MBOX7.MDL.byte.BYTE0; //J1772State
	    Vehicle_Data[45] = ECanaMboxes.MBOX7.MDL.byte.BYTE1; //J1772Diag
		Vehicle_Data[44] = ECanaMboxes.MBOX7.MDL.byte.BYTE2; //J1772Diag
		Vehicle_Data[52] = ECanaMboxes.MBOX7.MDL.byte.BYTE3; //J1772PilotPWM
		Vehicle_Data[54] = ECanaMboxes.MBOX7.MDH.byte.BYTE4; //J1772Prox
		Vehicle_Data[53] = ECanaMboxes.MBOX7.MDH.byte.BYTE5; //J1772Prox
	}
	else if (MASK == 0x08) {//CHAdeMO State //0x105
		// clear the mailbox RMP bit
		Vehicle_Data[26] = ECanaMboxes.MBOX8.MDL.byte.BYTE0; //CHAdeMOState
		Vehicle_Data[41] = ECanaMboxes.MBOX8.MDL.byte.BYTE1; //CHAdeMODiag
	}
	else if (MASK == 0x09) {//CTC State //0x605
		// clear the mailbox RMP bit
		Vehicle_Data[27] = ECanaMboxes.MBOX9.MDL.byte.BYTE0; //CTCState
		Vehicle_Data[42] = ECanaMboxes.MBOX9.MDL.byte.BYTE1; //CTCDiag
		Vehicle_Data[43] = ECanaMboxes.MBOX9.MDL.byte.BYTE2; //CTCDiag
	}
	else if(MASK == 0x0A){//MCUState //0X31
	   Vehicle_Data[29] = ECanaMboxes.MBOX10.MDL.byte.BYTE0; //MCUState
	   Vehicle_Data[49] = ECanaMboxes.MBOX10.MDL.byte.BYTE1; //MCUDiag
	   Vehicle_Data[48] = ECanaMboxes.MBOX10.MDL.byte.BYTE2; //MCUDiag
	   Vehicle_Data[51] = ECanaMboxes.MBOX10.MDL.byte.BYTE3; //MCUFailCode
	   Vehicle_Data[50] = ECanaMboxes.MBOX10.MDH.byte.BYTE4; //MCUFailCode
	}
	else if (MASK == 0x0B) {//DCDCState //0x200
		// clear the mailbox RMP bit
		Vehicle_Data[36] = ECanaMboxes.MBOX11.MDL.byte.BYTE0; //DCDCDiag
	}
	else if (MASK == 0x0C) {//DCACState //0x210
		// clear the mailbox RMP bit
		Vehicle_Data[37] = ECanaMboxes.MBOX12.MDL.byte.BYTE0; //DCACDiag
	}
	else if (MASK == 0x0D) {//Transmission //0x300
		// clear the mailbox RMP bit
		Vehicle_Data[46] = ECanaMboxes.MBOX13.MDL.byte.BYTE1; //TransmDiag
	}
	else if (MASK == 0x0E) {//ACCompState //0x440
		// clear the mailbox RMP bit
		Vehicle_Data[47] = ECanaMboxes.MBOX14.MDL.byte.BYTE0; //ACCompDiag
	}
	else if (MASK == 0x0F) {//CHAdeMO //0x102
		if(Vehicle_Data[31]==CHAdeMO){
			Vehicle_Data[8] = ECanaMboxes.MBOX15.MDL.byte.BYTE3; //ChargeTargetCurrent
			Vehicle_Data[11] = ECanaMboxes.MBOX15.MDL.byte.BYTE2; //ChargeTargetVoltage
			Vehicle_Data[10] = ECanaMboxes.MBOX15.MDL.byte.BYTE1; //ChargeTargetVoltage
		}
	}
	else if (MASK == 0x10){//CTC //0x601
		if(Vehicle_Data[31]==CTC){
			Vehicle_Data[8] = ECanaMboxes.MBOX16.MDH.byte.BYTE5; //ChargeTargetCurrent
			Vehicle_Data[11] = ECanaMboxes.MBOX16.MDH.byte.BYTE7; //ChargeTargetVoltage
			Vehicle_Data[10] = ECanaMboxes.MBOX16.MDH.byte.BYTE6; //ChargeTargetVoltage
		}
	}
	else if (MASK == 0x13){//RX Cmd
		if(ECanaMboxes.MBOX19.MDL.byte.BYTE0 == TELEMATICSIDSET){
			modemstatus.bit.bit3_ModemConfiguring = 1;
			process_Counter = 100;
			TelematicsID[0] = ECanaMboxes.MBOX19.MDL.byte.BYTE1;
			TelematicsID[0] |=ECanaMboxes.MBOX19.MDL.byte.BYTE2 <<8;
			ID = TelematicsID[0];
			if(ECanaMboxes.MBOX19.MDL.byte.BYTE3 == 0x01)
				modemstatus.bit.bit4_Verizon = 1;
			else if(ECanaMboxes.MBOX19.MDL.byte.BYTE3 == 0x02)
				modemstatus.bit.bit4_Verizon = 0;
			my_itoa(ID,Transmit_ID,10);
			tempchar = Transmit_ID[3];
			for(i=0;i<3;i++)
				Transmit_ID[i+3] = Transmit_ID[i];
			Transmit_ID[6] = tempchar;
			Transmit_ID[0] = 'M';
			Transmit_ID[1] = 'P';
			Transmit_ID[2] = 'N';
			SD_process = STORETELEMATICID;
		}
		else if(ECanaMboxes.MBOX19.MDL.byte.BYTE0 == TELEMATICSMDNSET){
			MDNNumber = ECanaMboxes.MBOX19.MDL.byte.BYTE1;
			MDNNumber |=(long)ECanaMboxes.MBOX19.MDL.byte.BYTE2 <<8;
			MDNNumber |=(long)ECanaMboxes.MBOX19.MDL.byte.BYTE3 <<16;
			MDNNumber |=(long)ECanaMboxes.MBOX19.MDH.byte.BYTE4 <<24;
			MDNNumber |=((long long)ECanaMboxes.MBOX19.MDH.byte.BYTE5)<<32 ;
			my_itoa(MDNNumber,MDNNumber_Array,10);
		}
		else if(ECanaMboxes.MBOX19.MDL.byte.BYTE0 == TELEMATICSMINSET){
			MINNumber = ECanaMboxes.MBOX19.MDL.byte.BYTE1;
			MINNumber |=(long)ECanaMboxes.MBOX19.MDL.byte.BYTE2 <<8;
			MINNumber |=(long)ECanaMboxes.MBOX19.MDL.byte.BYTE3 <<16;
			MINNumber |=(long)ECanaMboxes.MBOX19.MDH.byte.BYTE4 <<24;
			MINNumber |=((long long)ECanaMboxes.MBOX19.MDH.byte.BYTE5) <<32 ;
			my_itoa(MINNumber,MINNumber_Array,10);
		}
		else if(ECanaMboxes.MBOX19.MDL.byte.BYTE0 == TELEMATICSRECALL){
			process_Counter = 100;
			modemstatus.bit.bit6_ModemRecall = 1;
		}
		else if(ECanaMboxes.MBOX19.MDL.byte.BYTE0 == TELEMATICSCONNECTOSERVER){
			modemstatus.bit.bit3_ModemConfiguring = 0;
			modemstatus.bit.bit6_ModemRecall = 0;
			process_Counter = 100;
		}
	}
	else if (MASK == 0x14){//RX Data

	}

	ECanaShadow.CANRMP.all = ECanaRegs.CANRMP.all;
	ECanaShadow.CANRMP.all |= 0xFFFFFFFF;
	ECanaRegs.CANRMP.all = ECanaShadow.CANRMP.all;

}
void PutDataBuffer_innerCAN() {
	Uint16 MASK = 0;
	struct ECAN_REGS ECanbShadow;
	EALLOW;
	MASK = ECanbRegs.CANGIF1.bit.MIV1 ;
	EDIS;
	if (MASK == 0x00){
		ECanbShadow.CANTA.all = 0;
		ECanbShadow.CANTA.bit.TA0 = 1;
		ECanbRegs.CANTA.all = ECanbShadow.CANTA.all;
	}
	else if (MASK == 0x01){
		ECanbShadow.CANTA.all = 0;
		ECanbShadow.CANTA.bit.TA1 = 1;
		ECanbRegs.CANTA.all = ECanbShadow.CANTA.all;
	}
	else if (MASK == 0x02) {//(PackVoltage/BusVoltage/Current)
		// clear the mailbox RMP bit
		BMS_Data[0] = ECanbMboxes.MBOX2.MDL.byte.BYTE0; //PackV
		BMS_Data[1] = ECanbMboxes.MBOX2.MDL.byte.BYTE1; //PackV
		BMS_Data[2] = ECanbMboxes.MBOX2.MDL.byte.BYTE2; //BusV
		BMS_Data[3] = ECanbMboxes.MBOX2.MDL.byte.BYTE3; //BusV
		BMS_Data[4] = ECanbMboxes.MBOX2.MDH.byte.BYTE4; //BusCurrent
		BMS_Data[5] = ECanbMboxes.MBOX2.MDH.byte.BYTE5; //BusCurrent
		BMS_Data[6] = ECanbMboxes.MBOX2.MDH.byte.BYTE6; //BusCurrent
		BMS_Data[7] = ECanbMboxes.MBOX2.MDH.byte.BYTE7; //BusCurrent
		// clear the mailbox RMP bit

	}
	else if(MASK == 0x03){//(Residual/Rate Capacity)
		BMS_Data[8] = ECanbMboxes.MBOX3.MDL.byte.BYTE0; //Residual Capacity
		BMS_Data[9] = ECanbMboxes.MBOX3.MDL.byte.BYTE1; //Residual Capacity
		BMS_Data[10] = ECanbMboxes.MBOX3.MDL.byte.BYTE2; //Residual Capacity
		BMS_Data[11] = ECanbMboxes.MBOX3.MDL.byte.BYTE3; //Residual Capacity
		BMS_Data[12] = ECanbMboxes.MBOX3.MDH.byte.BYTE4; //Rated Capacity
		BMS_Data[13] = ECanbMboxes.MBOX3.MDH.byte.BYTE5; //Rated Capacity
		BMS_Data[14] = ECanbMboxes.MBOX3.MDH.byte.BYTE6; //Rated Capacity
		BMS_Data[15] = ECanbMboxes.MBOX3.MDH.byte.BYTE7; //Rated Capacity
		// clear the mailbox RMP bit
	}
	else if(MASK == 0x04){//(Accumulated Discharge/Charge Capacity)
		BMS_Data[16] = ECanbMboxes.MBOX4.MDL.byte.BYTE0; //Accumulated Discharge Capacity
		BMS_Data[17] = ECanbMboxes.MBOX4.MDL.byte.BYTE1; //Accumulated Discharge Capacity
		BMS_Data[18] = ECanbMboxes.MBOX4.MDL.byte.BYTE2; //Accumulated Discharge Capacity
		BMS_Data[19] = ECanbMboxes.MBOX4.MDL.byte.BYTE3; //Accumulated Discharge Capacity
		BMS_Data[20] = ECanbMboxes.MBOX4.MDH.byte.BYTE4; //Accumulated Charge Capacity
		BMS_Data[21] = ECanbMboxes.MBOX4.MDH.byte.BYTE5; //Accumulated Charge Capacity
		BMS_Data[22] = ECanbMboxes.MBOX4.MDH.byte.BYTE6; //Accumulated Charge Capacity
		BMS_Data[23] = ECanbMboxes.MBOX4.MDH.byte.BYTE7; //Accumulated Charge Capacity
		// clear the mailbox RMP bit
	}
	else if(MASK == 0x05){//(SOC)
		BMS_Data[24] = ECanbMboxes.MBOX5.MDL.byte.BYTE0;  //SOC
		// clear the mailbox RMP bit
	}
	else if(MASK == 0x06){//(Relay Status
		BMS_Data[25] = ECanbMboxes.MBOX6.MDL.byte.BYTE0; //relay status
		BMS_Data[26] = ECanbMboxes.MBOX6.MDL.byte.BYTE1; //other status
		// clear the mailbox RMP bit
	}
	else if(MASK == 0x08){//Cell_1->10
			/*	0x1831030B
			 0x1832030B
			 0x1833030B
			 0x1834030B
			 0x1835030B*/
		if(ECanbMboxes.MBOX8.MSGID.all == 0xD831030B )
			CANGlobalMaskReceive(MASK,&Cell_Data[0],8); //CellV 1->4
		else if(ECanbMboxes.MBOX8.MSGID.all == 0xD832030B )
			CANGlobalMaskReceive(MASK,&Cell_Data[8],8); //CellV 5->8
		else if(ECanbMboxes.MBOX8.MSGID.all == 0xD833030B )
			CANGlobalMaskReceive(MASK,&Cell_Data[16],4); //CellV 9->10
		else if(ECanbMboxes.MBOX8.MSGID.all == 0xD834030B )
			CANGlobalMaskReceive(MASK,&Cell_Data[226],8); //CellT 1->8
		else if(ECanbMboxes.MBOX8.MSGID.all == 0xD835030B )
			CANGlobalMaskReceive(MASK,&Cell_Data[234],2); //CellT 9->10
	}
	else if(MASK == 0x09){//Cell_11->21
		/*	0x1831030C
		 0x1832030C
		 0x1833030C
		 0x1834030C
		 0x1835030C*/
		if(ECanbMboxes.MBOX9.MSGID.all == 0xD831030C )
			CANGlobalMaskReceive(MASK,&Cell_Data[20],8); //CellV 11->14
		else if(ECanbMboxes.MBOX9.MSGID.all == 0xD832030C )
			CANGlobalMaskReceive(MASK,&Cell_Data[28],8); //CellV 15->18
		else if(ECanbMboxes.MBOX9.MSGID.all == 0xD833030C )
			CANGlobalMaskReceive(MASK,&Cell_Data[36],6); //Cell1V 9->21
		else if(ECanbMboxes.MBOX9.MSGID.all == 0xD834030C )
			CANGlobalMaskReceive(MASK,&Cell_Data[236],8); //CellT 11->18
		else if(ECanbMboxes.MBOX9.MSGID.all == 0xD835030C )
			CANGlobalMaskReceive(MASK,&Cell_Data[244],3); //CellT 19->21
	}
	else if(MASK == 0x0A){//Cell_22->33
		/*	0x1831030D
		 0x1832030D
		 0x1833030D
		 0x1834030D
		 0x1835030D*/
		if(ECanbMboxes.MBOX10.MSGID.all == 0xD831030D )
			CANGlobalMaskReceive(MASK,&Cell_Data[42],8); //CellV 22->25
		else if(ECanbMboxes.MBOX10.MSGID.all == 0xD832030D )
			CANGlobalMaskReceive(MASK,&Cell_Data[50],8); //CellV 26->29
		else if(ECanbMboxes.MBOX10.MSGID.all == 0xD833030D )
			CANGlobalMaskReceive(MASK,&Cell_Data[58],8); //CellV 30->33
		else if(ECanbMboxes.MBOX10.MSGID.all == 0xD834030D )
			CANGlobalMaskReceive(MASK,&Cell_Data[247],8); //CellT 22->29
		else if(ECanbMboxes.MBOX10.MSGID.all == 0xD835030D )
			CANGlobalMaskReceive(MASK,&Cell_Data[255],4); //CellT 30->33
	}
	else if(MASK == 0x0B){//Cell_34->43
		/*	0x1831030E
		 0x1832030E
		 0x1833030E
		 0x1834030E
		 0x1835030E*/
		if(ECanbMboxes.MBOX11.MSGID.all == 0xD831030E )
			CANGlobalMaskReceive(MASK,&Cell_Data[66],8); //CellV 34->37
		else if(ECanbMboxes.MBOX11.MSGID.all == 0xD832030E )
			CANGlobalMaskReceive(MASK,&Cell_Data[74],8); //CellV 38->41
		else if(ECanbMboxes.MBOX11.MSGID.all == 0xD833030E )
			CANGlobalMaskReceive(MASK,&Cell_Data[82],4); //CellV 42->43
		else if(ECanbMboxes.MBOX11.MSGID.all == 0xD834030E )
			CANGlobalMaskReceive(MASK,&Cell_Data[259],8); //CellT 34->41
		else if(ECanbMboxes.MBOX11.MSGID.all == 0xD835030E )
			CANGlobalMaskReceive(MASK,&Cell_Data[267],2); //CellT 42->43
	}
	else if(MASK == 0x0C){//Cell_44->53
		/*	0x1831030F
		 0x1832030F
		 0x1833030F
		 0x1834030F
		 0x1835030F*/
		if(ECanbMboxes.MBOX12.MSGID.all == 0xD831030F )
			CANGlobalMaskReceive(MASK,&Cell_Data[86],8); //CellV 44->47
		else if(ECanbMboxes.MBOX12.MSGID.all == 0xD832030F )
			CANGlobalMaskReceive(MASK,&Cell_Data[94],8); //CellV 48->51
		else if(ECanbMboxes.MBOX12.MSGID.all == 0xD833030F )
			CANGlobalMaskReceive(MASK,&Cell_Data[102],4); //CellV 52->53
		else if(ECanbMboxes.MBOX12.MSGID.all == 0xD834030F )
			CANGlobalMaskReceive(MASK,&Cell_Data[269],8); //CellT 44->51
		else if(ECanbMboxes.MBOX12.MSGID.all == 0xD835030F )
			CANGlobalMaskReceive(MASK,&Cell_Data[277],2); //CellT 52->53
	}
	else if(MASK == 0x0D){//Cell_54->63
		/*	0x18310310
		 0x18320310
		 0x18330310
		 0x18340310
		 0x18350310*/
		if(ECanbMboxes.MBOX13.MSGID.all == 0xD8310310 )
			CANGlobalMaskReceive(MASK,&Cell_Data[106],8); //CellV 54->57
		else if(ECanbMboxes.MBOX13.MSGID.all == 0xD8320310 )
			CANGlobalMaskReceive(MASK,&Cell_Data[114],8); //CellV 58->61
		else if(ECanbMboxes.MBOX13.MSGID.all == 0xD8330310 )
			CANGlobalMaskReceive(MASK,&Cell_Data[122],4); //CellV 62->63
		else if(ECanbMboxes.MBOX13.MSGID.all == 0xD8340310 )
			CANGlobalMaskReceive(MASK,&Cell_Data[279],8); //CellT 54->61
		else if(ECanbMboxes.MBOX13.MSGID.all == 0xD8350310 )
			CANGlobalMaskReceive(MASK,&Cell_Data[287],2); //CellT 62->63
	}
	else if(MASK == 0x0E){//Cell_64->75
		/*	0x18310311
		 0x18320311
		 0x18330311
		 0x18340311
		 0x18350311*/
		if(ECanbMboxes.MBOX14.MSGID.all == 0xD8310311 )
			CANGlobalMaskReceive(MASK,&Cell_Data[126],8); //CellV 64->67
		else if(ECanbMboxes.MBOX14.MSGID.all == 0xD8320311 )
			CANGlobalMaskReceive(MASK,&Cell_Data[134],8); //CellV 68->71
		else if(ECanbMboxes.MBOX14.MSGID.all == 0xD8330311 )
			CANGlobalMaskReceive(MASK,&Cell_Data[142],8); //CellV 72->75
		else if(ECanbMboxes.MBOX14.MSGID.all == 0xD8340311 )
			CANGlobalMaskReceive(MASK,&Cell_Data[289],8); //CellT 64->71
		else if(ECanbMboxes.MBOX14.MSGID.all == 0xD8350311 )
			CANGlobalMaskReceive(MASK,&Cell_Data[297],4); //CellT 72->75
	}
	else if(MASK == 0x0F){//Cell_76->85
		/*	0x18310312
		 0x18320312
		 0x18330312
		 0x18340312
		 0x18350312*/
		if(ECanbMboxes.MBOX15.MSGID.all == 0xD8310312 )
			CANGlobalMaskReceive(MASK,&Cell_Data[150],8); //CellV 76->79
		else if(ECanbMboxes.MBOX15.MSGID.all == 0xD8320312 )
			CANGlobalMaskReceive(MASK,&Cell_Data[158],8); //CellV 80->83
		else if(ECanbMboxes.MBOX15.MSGID.all == 0xD8330312 )
			CANGlobalMaskReceive(MASK,&Cell_Data[166],4); //CellV 84->85
		else if(ECanbMboxes.MBOX15.MSGID.all == 0xD8340312 )
			CANGlobalMaskReceive(MASK,&Cell_Data[301],8); //CellT 76->83
		else if(ECanbMboxes.MBOX15.MSGID.all == 0xD8350312 )
			CANGlobalMaskReceive(MASK,&Cell_Data[309],2); //CellT 84->85
	}
	else if(MASK == 0x10){//Cell_86->95
		/*	0x18310313
		 0x18320313
		 0x18330313
		 0x18340313
		 0x18350313*/
		if(ECanbMboxes.MBOX16.MSGID.all == 0xD8310313 )
			CANGlobalMaskReceive(MASK,&Cell_Data[170],8); //CellV 86->89
		else if(ECanbMboxes.MBOX16.MSGID.all == 0xD8320313 )
			CANGlobalMaskReceive(MASK,&Cell_Data[178],8); //CellV 90->93
		else if(ECanbMboxes.MBOX16.MSGID.all == 0xD8330313 )
			CANGlobalMaskReceive(MASK,&Cell_Data[186],4); //CellV 94->95
		else if(ECanbMboxes.MBOX16.MSGID.all == 0xD8340313 )
			CANGlobalMaskReceive(MASK,&Cell_Data[311],8); //CellT 86->93
		else if(ECanbMboxes.MBOX16.MSGID.all == 0xD8350313 )
			CANGlobalMaskReceive(MASK,&Cell_Data[319],2); //CellT 94->95
	}
	else if(MASK == 0x11){//Cell_96->105
		/*	0x18310314
		 0x18320314
		 0x18330314
		 0x18340314
		 0x18350314*/
		if(ECanbMboxes.MBOX17.MSGID.all == 0xD8310314 )
			CANGlobalMaskReceive(MASK,&Cell_Data[190],8); //CellV 96->99
		else if(ECanbMboxes.MBOX17.MSGID.all == 0xD8320314 )
			CANGlobalMaskReceive(MASK,&Cell_Data[198],8); //CellV 100->103
		else if(ECanbMboxes.MBOX17.MSGID.all == 0xD8330314 )
			CANGlobalMaskReceive(MASK,&Cell_Data[206],4); //CellV 104->105
		else if(ECanbMboxes.MBOX17.MSGID.all == 0xD8340314 )
			CANGlobalMaskReceive(MASK,&Cell_Data[321],8); //CellT 96->103
		else if(ECanbMboxes.MBOX17.MSGID.all == 0xD8350314 )
			CANGlobalMaskReceive(MASK,&Cell_Data[329],2); //CellT 104->105
	}
	ECanbShadow.CANRMP.all = ECanbRegs.CANRMP.all;
	ECanbShadow.CANRMP.all |= 0xFFFFFFFF;
	ECanbRegs.CANRMP.all = ECanbShadow.CANRMP.all;

}
void SendModemStatus(){

}
void SendTelematicsStatus(){
	struct ECAN_REGS ECanaShadow;
	static int Index = 0;
	if(modemstatus.bit.bit8_ReceivedSuccessful){
		ECanaMboxes.MBOX17.MDL.byte.BYTE0 = TELEMATICSRECEIVEDRX;
		ECanaRegs.CANTRS.bit.TRS17 = 1;  // Set TRS for all transmit mailboxes
	    while(ECanaRegs.CANTA.bit.TA17 != 1 ) {}  // Wait for all TAn bits to be set..
	    ECanaRegs.CANTA.bit.TA17 = 1;   // Clear all TAn
	    modemstatus.bit.bit8_ReceivedSuccessful = 0;
	}
	if(sdstatus.bit.bit13_IDstoredSuccess){
		ECanaMboxes.MBOX17.MDL.byte.BYTE0 = TELEMATICSSTORESDID;
		ECanaMboxes.MBOX17.MDL.byte.BYTE1 = 1;
		ECanaRegs.CANTRS.bit.TRS17 = 1;  // Set TRS for all transmit mailboxes
		while(ECanaRegs.CANTA.bit.TA17 != 1 ) {}  // Wait for all TAn bits to be set..
		ECanaRegs.CANTA.bit.TA17 = 1;   // Clear all TAn
		sdstatus.bit.bit13_IDstoredSuccess = 0;
	}
	if(modemstatus.bit.bit7_SendTelematicON){
		switch(Index){
		case 0:
			ECanaMboxes.MBOX17.MDL.byte.BYTE0 = TELEMATICSIDRX;
			ECanaMboxes.MBOX17.MDL.byte.BYTE1 = Transmit_ID[3];
			ECanaMboxes.MBOX17.MDL.byte.BYTE2 = Transmit_ID[4];
			ECanaMboxes.MBOX17.MDL.byte.BYTE3 = Transmit_ID[5];
			ECanaMboxes.MBOX17.MDH.byte.BYTE4 = Transmit_ID[6];
			ECanaRegs.CANTRS.bit.TRS17 = 1;  // Set TRS for all transmit mailboxes
		    while(ECanaRegs.CANTA.bit.TA17 != 1 ) {}  // Wait for all TAn bits to be set..
		    ECanaRegs.CANTA.bit.TA17 = 1;   // Clear all TAn
			Index++;
		break;
		case 1:
			ECanaMboxes.MBOX17.MDL.byte.BYTE0 = TEMATICSPHONENUMRXL;
			ECanaMboxes.MBOX17.MDL.byte.BYTE1 = PhoneNum[0];
			ECanaMboxes.MBOX17.MDL.byte.BYTE2 = PhoneNum[1];
			ECanaMboxes.MBOX17.MDL.byte.BYTE3 = PhoneNum[2];
			ECanaMboxes.MBOX17.MDH.byte.BYTE4 = PhoneNum[3];
			ECanaMboxes.MBOX17.MDH.byte.BYTE5 = PhoneNum[4];
			ECanaMboxes.MBOX17.MDH.byte.BYTE6 = PhoneNum[5];
			ECanaMboxes.MBOX17.MDH.byte.BYTE7 = PhoneNum[6];

			ECanaRegs.CANTRS.bit.TRS17 = 1;  // Set TRS for all transmit mailboxes
		    while(ECanaRegs.CANTA.bit.TA17 != 1 ) {}  // Wait for all TAn bits to be set..
		    ECanaRegs.CANTA.bit.TA17 = 1;   // Clear all TAn
			Index++;
		break;
		case 2:
			ECanaMboxes.MBOX17.MDL.byte.BYTE0 = TEMATICSPHONENUMRXH;
			ECanaMboxes.MBOX17.MDL.byte.BYTE1 = PhoneNum[7];
			ECanaMboxes.MBOX17.MDL.byte.BYTE2 = PhoneNum[8];
			ECanaMboxes.MBOX17.MDL.byte.BYTE3 = PhoneNum[9];
			ECanaRegs.CANTRS.bit.TRS17 = 1;  // Set TRS for all transmit mailboxes
		    while(ECanaRegs.CANTA.bit.TA17 != 1 ) {}  // Wait for all TAn bits to be set..
		    ECanaRegs.CANTA.bit.TA17 = 1;   // Clear all TAn
			Index++;
		break;
		case 3:
			ECanaMboxes.MBOX17.MDL.byte.BYTE0 = TELEMATICSMEIDNUMRXL;
			ECanaMboxes.MBOX17.MDL.byte.BYTE1 = MEID_number[0];
			ECanaMboxes.MBOX17.MDL.byte.BYTE2 = MEID_number[1];
			ECanaMboxes.MBOX17.MDL.byte.BYTE3 = MEID_number[2];
			ECanaMboxes.MBOX17.MDH.byte.BYTE4 = MEID_number[3];
			ECanaMboxes.MBOX17.MDH.byte.BYTE5 = MEID_number[4];
			ECanaMboxes.MBOX17.MDH.byte.BYTE6 = MEID_number[5];
			ECanaMboxes.MBOX17.MDH.byte.BYTE7 = MEID_number[7];
			ECanaRegs.CANTRS.bit.TRS17 = 1;  // Set TRS for all transmit mailboxes
		    while(ECanaRegs.CANTA.bit.TA17 != 1 ) {}  // Wait for all TAn bits to be set..
		    ECanaRegs.CANTA.bit.TA17 = 1;   // Clear all TAn
			Index++;
		break;
		case 4:
			ECanaMboxes.MBOX17.MDL.byte.BYTE0 = TELEMATICSMEIDNUMRXH;
			ECanaMboxes.MBOX17.MDL.byte.BYTE1 = MEID_number[8];
			ECanaMboxes.MBOX17.MDL.byte.BYTE2 = MEID_number[9];
			ECanaMboxes.MBOX17.MDL.byte.BYTE3 = MEID_number[10];
			ECanaMboxes.MBOX17.MDH.byte.BYTE4 = MEID_number[11];
			ECanaMboxes.MBOX17.MDH.byte.BYTE5 = MEID_number[12];
			ECanaMboxes.MBOX17.MDH.byte.BYTE6 = MEID_number[13];
			ECanaMboxes.MBOX17.MDH.byte.BYTE7 = MEID_number[14];

			ECanaRegs.CANTRS.bit.TRS17 = 1;  // Set TRS for all transmit mailboxes
		    while(ECanaRegs.CANTA.bit.TA17 != 1 ) {}  // Wait for all TAn bits to be set..
		    ECanaRegs.CANTA.bit.TA17 = 1;   // Clear all TAn
			Index++;
		break;
		case 5:
			ECanaMboxes.MBOX17.MDL.byte.BYTE0 = TELEMATICSSIGNALRX;
			ECanaMboxes.MBOX17.MDL.byte.BYTE1 = 0x2;
			ECanaRegs.CANTRS.bit.TRS17 = 1;  // Set TRS for all transmit mailboxes
		    while(ECanaRegs.CANTA.bit.TA17 != 1 ) {}  // Wait for all TAn bits to be set..
		    ECanaRegs.CANTA.bit.TA17 = 1;   // Clear all TAn
			Index++;
		break;
		case 6:
			ECanaMboxes.MBOX17.MDL.byte.BYTE0 = TELEMATICSMODEMSTATUSRX;
			ECanaMboxes.MBOX17.MDL.byte.BYTE1 = 0x2;
			ECanaRegs.CANTRS.bit.TRS17 = 1;  // Set TRS for all transmit mailboxes
		    while(ECanaRegs.CANTA.bit.TA17 != 1 ) {}  // Wait for all TAn bits to be set..
		    ECanaRegs.CANTA.bit.TA17 = 1;   // Clear all TAn
			Index++;
		break;
		case 7:
			ECanaMboxes.MBOX17.MDL.byte.BYTE0 = TELEMATICSVEHICLESTATUSRX;
			ECanaMboxes.MBOX17.MDL.byte.BYTE1 = 0x2;
			ECanaRegs.CANTRS.bit.TRS17 = 1;  // Set TRS for all transmit mailboxes
		    while(ECanaRegs.CANTA.bit.TA17 != 1 ) {}  // Wait for all TAn bits to be set..
		    ECanaRegs.CANTA.bit.TA17 = 1;   // Clear all TAn
			Index++;
		break;
		case 8:
			ECanaMboxes.MBOX17.MDL.byte.BYTE0 = TELEMATICSSDSTATUSRX;
			ECanaMboxes.MBOX17.MDL.byte.BYTE1 = 0x2;
			ECanaRegs.CANTRS.bit.TRS17 = 1;  // Set TRS for all transmit mailboxes
		    while(ECanaRegs.CANTA.bit.TA17 != 1 ) {}  // Wait for all TAn bits to be set..
		    ECanaRegs.CANTA.bit.TA17 = 1;   // Clear all TAn
			Index++;
			modemstatus.bit.bit7_SendTelematicON = 0;
		break;
		case 9:
			ECanaMboxes.MBOX17.MDL.byte.BYTE0 = TELEMATICSMODEMSTATUSRX;
			ECanaMboxes.MBOX17.MDL.byte.BYTE1 = modemstatus.all & 0xFF;
			ECanaRegs.CANTRS.bit.TRS17 = 1;  // Set TRS for all transmit mailboxes
			while(ECanaRegs.CANTA.bit.TA17 != 1 ) {}  // Wait for all TAn bits to be set..
			ECanaRegs.CANTA.bit.TA17 = 1;   // Clear all TAn
			Index++;
		break;
		case 10:
			ECanaMboxes.MBOX17.MDL.byte.BYTE0 = TELEMATICSOFTWAREVERSION;
			ECanaMboxes.MBOX17.MDL.byte.BYTE1 = VersionNumber;
			ECanaRegs.CANTRS.bit.TRS17 = 1;  // Set TRS for all transmit mailboxes
			while(ECanaRegs.CANTA.bit.TA17 != 1 ) {}  // Wait for all TAn bits to be set..
			ECanaRegs.CANTA.bit.TA17 = 1;   // Clear all TAn
			Index=0;
		break;
		case 11:

		break;
		}
	}
}
void CollectPackData() {
	unsigned int dummy = 0;
	struct ECAN_REGS ECanbShadow;
//Send Command
	ECanbMboxes.MBOX0.MDL.byte.BYTE0 = dummy;
	ECanbMboxes.MBOX1.MDL.byte.BYTE0 = dummy;
    ECanbShadow.CANTRS.all = 0;
    ECanbShadow.CANTRS.bit.TRS0 = 1;             // Set TRS for mailbox under test
    ECanbRegs.CANTRS.all = ECanbShadow.CANTRS.all;
    ECanbShadow.CANTA.all = 0;
    ECanbShadow.CANTA.bit.TA0 = 1;     	         // Clear TA5
    ECanbRegs.CANTA.all = ECanbShadow.CANTA.all;

    ECanbShadow.CANTRS.all = 0;
    ECanbShadow.CANTRS.bit.TRS1 = 1;             // Set TRS for mailbox under test
    ECanbRegs.CANTRS.all = ECanbShadow.CANTRS.all;
    ECanbShadow.CANTA.all = 0;
    ECanbShadow.CANTA.bit.TA1 = 1;     	         // Clear TA5
    ECanbRegs.CANTA.all = ECanbShadow.CANTA.all;
//Receive Data

}
void ClearReserved(){
	int i;
	for(i = 27; i < 32 ; i++)
		BMS_Data[i]  = 0;
	for(i = 210; i < 226 ; i++ )
		Cell_Data[i] = 0;
	for(i = 331; i < 336; i++)
		Cell_Data[i] = 0;
	for(i = 64; i < 88; i++)
		Vehicle_Data[i] = 0;
//	Vehicle_Data[17] = 0;
}
