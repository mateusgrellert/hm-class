/* 
 * File:   TComComplexityController.h
 * Author: grellert
 *
 * Created on February 4, 2015, 1:31 PM
 */

#ifndef TCOMCOMPLEXITYCONTROLLER_H
#define	TCOMCOMPLEXITYCONTROLLER_H

#define EN_COMPLEXITY_MANAGEMENT 1

#include <fstream>

using namespace std;

#define abs(x) ((x) > 0? (x) : -(x))

class TComComplexityController{
public:
    static double SP, SP_factor, PV;
    static double KP, KI, KD;
    static double PIOut, PIDOut;
    static double prevError, diffError, acumError;
    static int budgetAlgorithm,currBudgetDepth;

    static ofstream controlFile;
    
    TComComplexityController();
    static void init(double sp, int budgetAlg);
    static void setPV(double v) { PV = v; };
    static void setSP(double v) { SP = v*SP_factor; };
    static void setBudgetAlgorithm(int i) { budgetAlgorithm = i; };

    static void calcPI(int poc);
    static void calcPID(int poc);
    static void calcBudget();
    static void budgetAlgorithm0();
    static void budgetAlgorithm1();
    static bool isConstrained(int poc);
    static bool terminateCTU(unsigned int depth);

};

#endif	/* TCOMCOMPLEXITYCONTROLLER_H */

