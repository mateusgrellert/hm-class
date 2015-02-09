/* 
 * File:   TComComplexityController.h
 * Author: grellert
 *
 * Created on February 4, 2015, 1:31 PM
 */

#ifndef TCOMCOMPLEXITYCONTROLLER_H
#define	TCOMCOMPLEXITYCONTROLLER_H

#include <fstream>

using namespace std;

#define abs(x) ((x) > 0? (x) : -(x))

class TComComplexityController{
public:
    static double SP, PV;
    static double KP, KI, KD;
    static double PIOut, PIDOut;
    static double prevError, diffError, acumError;
    static int currBudgetDepth;

    static ofstream controlFile;
    
    TComComplexityController();
    static void init(double sp);
    static void setPV(double v) { PV = v; };
    static void calcPI(int poc);
    static void calcPID(int poc);
    static void calcBudget();
    static bool isConstrained(int poc);
    static bool terminateCTU(unsigned int depth);

};

#endif	/* TCOMCOMPLEXITYCONTROLLER_H */

