#include <fstream>
#include <sstream>
#include "TComClassifier.h"
#include <cmath>
using namespace std;

double** TComClassifier::grad;
double** TComClassifier::magnitude;

ofstream TComClassifier::outSobelGrad;
ofstream TComClassifier::outSobelMagn;
int TComClassifier::picWidth;
int TComClassifier::picHeight;

void TComClassifier::init(int picw, int pich){
    picWidth = picw;
    picHeight = pich;
   
    grad = new double*[picHeight];
    magnitude = new double*[picHeight];

    for(int i = 0; i < picHeight; i++){
        grad[i] = new double[picWidth];
        magnitude[i] = new double[picWidth];   
    }
    
    outSobelGrad.open("SobelGrad_0.out",ofstream::out);
    outSobelMagn.open("SobelMagn_0.out",ofstream::out);

}

void TComClassifier::calcFrameSobel(TComPicYuv* recFrame, int poc){    

            
    Pel* lumaPointer = recFrame->getLumaAddr();
    //lumaPointer += (picWidth+1);

    Int stride = recFrame->getStride();
    lumaPointer += stride+1;

   for(int i = 1; i < picHeight-1 ; i++){
        for(int j = 1; j < picWidth-1 ; j++){
             
    
            int s00 = *(&lumaPointer[j]-stride-1);
            int s01 = *(&lumaPointer[j]-stride);
            int s02 = *(&lumaPointer[j]-stride+1);
            int s10 = *(&lumaPointer[j]-1);
            int s11 = lumaPointer[j];
            int s12 = *(&lumaPointer[j]+1);
            int s20 = *(&lumaPointer[j]+stride-1);
            int s21 = *(&lumaPointer[j]+stride);
            int s22 = *(&lumaPointer[j]+stride+1);
            
            double gv = (-1.0*s00 - 2.0*s01 - 1.0*s02 + \
                          0*s10 +   0*s11 +   0*s12 + \
                        1.0*s20 + 2.0*s21 + 1.0*s22)/8.0;
            
            double gh = (-1.0*s00 -   0*s01 + 1.0*s02 + \
                        -2.0*s10 +  0*s11 + 2.0*s12 + \
                        -1.0*s20 +  0*s21 + 1.0*s22)/8.0;

            //cout << gv << " " << gh << " " << sqrt(gv*gv + gh*gh) << endl;
            grad[i-1][j-1] = sqrt(gv*gv + gh*gh) > 255? 255 :  sqrt(gv*gv + gh*gh);
            magnitude[i-1][j-1] = atan2(gv,gh);
            //magnitude[i][j] = *(&lumaPointer[j]-stride-1);
  
            //lumaPointer++;
        }
        lumaPointer += stride;
        //lumaPointer += 2;
    }
    printSobelFrames(poc);
}

void TComClassifier::printSobelFrames(int poc){
    if(not(outSobelGrad.is_open())){
        stringstream sstr;
        sstr << "SobelGrad_" << poc << ".out";
        outSobelGrad.open(sstr.str().c_str(),ofstream::app);
    }
    
    if(not(outSobelMagn.is_open())){
        stringstream sstr;
        sstr << "SobelMagn_" << poc << ".out";
        outSobelMagn.open(sstr.str().c_str(),ofstream::app);
    }
    
    
    double max_grad = -999;
    for(int i = 0; i < picHeight-2; i++){
        for(int j = 0; j < picWidth-2; j++){
            max_grad = max(grad[i][j], max_grad);
        }
    }
    
    for(int i = 0; i < picHeight-2; i++){
        for(int j = 0; j < picWidth-2; j++){
           
            outSobelGrad << round(grad[i][j]/(max_grad/3.0))<< " ";
            outSobelMagn << magnitude[i][j] << " ";

        }
        outSobelGrad << endl;
        outSobelMagn << endl;
    }
    outSobelGrad.close();
    outSobelMagn.close();
       
}

