/*
 * GMMLib.h
 *
 *  Created on: 20/10/2014
 *      Author: edielson
 */

#ifndef GMMLIB_H_
#define GMMLIB_H_

#include "ICommon.h"

/*********************************************************
    public constants.
*********************************************************/

#define GMMLIB_MAX_FEATURE_VEC_DIM		10

/*********************************************************
    public types.
*********************************************************/
typedef struct tag_mixture
{
	double 	dMixtureWeight;   										// mixture weight
	double 	dpGaussianMean[GMMLIB_MAX_FEATURE_VEC_DIM];  		// gaussian mean
	double 	dpGaussianCovariance[GMMLIB_MAX_FEATURE_VEC_DIM]; 	// covariance matrix (only the diagonal elements are stored)
}ttag_mixture;

/*********************************************************
    public operations.
*********************************************************/
Handle 	GMMLib_new(ttag_mixture* ptagGMM, int iDimFeatureVector, int iGaussiansNumber);
void 	GMMLib_delete(Handle hClassHandler);
double 	GMMLib_aposteriori(Handle hClassHandler, double** dpObsVectorSequence, int iFramesNumber);
int 	GMMLib_getFeatureVectorDim(Handle hClassHandler);
void 	GMMLib_init(Handle hClassHandler);
double 	GMMLib_getProbability(Handle hClassHandler);

#endif /* GMMLIB_H_ */
