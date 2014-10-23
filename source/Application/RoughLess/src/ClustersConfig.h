/*
 * ClustersConfig.h
 *
 *  Created on: 22/10/2014
 *      Author: edielson
 */

#ifndef CLUSTERSCONFIG_H_
#define CLUSTERSCONFIG_H_

#include "GMMLib.h"

#define CLUSTER_GAUSSIAN_NUMBER	8
#define CLUSTER_FEATURE_VEC_DIM	2

static ttag_mixture ptagGMMCluster1[] =
{
		/*Gaussian 1*/
		{0.118490,{-18.416565,-17.003494},{24.555412,27.100152}},
		/*Gaussian 2*/
		{0.120783,{-8.452489,-8.813692},{36.792546,17.752955}},
		/*Gaussian 3*/
		{0.133748,{-9.435669, 0.430009},{5.706564,5.204678}},
		/*Gaussian 4*/
		{0.105594,{-6.924192, -5.042134},{19.673234, 6.195827}},
		/*Gaussian 5*/
		{0.206736,{-6.040366, 4.420611},{5.077800, 5.564422}},
		/*Gaussian 6*/
		{0.141650,{-1.519570, 8.797092},{8.732912, 6.934726}},
		/*Gaussian 7*/
		{0.129087,{2.236190, -4.219155},{19.267317, 7.504733}},
		/*Gaussian 8*/
		{0.043913,{11.112560, -1.280352},{6.674525, 5.430487}}
};

static ttag_mixture ptagGMMCluster2[] =
{
		/*Gaussian 1*/
		{0.049103,{-0.873279, -31.614118},{25.749048, 43.546445}},
		/*Gaussian 2*/
		{0.057617,{6.587727, -36.499349},{29.292906, 14.765361}},
		/*Gaussian 3*/
		{0.125409,{-2.244924, -8.428135},{12.323667, 14.046788}},
		/*Gaussian 4*/
		{0.060249,{-0.871078, -26.416334},{33.841804, 15.274328}},
		/*Gaussian 5*/
		{0.133199,{-10.784135, -1.472004},{7.258475, 17.520104}},
		/*Gaussian 6*/
		{0.167083,{3.862718, -2.117228},{10.973416, 9.045722}},
		/*Gaussian 7*/
		{0.266034,{2.236190, -4.219155},{7.452281, 13.872032}},
		/*Gaussian 8*/
		{0.141306,{-1.498711, 7.973665},{13.254598, 10.311649}}
};

static ttag_mixture ptagGMMCluster3[] =
{
		/*Gaussian 1*/
		{0.006245,{3.130599, -35.853100},{17.850471, 14.025726}},
		/*Gaussian 2*/
		{0.041978,{17.394654, -38.880055},{18.767299, 10.016055}},
		/*Gaussian 3*/
		{0.062589,{15.611511, -45.357318},{46.748300, 18.784969}},
		/*Gaussian 4*/
		{0.049226,{27.253907, -45.447988},{39.844006, 15.496788}},
		/*Gaussian 5*/
		{0.096413,{-0.187662, -9.032226},{8.475427, 10.125859}},
		/*Gaussian 6*/
		{0.127341,{4.087088, -3.993064},{6.114496, 7.086124}},
		/*Gaussian 7*/
		{0.011163,{-17.382218, -10.668845},{83.474476, 17.054929}},
		/*Gaussian 8*/
		{0.605045,{-3.687806, 3.869862},{22.101639, 16.299648}}
};

#endif /* CLUSTERSCONFIG_H_ */