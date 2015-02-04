#include "TComComplexityController.h"

#include <iostream>
#include <fstream>

using namespace std;


double TComComplexityController::SP;
double TComComplexityController::PV;
double TComComplexityController::KP;
double TComComplexityController::KI;
double TComComplexityController::KD;
double TComComplexityController::PIOut;
double TComComplexityController::PIDOut;
double TComComplexityController::prevError;
double TComComplexityController::diffError;
double TComComplexityController::acumError;

ofstream TComComplexityController::controlFile;

TComComplexityController::TComComplexityController() {
    return;
}

void TComComplexityController::init(double sp){
    SP = sp;
    SP = 10.0;
    acumError = prevError = diffError = 0.0;
    KP =  KI = KD = 1.0;
    KP = 0.5;
    KI = 0.5;
    controlFile.open("controlOut.csv",ofstream::out);
    controlFile << "KP\t" << KP << "\tKI\t" << KI << "\tKD\t" << KD << endl << "SP\tPV\tControl Output" << endl;

}

void TComComplexityController::calcPI(int poc){
    
    if ((poc % 30) == 0){
        SP= (1.0 - 0.2*(poc/30))*10.0;
       // acumError = 0;
    }
    
    double error = SP - PV;
    acumError += error;
    diffError = prevError - error;
    prevError = error;
    
    PIOut = KP*error + KI*acumError;
    controlFile << SP << "\t" << PV << "\t" << PIOut + SP << endl;
    
}

void TComComplexityController::calcPID(int poc){
    
    if ((poc % 30) == 0){
        SP= (1.0 - 0.2*(poc/30))*10.0;
       // acumError = 0;
    }
    
    double error = SP - PV;
    acumError += error;
    diffError = prevError - error;
    prevError = error;

    
    PIOut = KP*error + KI*acumError + KD*diffError;
    controlFile << SP << "\t" << PV << "\t" << PIOut + SP << endl;
    
}

bool TComComplexityController::isConstrained(int poc){
    
    if (poc > 4 and (PIOut) < 0.1)
        return true;
    return false;
} 