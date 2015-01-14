
#include <vector>
#include <cstdlib>
#include <fstream>
#include <cstdio>
#include <sys/time.h>
#include <sstream>  

#include "TComCycleMonitor.h"
using namespace std;

double TComCycleMonitor::minTime;
double TComCycleMonitor::maxTime;
double TComCycleMonitor::lastTime;
int TComCycleMonitor::CTUIdx;

vector<cycle_triplet> TComCycleMonitor::cycleVector;
ofstream TComCycleMonitor::cycleResults;
uint TComCycleMonitor::currFrame;
struct timeval TComCycleMonitor::timer;

TComCycleMonitor::TComCycleMonitor() {

}


void TComCycleMonitor::init(){
    minTime = -1.0;
    maxTime = 99999.99;
    lastTime = -1.0;
    CTUIdx = 0;
    
    if(not(cycleResults.is_open())){        
        cycleResults.open("function_cycle_results.csv",ofstream::out);
        cycleResults << "\t Total Time (us)\tCalls (kCalls)"<< endl;

    }
    
    cycleVector.clear();
}

cycle_triplet TComCycleMonitor::createTriplet(string func){
    cycle_triplet triplet;
    vector<double> ret;
    for(int i = 0; i < 3; i++)
        ret.push_back(0.0);
    triplet = make_pair(func, ret);
    return triplet;
}

void TComCycleMonitor::setInitCycle(string func){
    vector<cycle_triplet>::iterator it;
    gettimeofday(&timer,NULL);
    
    if(func == "CTU"){
        ostringstream convert;
        convert << CTUIdx;
        
        func += "_" + convert.str();
        CTUIdx++;
    }
    
     // tries to find the function in the vector, if not, create node
    for(it = cycleVector.begin(); it != cycleVector.end(); it++){
        if (it->first == func){
            it->second[0] = timer.tv_sec +  timer.tv_usec*1.0/1000000;

            break;
        }
    }
    if(it == cycleVector.end()){
        cycle_triplet triplet = createTriplet(func);
        triplet.second[0] = timer.tv_sec +  timer.tv_usec*1.0/1000000;
        
        cycleVector.push_back(triplet);
    }
    
}

void TComCycleMonitor::setEndCycle(string func){
    vector<cycle_triplet>::iterator it;
    double diff_time;

    gettimeofday(&timer,NULL);

    for(it = cycleVector.begin(); it != cycleVector.end(); it++){
        if (it->first == func){
            double end_time = timer.tv_sec +  timer.tv_usec*1.0/1000000;
            diff_time = end_time - it->second[0];
            updateMinMaxLast(diff_time);
            it->second[2]++;
            it->second[1] += diff_time;
            break;
        }
    }
}

void TComCycleMonitor::updateMinMaxLast(double t){
    minTime = (t < minTime ? t : minTime);
    maxTime = (t > maxTime ? t : maxTime);
    lastTime = t;

}

void TComCycleMonitor::reportCycles(){
    vector<cycle_triplet>::iterator it;
    cycleResults << "Frame\t" << currFrame << endl;
    for(it = cycleVector.begin(); it != cycleVector.end(); it++){
        cycleResults << it->first << "\t" << it->second[1]*1000000<< "\t" << it->second[2]/1000 << endl;
    }
    cycleResults << endl;
    currFrame++;
    CTUIdx = 0;
}