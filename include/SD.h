//############################################################################
//
// FILE:   SD.h
//
// TITLE:  Command, Data Token, Global Prototypes, and Global Variables
//
//############################################################################
// Author: Hector Ta
// Release Date: Dec 2021
//############################################################################


/* ***********************************************************
* You may not use the Program in non-TI devices.
* ********************************************************* */


//#include "DSP280x_Device_lib.h"     // DSP280x Headerfile Spi Include File

//SD Standard Commands
#define GO_IDLE_STATE 				0x4000
#define SEND_OP_COND  				0x4100
#define SWITCH_FUNC					0x4600
#define SEND_IF_COND				0x4800
#define SEND_CSD	  				0x4900
#define SEND_CID	  				0x4A00
#define	STOP_TRANSMISSION			0x4C00
#define SEND_STATUS 				0x4D00
#define	SET_BLOCKLEN				0x5000
#define READ_SINGLE_BLOCK 			0x5100
#define READ_MULTIPLE_BLOCK			0x5200
#define WRITE_BLOCK 				0x5800
#define	WRITE_MULTIPLE_BLOCK		0x5900
#define PROGRAM_CSD					0x5B00
#define SET_WRITE_PROT				0x5C00
#define CLR_WRITE_PROT				0x5D00
#define SEND_WRITE_PROT				0x5E00
#define ERASE_WR_BLK_START_ADDR 	0x6000
#define ERASE_WR_BLK_END_ADDR 		0x6100
#define ERASE 						0x6600
#define	LOCK_UNLOCK					0x6A00
#define APP_CMD						0x7700
#define GEN_CMD						0x7800
#define READ_OCR	 				0x7A00
#define CRC_ON_OFF					0x7B00

//SD Application Specific Commands
#define	SD_STATUS					0x4D00
#define SEND_NUM_WR_BLOCKS			0x5600
#define SET_WR_BLK_ERASE_COUNT		0x5700
#define SD_SEND_OP_COND				0x6900
#define SET_CLR_CARD_DETECT			0x6A00
#define SEND_SCR					0x7300

//Data Tokens
#define INITIAL_CRC 				0x9500
#define DUMMY_CRC 					0xFF00
#define DUMMY_DATA 					0xFF00
#define START_BLOCK					0xFE00
#define MULTIPLE_START_BLOCK 		0xFC00
#define MULTIPLE_STOP_TRANSMISSION	0xFD00 
#define SECTOR_ZERO 		    	0x0000
#define SECTOR_ZERO_LOSTPACKET      0x0500
#define SECTOR_TELEMATICSID			0x0510
#define SECTOR_ZERO_BMS				0x1000
#define SECTOR_ZERO_CELL			0x3000
#define SECTOR_ZERO_VIM				0x5000
#define STUFF_BITS 					0x0000
#define BUSY						0x0000
#define SINGLE_SECTOR				0x0001
#define MULTIPLE_WRITE				0x0001
#define CRC_ON						0x0001
#define CRC_OFF						0x0000
#define BLOCK_LENGTH				0x0200
#define TRUE  						0x0001
#define FALSE 						0x0000
#define CHECK_PATTERN				0xAA00
#define SUPPLY_VOLTAGE				0x0100
#define HIGH_CAPACITY				0x4000
#define SUPPORTED_VOLTAGE			0xFF00
#define INTERFACE_COND				426
#define VER2_OP_COND				1073741824

//Status and Error Codes
#define IN_IDLE_STATE 				0x0100
#define SUCCESS 					0x0000
#define DATA_ACCEPTED 				0x0500
#define CRC_ERROR 					0x0B00
#define WRITE_ERROR 				0x0D00
#define ERROR						0x0100
#define CC_ERROR					0x0200
#define CARD_ECC_FAILED				0x0400
#define OUT_OF_RANGE				0x0800  
#define ILLEGAL_COMMAND_IDLE_STATE  0x0500 
#define Driving 68
#define Charging 67
#define PowerDown 100

