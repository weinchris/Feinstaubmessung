/* own header files */
#include "Logger.h"

/* system header files */
#include <stdio.h>

/* additional interface header files */
#include "BCDS_SDCard_Driver.h"
#include "BCDS_CmdProcessor.h"
#include "BSP_BoardType.h"
#include "BCDS_BSP_LED.h"
#include "ff.h"
#include "BCDS_Assert.h"
#include <FreeRTOS.h>
#include <timers.h>

/* constant definitions ***************************************************** */
/* constant definitions ***************************************************** */
#define DEFAULT_LOGICAL_DRIVE   ""
#define DRIVE_ZERO              UINT8_C(0)
#define FORCE_MOUNT             UINT8_C(1)
#define FIRST_LOCATION          UINT8_C(0)

/* global variables ********************************************************* */
static FIL fileObject;
const char* filename="staub.csv";//TODO maybe create multiple files

void sdcard_init(void){
  Retcode_T retVal = RETCODE_FAILURE;
  FRESULT fileSystemResult = FR_OK;
  static FATFS FatFileSystemObject;
  SDCardDriver_Initialize();
  if(SDCARD_INSERTED == SDCardDriver_GetDetectStatus()){
    retVal = SDCardDriver_DiskInitialize(DRIVE_ZERO);
    if(RETCODE_OK == retVal){
      printf("SD Card Disk initialize succeeded \n\r");
      fileSystemResult = f_mount(&FatFileSystemObject, DEFAULT_LOGICAL_DRIVE,
                                    FORCE_MOUNT);
      if (FR_OK != fileSystemResult){
        printf("Mounting SD card failed \n\r");
      }
    }
  }
  fileSystemResult = f_open(&fileObject, filename, FA_OPEN_ALWAYS | FA_WRITE);

}

void writeDataOnSD(const char* dataBuffer){
	FRESULT fileSystemResult;
	char ramBufferWrite[UINT16_C(512)]; // Temporay buffer for write file
	uint16_t fileSize;
	UINT bytesWritten;
	fileSize = (uint16_t) strlen(dataBuffer);

	for(uint32_t index = 0; index < fileSize; index++){
		ramBufferWrite[index] = dataBuffer[index];
	}
	f_lseek(&fileObject, f_size(&fileObject));
	fileSystemResult = f_write(&fileObject, ramBufferWrite, fileSize, &bytesWritten);

	if((fileSystemResult != FR_OK) || (fileSize != bytesWritten)){
		printf("Error: Cannot write to file %s; fileSystemResult: %i \n\r", filename, fileSystemResult);
	}
	printf("Write on SD card \n\r");
}

extern void sdcard_deintit(void)
{
	FRESULT fileSystemResult;
	fileSystemResult = f_close(&fileObject);
}
