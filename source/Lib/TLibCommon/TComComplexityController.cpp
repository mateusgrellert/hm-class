#include "TComComplexityController.h"

#include <iostream>
    
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

TComComplexityController::TComComplexityController() {
    return;
}

void TComComplexityController::init(double sp){
    SP = sp;
    SP= 5.0;
    acumError = prevError = diffError = 0.0;
    KP =  KI = KD = 1.0;
}

void TComComplexityController::calcPI(){
    double error = SP - PV;
    acumError += error;
    diffError = prevError - error;
    
    PIOut = KP*error + KI*error;
    std::cout << "\tSP: " << SP << "\tPV: " << PV << "\tPI output: " << PIOut << std::endl;
    
}

bool TComComplexityController::isConstrained(int poc){
    
    if (poc > 4 and abs(PIOut) > 0.1)
        return true;
    return false;
} 