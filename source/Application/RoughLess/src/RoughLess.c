/*
===============================================================================
 Name        : RoughLess.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>
#include <stdio.h>
#include <stdlib.h>

// TODO: insert other include files here
#include "GMMLib.h"
#include "ClustersConfig.h"
#include "SDCardCtrl.h"

// TODO: insert other definitions and declarations here
static void RoughLess_processNewFrame(double** dpFrame, int iFramesNumber);
static void RoughLess_finishProcess(void);

static Handle hCluster1 = NULL;
//static Handle hCluster2 = NULL;
//static Handle hCluster3 = NULL;

int main(void)
{
	tcbf_SDCardCtrlCallbackList cbfSDCardCtrlCallbackList;
	cbfSDCardCtrlCallbackList.cbfProcessNewFrame = RoughLess_processNewFrame;
	cbfSDCardCtrlCallbackList.cbfFinishProcess = RoughLess_finishProcess;

	/*Creating new instance for GMMLib class*/
	hCluster1 = GMMLib_new(ptagGMMCluster1,CLUSTER_FEATURE_VEC_DIM,CLUSTER_GAUSSIAN_NUMBER);
	//hCluster2 = GMMLib_new(ptagGMMCluster2,CLUSTER_FEATURE_VEC_DIM,CLUSTER_GAUSSIAN_NUMBER);
	//hCluster3 = GMMLib_new(ptagGMMCluster3,CLUSTER_FEATURE_VEC_DIM,CLUSTER_GAUSSIAN_NUMBER);

	/*Initializing the GMMLib class reseting the probability for 0*/
	GMMLib_init(hCluster1);
	//GMMLib_init(hCluster2);
	//GMMLib_init(hCluster3);

	/*Initializing SD card*/
	if(SDCardCtrl_createInstance(&cbfSDCardCtrlCallbackList) == True8)
	{
		SDCardCtrl_start();

		while(SDCardCtrl_process() != SDCARDCTRL_IDLE);

		SDCardCtrl_delete();
	}

	GMMLib_delete(hCluster1);

	return 0 ;
}

static void RoughLess_processNewFrame(double** dpFrame, int iFramesNumber)
{
	double p1, p2, p3; 	// P(O|M) for the GMM

	p1=GMMLib_aposteriori(hCluster1,dpFrame,iFramesNumber);
	//p2=GMMLib_aposteriori(hCluster2,dpFrame,iFramesNumber);
	//p3=GMMLib_aposteriori(hCluster1,dpFrame,iFramesNumber);

	//printf("Cluster 1: P(O|GMM) = %f\n",p1);
	//printf("Cluster 2: P(O|GMM) = %f\n",p2);
	//printf("Cluster 3: P(O|GMM) = %f\n",p3);
}

static void RoughLess_finishProcess(void)
{
	printf("Cluster1: final P(O|GMM)=%f\n",GMMLib_getProbability(hCluster1));
	//printf("Cluster1: final P(O|GMM)=%f\n",GMMLib_getProbability(hCluster2));
	//printf("Cluster1: final P(O|GMM)=%f\n",GMMLib_getProbability(hCluster3));

	SDCardCtrl_stop();
}
