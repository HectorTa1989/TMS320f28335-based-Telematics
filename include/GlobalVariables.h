#define ACChrg 1
#define CHAdeMO 2
#define CTC 3
#define DRIVE 4

struct TELEMATIC_STATUS_BITS {     // bits  description
    Uint16  bit0_ModemSignalStrong:1;
    Uint16  bit1_ModemConnected:1;
    Uint16  bit2_ModemActivated:1;
    Uint16  bit3_GPSDetected:1;
    Uint16  bit4_GPSSignalGood:1;
    Uint16  bit5_SDCardDetected:1;
    Uint16  bit6_SDCardStorageExist:1;
    Uint16  bit7_ModulePowerOn:1;
    Uint16  bit8_ModulePoweringDown:1;
    Uint16  bit9_CAN1Good:1;
    Uint16  bit10_CAN2Good:1;
    Uint16  bit11_Error:1;
    Uint16  bit12_reserved:1;
    Uint16  bit13_reserved:1;
    Uint16  bit14_reserved:1;
    Uint16  bit15_reserved:1;
};

union TELEMATICSTATUS {
   Uint16                all;
   struct TELEMATIC_STATUS_BITS   bit;
};
