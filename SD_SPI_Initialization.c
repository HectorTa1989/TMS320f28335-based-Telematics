//############################################################################
//
// FILE:   SD_SPI_Initialization.c
//
// TITLE:  SD/MMC Initialization Functions
//
//############################################################################
// Author: Hector Ta
// Release Date: Dec 2021
//############################################################################


/* ***********************************************************
* You may not use the Program in non-TI devices.
* ********************************************************* */

#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File

#include "SD.h"							//SD Include File

//Global Variables
Uint16 response, ocr_contents[5], csd_contents[16], cid_contents[16];
Uint16 card_status[2], data_manipulation = TRUE, high_capacity = FALSE, crc_enabled = FALSE;



//######################### CHECK_CARD_INSERTION #############################
void sd_card_insertion()
{
	Uint16 i;


   	//After Card Detection, SD protocol states that card needs 74 clock 
   	//cycles with the DATA IN line high for chip to stabilize. CS does not 
   	//need to be active for this action.
	sdstatus.bit.bit10_HighCapacity= 0;
	sdstatus.bit.bit0_CardDetected = 0;
   	CS_HIGH;							//Pull CS high
	for(i=0;i<10;i++){					//Transmit 0xFF for 80 clock cycles
   		spi_xmit_byte(DUMMY_DATA);
	}
	SD_process ++;
}

//######################### CHECK_CARD_INSERTION #############################


//########################## SD_INITIALIZATION ###############################
void sd_initialization()
{
	CS_LOW;									//Pull CS low
	data_manipulation = FALSE;				//Register manipulation function

	//Transmit GO IDLE STATE command to card with CS low to select SPI mode 
	//and put the card in IDLE mode.
   	spi_xmit_command(GO_IDLE_STATE, STUFF_BITS, INITIAL_CRC);	

   	RESET_RESPONSE;							//Reset response	
	while(response != IN_IDLE_STATE)		//Wait until card responds with IDLE response
		sd_command_response();


	sdstatus.bit.bit0_CardDetected = 1;
//	GpioDataRegs.GPASET.bit.GPIO20 = TRUE;  //Disable SDIO Mode LED, Optional
//	GpioDataRegs.GPACLEAR.bit.GPIO5 = TRUE; //Emit SPI Mode LED, Optional
	
	RESET_RESPONSE;							//Reset response			
	//After receiving response clock must be active for 8 clock cycles
	EIGHT_CLOCK_CYCLE_DELAY;

	crc_enabled = TRUE;						//CRC is always enabled for SEND_IF_COND command
	spi_xmit_command(SEND_IF_COND, INTERFACE_COND, DUMMY_CRC);	//Transmit SEND_IF_COND command
	crc_enabled = FALSE;					//CRC is disabled for SPI mode

	//Wait until card responds with IDLE response
	while((response != IN_IDLE_STATE) && (response != ILLEGAL_COMMAND_IDLE_STATE))	
		response = spi_xmit_byte(DUMMY_DATA);

	//If SEND_IF_COND returned illegal command call sd_version1_initialization for
	//standard capacity card initialization. Otherwise call sd_version2_initialization 
	//for high capacity card initialization
	if(response == ILLEGAL_COMMAND_IDLE_STATE)
		sd_version1_initialization();	
	else if(response == IN_IDLE_STATE)
		sd_version2_initialization();

	CS_HIGH;								//Pull CS high
   	//After receiving response clock must be active for 8 clock cycles
	EIGHT_CLOCK_CYCLE_DELAY;
	SD_process++;
	SpiaRegs.SPIBRR = 0x0003;				//Adjust Clock to 10.0 MHz (10.0 Mbps)
}
//########################## SD_INITIALIZATION ###############################


