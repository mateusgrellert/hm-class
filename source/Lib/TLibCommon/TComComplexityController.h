/* 
 * File:   TComComplexityController.h
 * Author: grellert
 *
 * Created on February 4, 2015, 1:31 PM
 */

#ifndef TCOMCOMPLEXITYCONTROLLER_H
#define	TCOMCOMPLEXITYCONTROLLER_H

#define EN_COMPLEXITY_MANAGEMENT 1
#define REFRESH_PERIOD 1 // period when controller updates and bugedting occurs
#include <fstream>

using namespace std;

#define abs(x) ((x) > 0? (x) : -(x))

static const double depthWeitghts[6] = {0.36, 0.6, 0.77, 2.75, 1.67, 1.29};

class TComComplexityController{
public:
    static double SP, SP_factor, PV;
    static double KP, KI, KD;
    static double PIOut, PIDOut;
    static double prevError, diffError, acumError, avgPV;
    static int budgetAlgorithm,currBudgetDepth, frameCounter;

    static ofstream controlFile;
    
    TComComplexityController();
    static void init(double sp, int budgetAlg);
    static void setPV(double v) { PV += v;};
    static void setSP()         { SP = PV*SP_factor; };
    static void setBudgetAlgorithm(int i) { budgetAlgorithm = i; };
    static void setPIDConstants(double, double, double);
    static void calcPI(int poc);
    static void calcPID(int poc);
    static void calcBudget();
    static void budgetAlgorithm0();
    static void budgetAlgorithm1();
    static void budgetAlgorithm2();
    static bool isConstrained(int poc);
    static bool terminateCTU(unsigned int depth);

};

#endif	/* TCOMCOMPLEXITYCONTROLLER_H */

