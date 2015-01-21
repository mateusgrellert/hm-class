/* 
 * File:   TComCycleMonitor.h
 * Author: grellert
 *
 * Created on April 16, 2014, 1:21 PM
 */

#ifndef TCOMCYCLEMONITOR_H
#define	TCOMCYCLEMONITOR_H


#define EN_CYCLE_MONITOR 1

#include <cstdlib>
#include <cstdio>
#include <vector>
#include "TComDataCU.h"
using namespace std;

typedef pair<string, vector<double> > cycle_triplet; // (func, cycle_ini, cycle_average, ncalls)

class TComCycleMonitor {
public:
    static vector<cycle_triplet> cycleVector;
    static vector< vector <pair <double, double> > >  CTUCycleVector; // curr_frame, average
    static ofstream cycleResults;
    static uint currFrame;
    static double minTime, maxTime, lastTime;
    static int CTUIdx;
    static struct timeval timer;
    
    TComCycleMonitor();
    static void init(int picW, int picH);
    static cycle_triplet createTriplet(string);
    static void setInitCycle(string);
    static void setEndCycle(string);
    static void setInitCTUCycle(TComDataCU* cu);
    static void setEndCTUCycle(TComDataCU* cu);

    static void reportCycles();
    static void updateMinMaxLast(double t);
};

#endif	/* TCOMCYCLEMONITOR_H */