//###################### SD_VERSION1_INITIALIZATION ##########################
void sd_version1_initialization()
{

	RESET_RESPONSE;						//Reset response
   	//After receiving response clock must be active for 8 clock cycles
	EIGHT_CLOCK_CYCLE_DELAY;

	spi_xmit_command(READ_OCR, STUFF_BITS, DUMMY_CRC);	//Transmit READ_OCR command
	//Wait until card responds with IN_IDLE_STATE response
	while(response != IN_IDLE_STATE)		
		sd_command_response();

	sd_ocr_response();					//Call OCR response function

	//If host voltage is not compatible with card voltage, do not communicate 
	//further with card
	if(ocr_contents[1] != SUPPORTED_VOLTAGE)		
		sd_error();

	RESET_RESPONSE;						//Reset response
 	//After receiving response clock must be active for 8 clock cycles
	EIGHT_CLOCK_CYCLE_DELAY;

	while(response != SUCCESS)
	{
		RESET_RESPONSE;					//Reset response
   		//After receiving response clock must be active for 8 clock cycles
		EIGHT_CLOCK_CYCLE_DELAY;
		
		//Before transmitting application specific commands, the APP_CMD
		//must be transmitted
		spi_xmit_command(APP_CMD, STUFF_BITS, DUMMY_CRC);
		//Wait until card responds with IN_IDLE_STATE response
		while(response != IN_IDLE_STATE)	 
			sd_command_response();

		RESET_RESPONSE;					//Reset response
   		//After receiving response clock must be active for 8 clock cycles
		EIGHT_CLOCK_CYCLE_DELAY;	
   		
   		//Transmit SEND OP COND command   								
 		spi_xmit_command(SD_SEND_OP_COND, STUFF_BITS, DUMMY_CRC);				  
		sd_command_response();			//Receive response
	}
	
	RESET_RESPONSE;						//Reset response
   	//After receiving response clock must be active for 8 clock cycles
	EIGHT_CLOCK_CYCLE_DELAY;			
}
//###################### SD_VERSION1_INITIALIZATION ##########################


//###################### SD_VERSION2_INITIALIZATION ##########################
void sd_version2_initialization()
{
	Uint16 i, send_if_cond_response[4], ccs_check;
	
	//Receive SEND_IF_COND response
	for(i=0;i<3;i++)
		send_if_cond_response[i] = spi_xmit_byte(DUMMY_DATA);

	//If voltage accepted or check pattern does not match, do not communicate further
	if((send_if_cond_response[2] != SUPPLY_VOLTAGE) || (send_if_cond_response[3] != CHECK_PATTERN))
		sd_error();

	RESET_RESPONSE;						//Reset response
 	//After receiving response clock must be active for 8 clock cycles
	EIGHT_CLOCK_CYCLE_DELAY;
	
	while((response != SUCCESS)&&(SD_process))
	{
		RESET_RESPONSE;					//Reset response
 	  	//After receiving response clock must be active for 8 clock cycles
		EIGHT_CLOCK_CYCLE_DELAY;

		//Before transmitting application specific commands, the APP_CMD
		//must be transmitted
		spi_xmit_command(APP_CMD, STUFF_BITS, DUMMY_CRC);
		while(response != IN_IDLE_STATE)//Wait until card responds with IN_IDLE_STATE response 
			sd_command_response();

		RESET_RESPONSE;					//Reset response
   		//After receiving response clock must be active for 8 clock cycles
		EIGHT_CLOCK_CYCLE_DELAY;
   		
   		//Transmit SEND_OP_COND command   								
 		spi_xmit_command(SD_SEND_OP_COND, VER2_OP_COND, DUMMY_CRC);				  
		sd_command_response();			//Receive response
	}
	
	RESET_RESPONSE;						//Reset response
   	//After receiving response clock must be active for 8 clock cycles
	EIGHT_CLOCK_CYCLE_DELAY;
		
	spi_xmit_command(READ_OCR, STUFF_BITS, DUMMY_CRC);	//Transmit READ_OCR command
	//Wait until card responds with SUCCESS response
	while((response != SUCCESS)&&(SD_process))
		sd_command_response();

	sd_ocr_response();					//Call OCR response function
	
	ccs_check = ocr_contents[0] & HIGH_CAPACITY; //Mask ocr_contents to test for High Capacity

	if(ccs_check == HIGH_CAPACITY){		//Check if card is High Capacity
		high_capacity = TRUE;				 
		sdstatus.bit.bit10_HighCapacity= 1;
	}
	RESET_RESPONSE;						//Reset response
   	//After receiving response clock must be active for 8 clock cycles
	EIGHT_CLOCK_CYCLE_DELAY;			
}
//###################### SD_VERSION2_INITIALIZATION ##########################
