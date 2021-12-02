/*
 * EVDO.h
 *  Author: Hector Ta
 *  Created on: Dec 2021
 *      
 */

#define BMSLastData 40
#define CellLastData 344
#define VehicleLastData  96

/******************Process Counter************************/
#define ATE 	0
#define Reply0  1
#define SIGNALSTRENGTH		2
#define Reply1  3
#define ClockManagement 4
#define ClockManagement_Reply 5
#define SocketConfiguration 6
#define Reply2 	7
#define GPSTurnON 8
#define GPS_Reply 9
#define ContextActivation 10
#define Reply3  11
#define SocketDial 		12
#define Reply4	13
#define SOCKETSTATUS	14
#define Reply6 15
#define Transmit	  54
#define Reply55 55
#define Reset  100
#define Reset2 101
#define Transmit2 200
#define Reply_CMD 201
#define SuspendMode 250
#define SuspendMode_Reply 251
#define RestoreSocket 300
#define ReturnToTransmit 301
#define GPSData	302
#define GPSData_Reply 303

#define MEID 398
#define MEIDReply	399
#define REGISTERVERIZON  400
#define REGISTERVERIZONREPLY 401
#define ACTIVATIONVERIZON 402
#define ACTIVATIONVERIZONREPLY 403
#define REGISTERAERIS_MDN 450
#define REGISTERAERIS_REPLY 451
#define REGISTERAERIS_MIN  452
#define REGISTERAERIS_REPLY2 453

#define MODEMRECALL 460
#define MODEMRECALL_REPLY 461
#define MODEMRECALL1 462
#define MODEMRECALL_REPLY1 463

#define ACTIVATIONOK 498
#define ACTIVATIONFAIL  499
#define HOLD 500

/***********************************************************/
struct MODEMSTATUS_BITS {     // bits  description
    Uint16  bit0_ModemDetected:1;
    Uint16  bit1_ConnectedServer:1;
    Uint16  bit2_ModemActivated:1;
    Uint16  bit3_ModemConfiguring:1;
    Uint16  bit4_Verizon:1;
    Uint16  bit5_ModemActivating:1;
    Uint16  bit6_ModemRecall:1;
    Uint16  bit7_SendTelematicON:1;
    Uint16  bit8_ReceivedSuccessful:1;
    Uint16  reserve5:1;
    Uint16  reserve7:1;
    Uint16  reserve8:1;
    Uint16  reserve9:1;
};

union MODEMSTATUS {
   Uint16                all;
   struct MODEMSTATUS_BITS   bit;
};
