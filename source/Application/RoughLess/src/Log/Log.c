

#include "Log.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#ifdef LOG_CONSOLE

#else
#include "lpc17xx_uart.h"
#include "lpc17xx_pinsel.h"

#define UART_DEV LPC_UART3
#define UART_INTERRUPT      UART3_IRQn
#endif

/*********************************************************
    private constants.
*********************************************************/

#define LOG_PACKET_SIZE							300   /*  */

#ifdef LOG_CONSOLE
	#define LOG_IP_REFERENCE					0xC0A80101  /* 192.168.1.1 */
#endif

/*********************************************************
    private types.
*********************************************************/


/*********************************************************
    private variable.
*********************************************************/

/*********************************************************
    private operations.
*********************************************************/
unsigned long Log_getDateTime(char* pcDateTimeFormat, unsigned long ulCurrentSize);
unsigned long Log_getHostIp(char* pcIpFormat, unsigned long ulCurrentSize);


/**
	Get date time in application, and return string in syslog format

    @param pointer to save string
    @param Current string size
    @return string size
    @author gabriels
    @date   15/10/2014
*/
unsigned long Log_getDateTime(char* pcDateTimeFormat, unsigned long ulCurrentSize)
{
	unsigned char ucSecond = 0;
	unsigned char ucMinute = 0;
	unsigned char ucHour = 12;
	unsigned char ucDay = 23;
	unsigned char ucMonth = 10;
	unsigned char ucYear = 14;

	unsigned long ulSize;

	/* Insert rtc get time */


	ulSize = snprintf((char*)pcDateTimeFormat, (LOG_PACKET_SIZE - ulCurrentSize),"%2.2d-%2.2d-%2.2d %2.2d:%2.2d:%2.2d ",
			ucYear, ucMonth, ucDay, ucHour, ucMinute, ucSecond);

	return ulSize;

}
/**
	Get host ip in application, and return string ip in syslog format

    @param pointer to save string
    @param Current string size
    @return string size
    @author gabriels
    @date   15/10/2014
*/
unsigned long Log_getHostIp(char* pcIpFormat, unsigned long ulCurrentSize)
{
	unsigned long ulSize;
	unsigned char ucIp[4] = {192, 168, 1,1};

	/* Insert get host ip */

	ulSize = snprintf((char*)pcIpFormat, (LOG_PACKET_SIZE - ulCurrentSize),"%d.%d.%d.%d ",
				ucIp[0], ucIp[1], ucIp[2], ucIp[3]);

	return ulSize;
}


/*********************************************************
    Operations implementation
*********************************************************/
/**
	Create instance

    @param void
    @return void
    @author gabriels
    @date   15/10/2014
*/
void Log_new(void)
{
	return;
}
/**
	Delete instance

    @param void
    @return void
    @author gabriels
    @date   15/10/2014
*/
void Log_delete(void)
{
	return;
}

/**
	Initialize log app

    @param void
    @return void
    @author gabriels
    @date   15/10/2014
*/
void Log_init(void)
{
	#ifdef LOG_CONSOLE

	#else
	PINSEL_CFG_Type PinCfg;
	UART_CFG_Type uartCfg;

	/* Initialize UART3 pin connect */
	PinCfg.Funcnum = 2;
	PinCfg.Pinnum = 0;
	PinCfg.Portnum = 0;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 1;
	PINSEL_ConfigPin(&PinCfg);

	uartCfg.Baud_rate = 115200;
	uartCfg.Databits = UART_DATABIT_8;
	uartCfg.Parity = UART_PARITY_NONE;
	uartCfg.Stopbits = UART_STOPBIT_1;

	UART_Init(UART_DEV, &uartCfg);

	UART_TxCmd(UART_DEV, ENABLE);
	#endif
}

/**
	Print log in console or ethernet

    @param Log Facility
    @param Log Severity
    @param message
    @return void
    @author gabriels
    @date   15/10/2014
*/
void  Log_print(tenu_LogFacility enuFacility, tenu_LogSeverity enuSeverity, const char* cString, ...)
{
	unsigned long  ulMsgSize;
	char    cMessage[LOG_PACKET_SIZE];

	#ifdef LOG_DISABLE

	#else
		va_list arguments;
		va_start( arguments, cString );

		ulMsgSize = snprintf((char*)cMessage, LOG_PACKET_SIZE,"<%d> ", ((enuFacility*8) + enuSeverity));
		ulMsgSize += Log_getDateTime((char*)&cMessage[ulMsgSize], ulMsgSize);
		ulMsgSize += Log_getHostIp((char*)&cMessage[ulMsgSize], ulMsgSize);
		ulMsgSize += vsnprintf((char*)&cMessage[ulMsgSize], (LOG_PACKET_SIZE-ulMsgSize), (char*)cString, arguments);

		snprintf((char*)&cMessage[ulMsgSize], (LOG_PACKET_SIZE - ulMsgSize),(char*)"\n");
		#ifdef LOG_CONSOLE
			printf((char*)cMessage);
		#else
			UART_SendString(UART_DEV,(uint8_t*)cMessage);
		#endif
	#endif
}

