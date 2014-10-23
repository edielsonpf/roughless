/*
 * SDCardCtrl.c
 *
 *  Created on: 22/10/2014
 *      Author: edielson
 */

#include <stdio.h>
#include <stdlib.h>

#include "SDCardCtrl.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_ssp.h"
#include "lpc17xx_uart.h"
#include "lpc17xx_timer.h"
#include "stdio.h"
#include "diskio.h"
#include "ff.h"

#define TIME_RESOLUTION					10/1000
#define UTTERANCE_FILE_NAME				"UTTERA~1.MEL"
#define UTTERANCE_DIM_FEATURE			2
#define UTTERANCE_MAX_FRAME_SIZE		1

static void 	SDCardCtrl_initSSP(void);
static Bool8 	SDCardCtrl_init(void);


typedef struct tag_SDCardCtrl
{
	tenu_SDCardProcessState eProcessState;

	FILINFO 	Finfo;
	FATFS 		Fatfs[1];
	FIL 		fileHandler;
	int 		iTotalBytes;
	int			iNumberFrames;
	int			iFrameSize;
	int			iFrameCounter;
	double**	dpUtteranceFrame;

	unsigned long ulMileSeconds;

	tcbf_SDCardCtrlCallbackList* pcbfCallbackList;

}ttag_SDCardCtrl;

static ttag_SDCardCtrl tagSDCardCtrl;

/**
	SDCardCtrl create instance. This is the method that creates the instance for the SDCardCtrl class.

    @param the class handle
    @return - void
    @author edielson
    @date   02/09/2014
*/
Bool8 SDCardCtrl_createInstance(tcbf_SDCardCtrlCallbackList* pcbfCallbackList)
{
	Bool8 bResult = False8;
	int iCounter;

	if(SDCardCtrl_init()==True8)
	{
		tagSDCardCtrl.eProcessState = SDCARDCTRL_IDLE;
		tagSDCardCtrl.pcbfCallbackList = pcbfCallbackList;
		tagSDCardCtrl.ulMileSeconds = 0;
		// Allocating memory for utterance
		tagSDCardCtrl.dpUtteranceFrame = malloc(sizeof(double)*UTTERANCE_MAX_FRAME_SIZE);
		for (iCounter=0;iCounter<UTTERANCE_MAX_FRAME_SIZE;iCounter++)
			tagSDCardCtrl.dpUtteranceFrame[iCounter] = malloc(sizeof(double)*UTTERANCE_DIM_FEATURE);

		bResult = True8;
	}
	return bResult;
}

/**
	SDCardCtrl delete instance. This is the method that deletes the SDCardCtrl class instance.

    @param the class handle
    @return - void
    @author edielson
    @date   02/09/2014
*/
void SDCardCtrl_delete(void)
{
	int iCounter;
	// Freeing the utterance
	for (iCounter=0;iCounter<UTTERANCE_MAX_FRAME_SIZE;iCounter++)
	  free(tagSDCardCtrl.dpUtteranceFrame[iCounter]);
	free(tagSDCardCtrl.dpUtteranceFrame);
}

/**
	SDCardCtrl start. This is the method that starts process of reading frames.

    @param the class handle
    @return - void
    @author edielson
    @date   02/09/2014
*/
void SDCardCtrl_start(void)
{
	printf("Opening file %s...",UTTERANCE_FILE_NAME);
	if(f_open(&tagSDCardCtrl.fileHandler,(char*)UTTERANCE_FILE_NAME, FA_READ) == FR_OK)
	{
		printf("Success!\n",UTTERANCE_FILE_NAME);

		tagSDCardCtrl.iTotalBytes = sizeof(tagSDCardCtrl.fileHandler.fsize);
		tagSDCardCtrl.iFrameSize = UTTERANCE_DIM_FEATURE*sizeof(double);
		tagSDCardCtrl.iNumberFrames = tagSDCardCtrl.fileHandler.fsize/tagSDCardCtrl.iFrameSize;
		tagSDCardCtrl.iFrameCounter=0;
		printf("Number of frames: %d\n",tagSDCardCtrl.iNumberFrames);
		tagSDCardCtrl.eProcessState = SDCARDCTRL_PROCESSING;
		printf("[%2.2f s] Starting...\n",(double)tagSDCardCtrl.ulMileSeconds*TIME_RESOLUTION);
	}
	else
	{
		printf("Couldn't open file %s",UTTERANCE_FILE_NAME);
	}
}

