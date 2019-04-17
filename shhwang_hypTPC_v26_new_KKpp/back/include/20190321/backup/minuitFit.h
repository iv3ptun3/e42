#ifndef __MINUITFIT_H__
#define __MINUITFIT_H__
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <unistd.h>
#include "track.h"

//#include "tpcParameter.h"

#define NUM_PARA_RK 5
#define MAX_ITERATION 1000
#define MINUIT_TOLERANCE 1.

//void minuitInit(double printLevel);
int minuitFit(double* initPara, float* finalChi2, double* para,
              Track* aTrack, int sector, int lay);
//void minuitInit(double printLevel);
//TpcParam* tpcPar4minuit; /*NTPC*/

#endif /* __MINUITFIT_H__ */
