/*******************************************************************************
  * @author  Vince Keiper AstrodyneTDI
  * @version V1.0.0
  * @date    24-June-2015
  * @brief   ICCP Datalogger   
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2015 AstrodyneTDI</center></h2>
  *
  *
  ******************************************************************************  
  */ 

/* Includes ------------------------------------------------------------------*/

#include "k_storage.h"
#include "iccpfuncs.h"
#include "k_rtc.h" 
#include "k_modules_res.h"
#include "iccp_can_stm32f429.h"

/******  ICCP   *********/

#include "cmsis_os.h"
#include "stdio.h"
#include "string.h"


/**
  *
  * EXternal Resources
  *
  **/
extern char mSDDISK_Drive[];
extern FATFS mSDDISK_FatFs;   


/**
  *
  * Private Resources
  *
  **/
char tmpstr[32];
  
uint8_t WriteToLog(char *ptrpath,
				uint8_t *sec,uint8_t *min,uint8_t *hour,
				uint8_t *mon,uint8_t *day,uint16_t *year, 
				uint16_t *vrd,uint16_t *ird,uint8_t *temp,
				uint8_t *psuidx);
uint8_t ReadFromLogSD(char *ptrpath,FIL fil,char *str,uint16_t len);						

/**
  *
  * @brief  Read From SD card log
  * @param  ptrpath  = pointer to patch string ** must contain logical drive before file name so SD is 1 so example  "1:/xxxx.xxx"
  * @param  fil  = filename: text file name
  * @param  *str = ptr to string buffer read data will copy to
  * @param  len = length: amount of data to copy from file to buffer
  * @retval 0 if failed to open or read from file; 1 if opened and read more than 0 bytes
  *
  **/
uint8_t ReadFromLogSD(char *ptrpath,FIL fil,char *str,uint16_t len)						
{
	uint8_t retval=0;
	
	FRESULT fr;          /* FatFs function result code */
	uint32_t bytesread;  /* File read count */

	/*##-- Open a text file object with READ access #####*/
	/***--  example path "1:/default.txt" **/
	if(f_open(&fil, ptrpath, FA_READ) != FR_OK)
	{
		retval =0;
	}
	else
	{
		memset(str,0x00,sizeof((char*)str));// clear buffer for repop
		/*##-- Read data from the text file ###########################*/
		fr = f_read(&fil, str, len, (UINT*)&bytesread);
		
		if((bytesread == 0) || (fr != FR_OK))
		{
			retval=0;
		}
		else
		{
			/*##-- Close the open text file #############################*/
			f_close(&fil);
			retval=1;
		}
	}
	return retval;					
}


/**
  *
  * @brief  Write data to SD card datalog
  * @param  ptrpath  = pointer to patch string ** must contain logical drive before file name so SD is 1 so example  "1:/xxxx.xxx"
  * @param  fil  = FATfs file obj
  * @param  *wrstr = ptr to string buffer that data being written
  * @param  len = length: amount of data to copy from buffer to file
  * @param  sec,min,hour,mon,day,year = time and date from caller
  * @retval 0 if failed to open or write to file; 1 if opened and wrote less than 'len' bytes
  *
  **/
uint8_t WriteToLog(char *ptrpath,
				uint8_t *sec,uint8_t *min,uint8_t *hour,
				uint8_t *mon,uint8_t *day,uint16_t *year, 
				uint16_t *vrd,uint16_t *ird,uint8_t *temp,
				uint8_t *psuidx)	
{
	FIL fil;
	FRESULT fr;                       /* FatFs function common result code */
	uint8_t file_valid = 0;
	uint8_t retval=0;
	UINT bw;
	
	/*##-- try to open log file with write access #####*/
	fr = f_open(&fil, ptrpath, FA_WRITE);
	if(fr != FR_OK)
	{
		/*##-- If file does not exist try to create the log file #####*/
		if(fr == FR_NO_FILE || fr == FR_NOT_ENABLED)
		{
			if(f_open(&fil, ptrpath, FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
			{
				retval =0;// exit with fail
			}
			else
			{
				file_valid = 1;
			}
		}
		else{// ant other failure just exit early w failure
			retval =0;
			return retval;
		}
	}
	else
	{
		file_valid = 1;
	}
	// if we have an open file write the data passed in to it
	if(file_valid ==1)
	{
		/* Append a line */
		//f_printf(&fil, "%02u/%02u/%u, %2u:%02u:%2u,%2u,%4.3f,%4.3f,%2u\n", *mon,*day,*year,*hour,*min,*sec,*psuidx,(float)*vrd,(float)*ird,*temp);
		//f_printf(&fil, "%02u/%02u/%u, %2u:%02u:%2u,%2u,%4.3f,%4.3f,%2u\n", 10,12,2015,14,17,30,0,49.5,125.01,25);
		f_write(&fil,"Test\n",5,&bw);
		if(bw<5)
		{
				retval=0;
		}
		/* Close the file */
		f_close(&fil);
		retval =1;
	}
	
	return retval;
}
	
static void DlogWrThread(void const * argument);
int createLogWriteThread(void);
int createLogWriteThread(void)
{
	/* C-reate Data Log Write task */
	osThreadDef(Start_DlogWr_Thread, DlogWrThread, osPriorityLow, 0, 2 * configMINIMAL_STACK_SIZE);
	osThreadCreate (osThread(Start_DlogWr_Thread), NULL); 
	return 1;
}

static void DlogWrThread(void const * argument)
{
	/* Infinite loop */
  while(1)
  {
		  
			osDelay(50);
	}
}


//FRESULT open_append (
//    FIL* fp,            /* [OUT] File object to create */
//    const char* path    /* [IN]  File name to be opened */
//)
//{
//    FRESULT fr;

//    /* Opens an existing file. If not exist, creates a new file. */
//    fr = f_open(fp, path, FA_WRITE | FA_OPEN_ALWAYS);
//    if (fr == FR_OK) {
//        /* Seek to end of the file to append data */
//        fr = f_lseek(fp, f_size(fp));
//        if (fr != FR_OK)
//            f_close(fp);
//    }
//    return fr;
//}



///************************ (C) COPYRIGHT AstrodyneTDI 2015 *****END OF FILE****/
