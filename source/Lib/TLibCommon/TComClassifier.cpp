#include <fstream>
#include <sstream>
#include "TComClassifier.h"
#include <cmath>
using namespace std;

double** TComClassifier::grad;
double** TComClassifier::magnitude;
double TComClassifier::max_grad;

ofstream TComClassifier::outSobelGrad;
ofstream TComClassifier::outSobelMagn;
int TComClassifier::picWidth;
int TComClassifier::picHeight;

int** TComClassifier::predictedMap;
int** TComClassifier::actualMap;

void TComClassifier::init(int picw, int pich){    
    picWidth = picw;
    picHeight = pich;
    
    max_grad = -999;

    grad = new double*[picHeight];
    magnitude = new double*[picHeight];
    for(int i = 0; i < picHeight; i++){
        grad[i] = new double[picWidth];
        magnitude[i] = new double[picWidth];   
    }
    
    predictedMap = new int*[picHeight/64+1];
    actualMap = new int*[picHeight/64+1];

    for(int i = 0; i < picHeight/64+1; i++){
        predictedMap[i] = new int[picWidth/64+1];
        actualMap[i] = new int[picWidth/64+1];
    }

    outSobelGrad.open("SobelGrad_0.out",ofstream::out);
    outSobelMagn.open("SobelMagn_0.out",ofstream::out);

}

void TComClassifier::estimateCTUEffort(TComDataCU* cu, TComPicYuv* origFrame){  
    Pel* lumaPointer = origFrame->getLumaAddr();
    Int stride = origFrame->getStride();
    Int cuWidth = cu->getWidth(0);
    Int cuHeight = cu->getHeight(0);
    Int cuX = cu->getCUPelX();
    Int cuY = cu->getCUPelY();
    lumaPointer += cuY*stride;
    

    
    xcalcCTUDivisions(cuX, cuY, cuWidth, cuHeight);

}  


void TComClassifier::xcalcCTUDivisions(int cuX, int cuY, int cuWidth, int cuHeight){
    //double noise = 0.5;
   

    predictedMap[cuY/64][cuX/64] = actualMap[cuY/64][cuX/64];
        
}

void TComClassifier::setEncCTUDepth(TComDataCU* pu, UInt absIdx){
    int depth = pu->getDepth(absIdx);
    int cuX = pu->getCUPelX();
    int cuY = pu->getCUPelY();
    actualMap[cuY/64][cuX/64] = depth;
}


void TComClassifier::printHitMissCTUPrediction(){
    int hit_count, miss_count, total;
    hit_count = miss_count = total = 0;
    cout << endl;
    for (int i = 0; i < picHeight/64; i++){
        for(int j = 0; j < picWidth/64; j++){
            if (predictedMap[i][j] >= actualMap[i][j])
                hit_count++;
            else
                miss_count++;
            total++;
            cout << predictedMap[i][j] << ' ';
        }
        cout << "\t\t"; 
        for(int j = 0; j < picWidth/64; j++){
            cout << actualMap[i][j] << ' ';
        }
        
        cout << endl;
    }
    cout << "\tHits: " << double(hit_count)/total << "\t";
    cout << "Misses: " << double(miss_count)/total << endl;
}