/*
===============================================================================
 Name        : GMMLib.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/
#include <stdio.h>
#include <stdlib.h>

#include "math.h"
#include "string.h"

#include "GMMLib.h"
#include "GMMLibConfig.h"

#define PI 2*asin(1)
#define INF 1.5e308

typedef struct tag_GMMLib
{
	ttag_mixture* ptagGMMElements; // stores the elements of the GMM

	int iGaussiansNumber; 		// number of gaussians in the mixture
	int iDimFeatureVector;      // dimension of feature vectors

	double dProbability;
}ttag_GMMLib;


/*********************************************************
    private constants.
*********************************************************/

/*********************************************************
    private types.
*********************************************************/

/*********************************************************
    private attributes.
*********************************************************/

/*********************************************************
    private operations.
*********************************************************/
//static void 	GMMLib_loadGMM(Handle hClassHandler, char* GMMFilename);
static double 	GMMLib_mixture(double* x, ttag_mixture* l, int nGaussians, int dim);
static double 	GMMLib_gaussFunction(double* x,  double* media, double* var, int ordem);
/*********************************************************
    Operations implementation
*********************************************************/

/**
	GMMLib constructor. This is the constructor method, it allocates memory
	for the current object and initialize it. In case of a failure the method
	returns zero, otherwise it returns the class handle.

    @param void
    @return - the class handle
    @author edielson
    @date   02/09/2014
*/
Handle GMMLib_new(ttag_mixture* ptagGMM, int iDimFeatureVector, int iGaussiansNumber)
{
	ttag_GMMLib* ptagGMMLib = (ttag_GMMLib*)malloc(sizeof(ttag_GMMLib));
	int i,j;
	if(ptagGMMLib)
	{
		ptagGMMLib->ptagGMMElements = ptagGMM;
		ptagGMMLib->iDimFeatureVector = iDimFeatureVector;
		ptagGMMLib->iGaussiansNumber = iGaussiansNumber;
		ptagGMMLib->dProbability=0;

		/*TODO remover*/
//		for (i=0;i<iGaussiansNumber;i++)
//		{
//			printf("Gaussian %d:\n",i+1);
//			printf("p = %f\n",ptagGMMLib->ptagGMMElements[i].dMixtureWeight);
//			puts("m = ");
//			for (j=0;j<iDimFeatureVector;j++)
//			  printf("%f\n",ptagGMMLib->ptagGMMElements[i].dpGaussianMean[j]);
//			puts("s = ");
//			for (j=0;j<iDimFeatureVector;j++)
//			  printf("%f\n",ptagGMMLib->ptagGMMElements[i].dpGaussianCovariance[j]);
//			puts("\n");
//		}
	}
	return (Handle)ptagGMMLib;
}


/**
	GMMLib destructor. This is the destructor method, it deallocates the memory
	reserved for the object. If the method is executed correctly the return value
	is true.

    @param the class handle
    @return - void
    @author edielson
    @date   02/09/2014
*/
void GMMLib_delete(Handle hClassHandler)
{
	if(!hClassHandler) return;
	ttag_GMMLib* ptagGMMLib = (ttag_GMMLib*)hClassHandler;
	free(ptagGMMLib);
}

/**
	GMMLib initializer. This is the method that initializes the probability.

    @param the class handle
    @return - void
    @author edielson
    @date   02/09/2014
*/
void GMMLib_init(Handle hClassHandler)
{
	if(!hClassHandler) return;
	ttag_GMMLib* ptagGMMLib = (ttag_GMMLib*)hClassHandler;
	ptagGMMLib->dProbability=0;
}

/**
	GMMLib get probability. This is the method that returns the resulted probability.

    @param the class handle
    @return - void
    @author edielson
    @date   02/09/2014
*/
double GMMLib_getProbability(Handle hClassHandler)
{
	if(!hClassHandler) return;
	ttag_GMMLib* ptagGMMLib = (ttag_GMMLib*)hClassHandler;
	return ptagGMMLib->dProbability;
}
/**
	GMMLib destructor. This is the destructor method, it deallocates memory
	for the current object and deinitialize it.

    @param the class handle
    @return - void
    @author edielson
    @date   02/09/2014
*/
double GMMLib_aposteriori(Handle hClassHandler, double** dpObsVectorSequence, int iFramesNumber)
{
	if(!hClassHandler) return 0;
	ttag_GMMLib* ptagGMMLib = (ttag_GMMLib*)hClassHandler;

	double p; 		// P(i|X,M)
	int t; 			// counter

	for (t=0;t<iFramesNumber;t++)
	{
		p = GMMLib_mixture(dpObsVectorSequence[t],ptagGMMLib->ptagGMMElements,ptagGMMLib->iGaussiansNumber,ptagGMMLib->iDimFeatureVector);
		ptagGMMLib->dProbability += log10(p);
	}
	return ptagGMMLib->dProbability;
}

/**
	GMMLib destructor. This is the destructor method, it deallocates memory
	for the current object and deinitialize it.

    @param the class handle
    @return - void
    @author edielson
    @date   02/09/2014
*/
int GMMLib_getFeatureVectorDim(Handle hClassHandler)
{
	if(!hClassHandler) return 0;
	ttag_GMMLib* ptagGMMLib = (ttag_GMMLib*)hClassHandler;
	return ptagGMMLib->iDimFeatureVector;
}

/**
	GMMLib destructor. This is the destructor method, it deallocates memory
	for the current object and deinitialize it.

    @param the class handle
    @return - void
    @author edielson
    @date   02/09/2014
*/
static double GMMLib_mixture(double *x, ttag_mixture* l, int nGaussians, int dim)
{
	double p; // P(O|M)
	int j;    // counter

	p = 0.0;
	for (j=0;j<nGaussians;j++)
	  p += l[j].dMixtureWeight * GMMLib_gaussFunction(x,l[j].dpGaussianMean,l[j].dpGaussianCovariance,dim);

	return p;
}

/**
	GMMLib destructor. This is the destructor method, it deallocates memory
	for the current object and deinitialize it.

    @param the class handle
    @return - void
    @author edielson
    @date   02/09/2014
*/
static double GMMLib_gaussFunction(double* x,  double* media, double* var, int ordem)
{
	 // Declaracao das variaveis locais
	double aux=0.0,aux1,aux2 = 0.0; // variaveis auxiliares
	double det; // determinante da matriz de covariancia
	double dif; // var aux p/ o calculo da gaussiana
	int i; // contador
	double prob=0.0; // probabilidade do vetor dada a distribuicao

	// (2*pi)^(ordem/2)
	aux1 = 2.0*PI;
	aux2 = ordem / 2.0;
	aux1 = pow(aux1,aux2);
	//aux1 = exp(aux2*log(aux1));

	// Calculando determinante da matriz de covariancia
	det = 1.0;
	for (i=0;i<ordem;i++)
	det *= var[i];

	if (det != 0)
	{
		aux2 = fabs(det);
		aux2 = pow(aux2,0.5);

		for (i=0;i<ordem;i++)
		{
			dif = x[i] - media[i];
			aux += (dif*dif)/var[i];
		}
		aux *= (-0.5);
		aux = exp(aux);

		prob = aux/(aux1*aux2);
	}
	return(prob);
}
