/*
 * SDCardCtrl.h
 *
 *  Created on: 22/10/2014
 *      Author: edielson
 */

#ifndef SDCARDCTRL_H_
#define SDCARDCTRL_H_

#include "ICommon.h"


typedef enum tenu_SDCardProcessState
{
	SDCARDCTRL_PROCESSING,
	SDCARDCTRL_ENDING,
	SDCARDCTRL_ERROR,
	SDCARDCTRL_IDLE
}tenu_SDCardProcessState;

/* Virtual function list */
typedef struct cbf_SDCardCtrlCallbackList
{
	/* Callback templates */
	void (*cbfProcessNewFrame)(double** dpFrame, int iNumberFrames);
	void (*cbfFinishProcess)(void);
}tcbf_SDCardCtrlCallbackList;

Bool8 	SDCardCtrl_createInstance(tcbf_SDCardCtrlCallbackList* pcbfCallbackList);
void 	SDCardCtrl_delete(void);
void 	SDCardCtrl_start(void);
void 	SDCardCtrl_stop(void);
tenu_SDCardProcessState SDCardCtrl_process(void);


#endif /* SDCARDCTRL_H_ */
