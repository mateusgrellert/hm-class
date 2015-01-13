/* 
 * File:   TComClassifier.h
 * Author: grellert
 *
 * Created on November 11, 2014, 11:26 AM
 */

#ifndef TCOMCLASSIFIER_H
#define	TCOMCLASSIFIER_H

#include "TLibEncoder/TEncCu.h"
#include "TComDataCU.h"
#include <math.h>

#define EN_COMPLEXITY_MANAGEMENT 1
#define MAX_SOBEL 7.0

class TComClassifier{
public:
    static double **grad, **magnitude;
    static double **avg_grad, **avg_magnitude;
    static double max_grad;
    
    static ofstream outSobelGrad, outSobelMagn;
    static int picHeight, picWidth;
    static int **predictedDepthMap;
    static int **actualDepthMap;
    
    TComClassifier();
    static void init(int, int);
    static void calcFrameSobel(TComPicYuv* recFrame, int poc);
    static void calcCTUSobel(TComDataCU* cu, TComPicYuv* origFrame);
    static void xcalcCTUDivisions(double local_max_grad, int cuX, int cuY, int cuWidth, int cuHeight);
    static void setEncCTUDepth(TComDataCU* pu, UInt absIdx);
    static void printSobelFrames(int);
    static void printHitMissCTUPrediction();
};


#endif	/* TCOMCLASSIFIER_H */