/**
	SDCardCtrl stop. This is the method that stops the process of reading frames.

    @param void
    @return - void
    @author edielson
    @date   02/09/2014
*/
void SDCardCtrl_stop(void)
{
	f_close(&tagSDCardCtrl.fileHandler);
	tagSDCardCtrl.eProcessState = SDCARDCTRL_IDLE;
	printf("[%2.2f s] Done!\n",(double)tagSDCardCtrl.ulMileSeconds*TIME_RESOLUTION);
}
/**
	SDCardCtrl init. This is the method that initializes the SD card.

    @param the class handle
    @return - void
    @author edielson
    @date   02/09/2014
*/
static Bool8 SDCardCtrl_init(void)
{
	Bool8 bResult = False8;
	DSTATUS stat;
	DWORD p2;
	WORD w1;
	BYTE res, b1;
	DIR dir;

	/*Initializing SD Card*/
	SDCardCtrl_initSSP();

	SysTick_Config(SystemCoreClock / 100);
//	SysTick_Config(SystemCoreClock / 1000);

	printf("Starting MMC/SD.....\n");

	Timer0_Wait(500);

	stat = disk_initialize(0);
	if(stat & STA_NODISK)
	{
		printf("MMC: No Disk\n");
	}
	if(stat & STA_NOINIT)
	{
		printf("MMC: Not initialized\n");
	}
	if(stat == 0)
	{
		printf("MMC: Initialized\n");

		if (disk_ioctl(0, GET_SECTOR_COUNT, &p2) == RES_OK)
		{
			printf("Drive size: %ld \n", p2);
		}
		if (disk_ioctl(0, GET_SECTOR_SIZE, &w1) == RES_OK)
		{
				printf("Sector size: %d \n", w1);
		}

		if (disk_ioctl(0, GET_BLOCK_SIZE, &p2) == RES_OK)
		{
			printf("Erase block size: %ld \n", p2);
		}

		if (disk_ioctl(0, MMC_GET_TYPE, &b1) == RES_OK)
		{
			printf("Card type: %d \n", b1);
		}

		res = f_mount(0, &tagSDCardCtrl.Fatfs[0]);
		if (res != FR_OK)
		{
			printf("Failed to mount 0: %d \n", res);
		}
		else
		{
			res = f_opendir(&dir, "/");
			if (res)
			{
				printf("Failed to open /: %d \n", res);
			}
			else
			{
				printf("\nListing files and folders\r\n", res);
				do
				{
					res = f_readdir(&dir, &tagSDCardCtrl.Finfo);
					if ((res == FR_OK) && (tagSDCardCtrl.Finfo.fname[0]))
					{
						printf(&(tagSDCardCtrl.Finfo.fname[0]));
						printf("\n");
					}
				}while((res == FR_OK) && (tagSDCardCtrl.Finfo.fname[0]));
				printf("\r\n");
				bResult = True8;
			}
		}
	}
	return bResult;
}

/**
	SDCardCtrl process. This is the method that process the utterance.

    @param the class handle
    @return - void
    @author edielson
    @date   02/09/2014
*/
tenu_SDCardProcessState SDCardCtrl_process(void)
{
	UINT byteRead = 0;

	switch (tagSDCardCtrl.eProcessState)
	{
		case SDCARDCTRL_PROCESSING:
			if(tagSDCardCtrl.iFrameCounter < tagSDCardCtrl.iNumberFrames)
			{
				byteRead = 0;
				if(f_read(&tagSDCardCtrl.fileHandler, tagSDCardCtrl.dpUtteranceFrame[0], sizeof(double)*UTTERANCE_DIM_FEATURE, &byteRead)==FR_OK)
				{
					if(byteRead == tagSDCardCtrl.iFrameSize)
					{
						/*Calculating the cumulative P(O|GMM) for each frame*/
						if(tagSDCardCtrl.pcbfCallbackList->cbfProcessNewFrame)
							tagSDCardCtrl.pcbfCallbackList->cbfProcessNewFrame(tagSDCardCtrl.dpUtteranceFrame,UTTERANCE_MAX_FRAME_SIZE);

						tagSDCardCtrl.iTotalBytes-=byteRead;
						tagSDCardCtrl.iFrameCounter++;
					}
					else
					{
						printf("Bytes read different from the expected: %d != %d\r\n", byteRead, tagSDCardCtrl.iFrameSize);
						tagSDCardCtrl.eProcessState = SDCARDCTRL_ERROR;
					}
				}
				else
				{
					printf("Error reading file!\r\n");
					tagSDCardCtrl.eProcessState = SDCARDCTRL_ERROR;
				}
			}
			else
			{
				tagSDCardCtrl.eProcessState = SDCARDCTRL_ENDING;
			}
			break;
		case SDCARDCTRL_ENDING:
			/*callback function for ending process*/
			if(tagSDCardCtrl.pcbfCallbackList->cbfFinishProcess)
				tagSDCardCtrl.pcbfCallbackList->cbfFinishProcess();
			break;
		case SDCARDCTRL_ERROR:
			printf("Error, closing file");
			f_close(&tagSDCardCtrl.fileHandler);
			tagSDCardCtrl.eProcessState = SDCARDCTRL_IDLE;
			break;
		default:
			break;
	}
	return tagSDCardCtrl.eProcessState;
}
/**
	SDCardCtrl init. This is the method that initializes the SD card.

    @param the class handle
    @return - void
    @author edielson
    @date   02/09/2014
*/
static void SDCardCtrl_initSSP(void)
{
	SSP_CFG_Type SSP_ConfigStruct;
	PINSEL_CFG_Type PinCfg;

	/*
	 * Initialize SPI pin connect
	 * P0.7 - SCK;
	 * P0.8 - MISO
	 * P0.9 - MOSI
	 * P2.2 - SSEL - used as GPIO
	 */
	PinCfg.Funcnum = 2;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Portnum = 0;
	PinCfg.Pinnum = 7;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 8;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 9;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Funcnum = 0;
	PinCfg.Portnum = 2;
	PinCfg.Pinnum = 2;
	PINSEL_ConfigPin(&PinCfg);

	SSP_ConfigStructInit(&SSP_ConfigStruct);

	// Initialize SSP peripheral with parameter given in structure above
	SSP_Init(LPC_SSP1, &SSP_ConfigStruct);

	// Enable SSP peripheral
	SSP_Cmd(LPC_SSP1, ENABLE);

}

/**
	SDCardCtrl init. This is the method that initializes the SD card.

    @param the class handle
    @return - void
    @author edielson
    @date   02/09/2014
*/
DWORD get_fattime(void)
{
    return 0;
}

/**
	SDCardCtrl init. This is the method that initializes the SD card.

    @param the class handle
    @return - void
    @author edielson
    @date   02/09/2014
*/
void SysTick_Handler(void)
{
    disk_timerproc();
    tagSDCardCtrl.ulMileSeconds++;
}
