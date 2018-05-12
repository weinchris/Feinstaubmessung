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

#include "../ADC/ADC.h"
#include "../BME_280/BME_280_ch.h"
#include "../BME_280/BME_280_ih.h"
#include "../SDS011/SDS.h"

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
        printf("Mounting SD card failed; fileSystemResult: %i  \n\r", fileSystemResult);
      }
    }
  }

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

	fileSystemResult = f_open(&fileObject, filename, FA_OPEN_ALWAYS | FA_WRITE);
	if(fileSystemResult != FR_OK){
	  	printf("Error: Cannot open file %s; fileSystemResult: %i \n\r", filename, fileSystemResult);
	}
	fileSystemResult = f_lseek(&fileObject, f_size(&fileObject));
	if((fileSystemResult != FR_OK)){
			printf("Error: Cannot move file pointer for file %s; fileSystemResult: %i \n\r", filename, fileSystemResult);
		}

	fileSystemResult = f_write(&fileObject, ramBufferWrite, fileSize, &bytesWritten);
	if((fileSystemResult != FR_OK) || (fileSize != bytesWritten)){
		printf("Error: Cannot write to file %s; fileSystemResult: %i \n\r", filename, fileSystemResult);
	}

	fileSystemResult = f_close(&fileObject);
	if(fileSystemResult != FR_OK){
		  	printf("Error: Cannot close file %s; fileSystemResult: %i \n\r", filename, fileSystemResult);
	}
}

extern void sdcard_deintit(void)
{
	FRESULT fileSystemResult;
	fileSystemResult = f_close(&fileObject);
}

void writeLogs(void)
{
	char buffer[200];
	printf("Temperature, Pressure, Humidity, PM10, PM25, MQ9, MQ135, Rain\n\r");
	writeDataOnSD("Temperature, Pressure, Humidity, PM10, PM25, MQ9, MQ135, Rain\n");

	for(;;){
		vTaskDelay(10000);
		sprintf(buffer ,"%i, %i, %i, %f, %f, %f, %f, %f\n",bme280s.temperature, bme280s.pressure, bme280s.humidity, value_pm10, value_pm25, value_mq9, value_mq135, value_rain);
		printf("Buffer: %s\r", buffer);
		writeDataOnSD(buffer);
	}
}

void loggerTask(void)
{
	xTaskCreate(
			writeLogs,                 // function that implements the task
		    (const char * const) "writeLogs", // a name for the task
			(( unsigned short ) 1024),       // depth of the task stack
		    NULL,                           // parameters passed to the function
		    2,               // task priority
		    NULL                      // pointer to a task handle for late reference
		  );
}
