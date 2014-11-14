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

class TComClassifier{
public:
    static double **grad, **magnitude;
    static double **avg_grad, **avg_magnitude;

    static ofstream outSobelGrad, outSobelMagn;
    static int picHeight, picWidth;
    
    TComClassifier();
    static void init(int, int);
    static void calcFrameSobel(TComPicYuv* recFrame, int poc);
    static void printSobelFrames(int);

};


#endif	/* TCOMCLASSIFIER_H */

