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
#define TIMESTAMP_UNIT_IN_MS	                1
#define TIMESTAMP_UNIT_IN_TICKS          (TIMESTAMP_UNIT_IN_MS/portTICK_PERIOD_MS)

/* local variables ********************************************************** */
//static xTimerHandle sdCardWriteReadHandle; /**< variable to store timer handle*/
//static CmdProcessor_T *AppCmdProcessor; /**< Application Command Processor Instance */
//static SDCardDriver_DiskStatus_T diskInitStatus = SDCARD_NOT_INITIALIZED;
//#if FAT_FILE_SYSTEM==1
//static FATFS FatFileSystemObject; /* File system specific objects */
//#endif

xSemaphoreHandle ReadSensorSemaphor = NULL; /**< Semaphore to lock/unlock the Ping/Pong-Buffer*/

static Retcode_T SDC_diskInitStatus = SDCARD_NOT_INITIALIZED;
int8_t newFile = 1; /**< variable to check if a new file has to be added, set by Button pressed event*/
TCHAR filename[13] = { 0 }; //"log_%02d.csv"; /**< variable to store the actual filename*///TODO
FATFS globalmnt; /**< variable for mount process of SD-Card*/
//xSemaphoreHandle ReadSensorSemaphor = NULL; /**< Semaphore to lock/unlock the Ping/Pong-Buffer*/
Buffer pingBuffer = { 0 };//TODO
Buffer pongBuffer = { 0 };//TODO
Buffer *ActiveBuffer; /**< Pointer to the Buffer which is filled */
Buffer *BackBuffer; /**< Pointer to the Buffer to read out data */
FIL fileObject = { 0 }; /* File objects */

/* global variables ********************************************************* */

/* local module global variable declarations */
uint8_t writeBuffer[SINGLE_SECTOR_LEN] = "XDK ASAMPLE TEST FOR MMC"; /**< Write Buffer for SD-Card */
uint8_t readBuffer[SINGLE_SECTOR_LEN]; /**< read buffer for SD-Card */

/* inline functions ********************************************************* */

/* local functions ********************************************************** */

void writeSensorDataCsvHeader()
{
    //BME
    ActiveBuffer->length += sprintf(
                    ActiveBuffer->data + ActiveBuffer->length,
                    ";bme280_temp[mDeg];bme280_press[Pa];bme280_hum[rh]");

    ActiveBuffer->length += sprintf(ActiveBuffer->data + ActiveBuffer->length,
            ";\n");
}


void writeSensorDataCsv()
{
	//create timestamp
	static uint32_t numberOfTicks = TIMESTAMP_UNIT_IN_TICKS;
	uint32_t currentTicks = xTaskGetTickCount();
    uint32_t timestamp = 0;

    timestamp = ((currentTicks) / numberOfTicks); /*calculate the time stamp without the logStartTime-Offset*/

    //BME TODO muss noch übergeben/eingebunden werden
//    ActiveBuffer->length += sprintf(
//                    ActiveBuffer->data + ActiveBuffer->length, ";%ld;%ld;%ld",
//                    bme280s.temperature, bme280s.pressure, bme280s.humidity);

    ActiveBuffer->length += sprintf(ActiveBuffer->data + ActiveBuffer->length,
            ";\n");
}

void sdcard_writeBuffer(void)
{
	FRESULT fileSystemResult;
	UINT bytesWritten;

	if(newFile==1)
	{

		fileSystemResult = f_open(&fileObject, SD_FILENAME, FA_OPEN_ALWAYS | FA_WRITE);
		if (fileSystemResult != FR_OK)
		{
			/* Error. Cannot create the file */
			printf("fileSystemResult=%i \n\r", fileSystemResult);
			assert(0);
		}

		writeSensorDataCsvHeader();

		fileSystemResult = f_write(&fileObject, ActiveBuffer->data, ActiveBuffer->length, &bytesWritten);
		if ((fileSystemResult != FR_OK) || (ActiveBuffer->length != bytesWritten))
		{
		/* Error. Cannot write the file */
			printf("fileSystemResult=%i \n\r", fileSystemResult);
			assert(0);
		}

		fileSystemResult = f_close(&fileObject);
		if (fileSystemResult != FR_OK)
		{
			/* Error. Cannot close the file */
			printf("fileSystemResult=%i \n\r", fileSystemResult);
			assert(0);
		}
	}
	printf("Buffer written \r\n");//TODO Wird nicht bis hier ausgeführt
}

extern void sdcard_init(void)
{

	Retcode_T sdInitreturnValue;
	SDC_sdCardAppReturn_t csuReturnValue = 0;
	/* Initialize SD card */
	sdInitreturnValue = SDCardDriver_Initialize();

	printf("Starting SD initialization \r\n");

	if (sdInitreturnValue != RETCODE_OK)
	{ /* Debug fail case test SDC Init */
		assert(0);
	}
	else
	{
		if (SDC_diskInitStatus != SDCARD_DISK_INITIALIZED) /*SD-Card Disk Not Initialized */
		{
			SDC_diskInitStatus = SDCardDriver_DiskInitialize(SDC_DRIVE_ZERO); /* Initialize SD card */
		}
		if (SDCARD_DISK_INITIALIZED == SDC_diskInitStatus)
		{
			if (f_mount(&globalmnt, DEFAULT_LOGICAL_DRIVE, FORCE_MOUNT) != FR_OK)
			{
				assert(0);
			}
		}
	   else {
		   assert(0);
		}
	}
	if (SDC_APP_ERR_ERROR == csuReturnValue)
	{ /* Debug fail case test for SDC Read/Write */
		assert(0);
	}

	printf("SD initalized \r\n");
}

extern void sdcard_deInit(void)
{
//
//    Retcode_T DeInitStatus = RETCODE(RETCODE_SEVERITY_ERROR, SDCARD_DE_INIT_FAILED); /* Default return value is in Error Deinit state */
//
//    if (pdFAIL != xTimerStop(sdCardWriteReadHandle,
//            WRITEREAD_BLOCK_TIME))
//    {
//        DeInitStatus = RETCODE_OK;
//    }
//
//    //return (DeInitStatus);

}