#define POWERUPINIT 0
#define CARDINIT	1
#define READ_REGISTER1		2
#define READ_REGISTER2		3
#define READ_REGISTER3		4
#define READLOSTPACKET 		5
#define READMEIDNUMBER      6
#define CARDACTIVE			7
#define WRITING_BMS			8
#define WRITING_CELL		9
#define WRITING_VIM			10
#define READING				11
#define STORELOSTPACKET    20
#define STORETELEMATICID   30
#define POWERDOWN 		  255

#define OffLineError 	1
//Continuous Commands
#define CS_HIGH GpioDataRegs.GPBSET.bit.GPIO57 = 1;
#define	CS_LOW GpioDataRegs.GPBCLEAR.bit.GPIO57 = 1;
#define RESET_RESPONSE response = 0xFF00;
#define EIGHT_CLOCK_CYCLE_DELAY spi_xmit_byte(DUMMY_DATA);

#define SDERR_CNT 50000
#define nSector 512
//Global Prototypes

//Functions located in SD_SPI_Erase.c
extern void sd_erase_block(Uint16 starting_sector, Uint16 total_sectors);

//Functions located in SD_SPI_Initialization.c
extern void spi_initialization();
extern void led_initialization();
extern void sd_card_insertion();
extern void sd_initialization();
extern void sd_version1_initialization();
extern void sd_version2_initialization();

//Functions located in SD_SPI_Read.c
extern void sd_read_block_init(Uint16 sector, Uint16 *pBuffer);
extern void sd_read_block(Uint16 sector, Uint16 *pBuffer);
extern void sd_read_multiple_block(Uint16 sector, Uint16 *pBuffer, Uint16 total_sectors);
extern void sd_data_response(Uint16 *pBuffer, Uint16 total_sectors);

//Functions located in SD_SPI_Registers.c
extern void sd_read_register(Uint16 command);
extern void sd_ocr_response();
extern void sd_cid_csd_response(Uint16 command);
extern void sd_send_status();
extern void sd_switch_function(Uint16 *pBuffer);

//Functions located in SD_SPI_Transmission.c
extern Uint16 spi_xmit_byte(Uint16 byte);
extern void spi_xmit_command(Uint16 command, Uint32 data, Uint16 crc);
extern Uint16 sd_crc7(Uint16 full_command[6]);
extern void sd_command_response();
extern void sd_error();

//Functions located in SD_SPI_Write.c
extern void sd_write_block(Uint16 sector, Uint16 *pBuffer);
extern void sd_write_multiple_block(Uint16 sector, Uint16 *pBuffer, Uint16 total_sectors);
extern void sd_write_data(Uint16 *pBuffer, Uint16 total_sectors, Uint16 multiple_write);
extern void sd_write_block_PwrDown(Uint16 sector, Uint16 *pBuffer);
extern void sd_write_data_PwrDown(Uint16 *pBuffer, Uint16 total_sectors, Uint16 multiple_write);
//Global Variables
extern Uint16 response, ocr_contents[5], csd_contents[16], cid_contents[16];
extern Uint16 card_status[2], data_manipulation, high_capacity, crc_enabled;
extern  union SDSTATUS sdstatus;
extern unsigned int SD_process;
struct SDSTATUS_BITS {     // bits  description
    Uint16  bit0_CardDetected:1;
    Uint16  bit1_InitNotComplete:1;
    Uint16  bit2_Write_BMS:1;
    Uint16  bit3_Write_Cell:1;
    Uint16  bit4_Write_VIM:1;
    Uint16  bit5_WritingBlock:1;
    Uint16  bit6_Read_BMS:1;
    Uint16  bit7_Read_Cell:1;
    Uint16  bit8_Read_VIM:1;
    Uint16  bit9_ReadSucceed:1;
    Uint16  bit10_HighCapacity:1;
    Uint16  bit11_Error:1;
    Uint16  bit12_ReadSucceed2:1;
    Uint16  bit13_IDstoredSuccess:1;
};

union SDSTATUS {
   Uint16                all;
   struct SDSTATUS_BITS   bit;
};
