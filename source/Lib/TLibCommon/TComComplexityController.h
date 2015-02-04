/* 
 * File:   TComComplexityController.h
 * Author: grellert
 *
 * Created on February 4, 2015, 1:31 PM
 */

#ifndef TCOMCOMPLEXITYCONTROLLER_H
#define	TCOMCOMPLEXITYCONTROLLER_H

#define abs(x) ((x) > 0? (x) : -(x))

class TComComplexityController{
public:
    static double SP, PV;
    static double KP, KI, KD;
    static double PIOut, PIDOut;
    static double prevError, diffError, acumError;
    
    TComComplexityController();
    static void init(double sp);
    static void setPV(double v) { PV = v; };
    static void calcPI();
    static void calcPID();
    static bool isConstrained(int poc);


};

#endif	/* TCOMCOMPLEXITYCONTROLLER_H */

