
/*----------------------------------------------------------------------------*/
/* header definition ******************************************************** */
#ifndef LOGGER_LOGGER_H_
#define LOGGER_LOGGER_H_

//#include "BCDS_Retcode.h"

///* public interface declaration ********************************************* */
//#define BUFFSIZE  19500
//
///* local interface declaration ********************************************** */
//#define SD_FILENAME    "staub.csv"	/**< Filename to open/write/read from SD-card */
//
//
///* local type and macro definitions */
//#define SDC_WRITEREAD_DELAY 			UINT32_C(5000) 	 /**< Millisecond delay for WriteRead timer task */
//#define SDC_WRITEREAD_BLOCK_TIME 		UINT32_C(0xffff) /**< Macro used to define block time of a timer*/
//#define SDC_DETECT_SD_CARD_INSERTED 	UINT8_C(1)
//#define SDC_SINGLE_BLOCK				UINT8_C(1)      /**< SD- Card Single block write or read */
//#define SDC_DRIVE_ZERO				    UINT8_C(0)      /**< SD Card Drive 0 location */
//#define SDC_PARTITION_RULE_FDISK	    UINT8_C(0)      /**< SD Card Drive partition rule 0: FDISK, 1: SFD */
//#define SDC_AUTO_CLUSTER_SIZE		    UINT8_C(0)      /**< zero is given, the cluster size is determined depends on the volume size. */
//#define SDC_SEEK_FIRST_LOCATION		    UINT8_C(0)      /**< File seek to the first location */
//
//#define MAX_FILE_NAME_LENGTH              UINT8_C(13)
//#define MAX_PATH_LENGTH                   UINT8_C(256)
//
//typedef enum SDC_sdCardAppReturn_e
//{
//    SDC_APP_ERR_DEINIT_FAILED = INT8_C(-1), /**< SD-Card DeInitialize failure */
//    SDC_APP_ERR_INIT_FAILED = INT8_C(-2), /**< SD-Card Initialize failure */
//    SDC_APP_ERR_ERROR = INT8_C(-3), /**< SD-Card Non-Error return */
//    SDC_APP_ERR_WRITE_FAILURE = INT8_C(-4), /**< SD Card return write failure */
//    SDC_APP_ERR_READ_FAILURE = INT8_C(-5), /**< SD Card return read failure */
//    SDC_APP_ERR_NOTREADY_FAILURE = INT8_C(-6), /**< SD Card return Not ready failure */
//    SDC_APP_ERR_NO_ERROR = INT8_C(0) /**< SD Card Error Return */
//} SDC_sdCardAppReturn_t;
//
///**
// *  @brief Custom Error code returned by APIs of SD card application
// */
//
//typedef struct
//{
//    int32_t length;
//    char data[BUFFSIZE];
//} Buffer;
//
//#define DEFAULT_LOGICAL_DRIVE        ""             /**< Macro to define default logical drive */
//#define FORCE_MOUNT                 UINT8_C(1)      /**< Macro to define force mount */
//#define TIMER_AUTORELOAD_ON         UINT32_C(1)      /**< Auto reload of timer is enabled*/
//#define TIMER_AUTORELOAD_OFF        UINT32_C(0)      /**< Auto reload of timer is enabled*/
//#define WRITEREAD_DELAY 			UINT32_C(5000) 	 /**< Millisecond delay for WriteRead timer task */
//#define WRITEREAD_BLOCK_TIME 		UINT32_C(0xffff) /**< Macro used to define block time of a timer*/
//#define SINGLE_BLOCK				UINT8_C(1)      /**< SD- Card Single block write or read */
//#define DRIVE_ZERO				    UINT8_C(0)      /**< SD Card Drive 0 location */
//#define PARTITION_RULE_FDISK	    UINT8_C(0)      /**< SD Card Drive partition rule 0: FDISK, 1: SFD */
//#define AUTO_CLUSTER_SIZE		    UINT8_C(0)      /**< zero is given, the cluster size is determined depends on the volume size. */
//#define SEEK_FIRST_LOCATION		    UINT8_C(0)      /**< File seek to the first location */
//#define SECTOR_VALUE			    UINT8_C(6)      /**< SDC Disk sector value */
//#define SINGLE_SECTOR_LEN           UINT32_C(512)   /**< Single sector size in SDcard */
//
//
//#define FAT_FILE_SYSTEM     1 /** Macro to write data into SDCard either through FAT file system or SingleBlockWriteRead depends on the value **/
//
///* Ram buffers
// * BUFFERSIZE should be between 512 and 1024, depending on available ram on efm32
// */
//#define BUFFER_SIZE       UINT16_C(512)
//#define FILE_EQUAL 		     UINT8_C(0)
//#define FILE_NOT_EQUAL 	     UINT8_C(1)
//
///* local function prototype declarations */
//
///* local function prototype declarations */
//
///* local module global variable declarations */

/* local inline function definitions */
//extern Retcode_T sdCardFatFileSystemWriteRead(void);

//void sdcard_writeBuffer(void);

extern void sdcard_init(void);
extern void sdcard_deintit(void);

void writeDataOnSD(const char* dataBuffer);


#endif /* LOGGER_LOGGER_H_ */
/** ************************************************************************* */
