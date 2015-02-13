#include "TComComplexityController.h"

#include <iostream>
#include <fstream>

using namespace std;


double TComComplexityController::SP;
double TComComplexityController::SP_factor;
double TComComplexityController::PV;
double TComComplexityController::KP;
double TComComplexityController::KI;
double TComComplexityController::KD;
double TComComplexityController::PIOut;
double TComComplexityController::PIDOut;
double TComComplexityController::prevError;
double TComComplexityController::diffError;
double TComComplexityController::acumError;
double TComComplexityController::avgPV;
int TComComplexityController::budgetAlgorithm;
int TComComplexityController::currBudgetDepth;
int TComComplexityController::frameCounter;

ofstream TComComplexityController::controlFile;

TComComplexityController::TComComplexityController() {
    return;
}

void TComComplexityController::init(double sp_factor, int budgetAlg){
    SP_factor = sp_factor;
    currBudgetDepth = 3;
    budgetAlgorithm = budgetAlg;
    PV = 0;
    acumError = prevError = diffError = 0.0;
    KP =  KI = KD = 1.0;
   // KP = 0.5;
   // KI = 0.5;
    KI = 1.0;
    KD = 0.5;
    avgPV = 0.0;
    frameCounter = 0;
    
    controlFile.open("controlOut.csv",ofstream::out);
    controlFile << "KP\t" << KP << "\tKI\t" << KI << "\tKD\t" << KD << endl << "SP\tPV\tControl Output\tBudget Depth for Fi+1" << endl;

}

void TComComplexityController::calcPI(int poc){
    
 //   if ((poc % 30) == 0){
 //       SP= (1.0 - 0.2*(poc/30))*10.0;
       // acumError = 0;
 //   }
    
    double error = SP - PV;
    acumError += error;
    diffError = prevError - error;
    prevError = error;
    
    PIOut = KP*error + KI*acumError;
            
    avgPV += PV;
    frameCounter++;
    
    calcBudget();
    
    controlFile << SP << "\t" << PV << "\t" << PIOut + PV << "\t" << currBudgetDepth << endl;
    PV = 0;
}

void TComComplexityController::calcPID(int poc){
    
 //   if ((poc % 30) == 0){
 //       SP= (1.0 - 0.2*(poc/30))*10.0;
       // acumError = 0;
  //  }
    
    double error = SP - PV;
    acumError += error;
    diffError = error - prevError;
    prevError = error;

    
    PIOut = KP*error + KI*acumError + KD*diffError;
        
    avgPV += PV;
    frameCounter++;
    
    calcBudget();

    controlFile << SP << "\t" << PV << "\t" << PIOut + PV << "\t" << currBudgetDepth << endl;
    PV = 0;

}

void TComComplexityController::calcBudget(){
        switch(budgetAlgorithm){
            case 0:  budgetAlgorithm0();             
                     break;
            case 1:  budgetAlgorithm1();            
                     break;
           // case 2:  budgetAlgorithm2();
            default: budgetAlgorithm0();             
                     break;
        }

}


void TComComplexityController::budgetAlgorithm1(){  
        
    double PVDiff = PV - (PV+PIOut);

    if (PVDiff/SP < -0.1)
        currBudgetDepth++;
    else if ( PVDiff/SP > 0.1)
        currBudgetDepth--;
        
    currBudgetDepth = (currBudgetDepth >= 0? currBudgetDepth : 0);
    currBudgetDepth = (currBudgetDepth <= 3? currBudgetDepth : 3);

}

void TComComplexityController::budgetAlgorithm0(){
            
    double PVDiff = 1-((PV+PIOut)/SP);

    if (PVDiff <= -0.4)
        currBudgetDepth += 3;  
    if (PVDiff > -0.4 and PVDiff <= -0.2)
        currBudgetDepth += 2;    
    if (PVDiff > -0.2 and PVDiff <= 0.0)
        currBudgetDepth += 1;    
    if (PVDiff > 0.0 and PVDiff <= 0.2)
        currBudgetDepth -= 1;
    if (PVDiff > 0.2 and PVDiff <= 0.4)
        currBudgetDepth -= 2;
    if (PVDiff > 0.4)
        currBudgetDepth -= 3;
                
    currBudgetDepth = (currBudgetDepth >= 0? currBudgetDepth : 0);
    currBudgetDepth = (currBudgetDepth <= 3? currBudgetDepth : 3);
}

bool TComComplexityController::isConstrained(int poc){
    
    if (poc > 4 + REFRESH_PERIOD)
        return true;
    return false;
} 

bool TComComplexityController::terminateCTU(unsigned int depth){
    
    if (depth >= currBudgetDepth)
        return true;
    return false;
}