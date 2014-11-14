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
    
    outSobelGrad.open("SobelGrad_0.out",ofstream::out);
    outSobelMagn.open("SobelMagn_0.out",ofstream::out);

}

void TComClassifier::calcCTUSobel(TComDataCU* cu, TComPicYuv* origFrame){  
    Pel* lumaPointer = origFrame->getLumaAddr();
    Int stride = origFrame->getStride();
    Int cuWidth = cu->getWidth(0);
    Int cuHeight = cu->getHeight(0);
    Int cuX = cu->getCUPelX();
    Int cuY = cu->getCUPelY();
    lumaPointer += cuY*stride;
    double local_max_grad = -999;
    
    for(int i = 0; i < cuHeight and cuY+i < picHeight  ; i++){
        for(int j = 0; j < cuWidth and cuX+j < picWidth ; j++){
            if (j == 0 or j == cuWidth-1 or i == 0 or i == cuHeight-1 or  cuY+i == picHeight-1 or cuX+j == picWidth-1 ){
                 grad[cuY+i][cuX+j] = -1;
            }
            else{
                int s00 = *(&lumaPointer[j+cuX]-stride-1);
                int s01 = *(&lumaPointer[j+cuX]-stride);
                int s02 = *(&lumaPointer[j+cuX]-stride+1);
                int s10 = *(&lumaPointer[j+cuX]-1);
                int s11 = lumaPointer[j+cuX];
                int s12 = *(&lumaPointer[j+cuX]+1);
                int s20 = *(&lumaPointer[j+cuX]+stride-1);
                int s21 = *(&lumaPointer[j+cuX]+stride);
                int s22 = *(&lumaPointer[j+cuX]+stride+1);
                double gv = (-1.0*s00 - 2.0*s01 - 1.0*s02 + \
                            0*s10 +   0*s11 +   0*s12 + \
                          1.0*s20 + 2.0*s21 + 1.0*s22)/8.0;

                double gh = (-1.0*s00 -   0*s01 + 1.0*s02 + \
                          -2.0*s10 +  0*s11 + 2.0*s12 + \
                          -1.0*s20 +  0*s21 + 1.0*s22)/8.0;
                double gr = sqrt(gv*gv + gh*gh);
                
                max_grad = max(gr, max_grad);
                local_max_grad = max(gr, local_max_grad);
                grad[cuY+i][cuX+j] = gr; // > 255? 255 :  sqrt(gv*gv + gh*gh);
                magnitude[cuY+i][cuX+j] = atan2(gv,gh);


            }
        }
        lumaPointer += stride;
    }
        
    lumaPointer = origFrame->getLumaAddr();
    lumaPointer += cuY*stride;
    
    double th = local_max_grad/4.0;
    xCalcCTUDivisions(th);

}  


void TComClassifier::xcalcCTUDivisions(double th){
    
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
            double gr = sqrt(gv*gv + gh*gh) > 255? 255 :  sqrt(gv*gv + gh*gh);
            max_grad = max(gr, max_grad);

            grad[i-1][j-1] = gr;
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
        outSobelGrad.open(sstr.str().c_str(),ofstream::out);
    }
    
    if(not(outSobelMagn.is_open())){
        stringstream sstr;
        sstr << "SobelMagn_" << poc << ".out";
        outSobelMagn.open(sstr.str().c_str(),ofstream::out);
    }
    
    

    
    for(int i = 0; i < picHeight; i++){
        for(int j = 0; j < picWidth; j++){
            if (grad[i][j] == -1)
                outSobelGrad << MAX_SOBEL+1 << " ";
            else
                outSobelGrad << round(grad[i][j]/(max_grad/MAX_SOBEL))<< " ";
            outSobelMagn << magnitude[i][j] << " ";

        }
        outSobelGrad << endl;
        outSobelMagn << endl;
    }
    outSobelGrad.close();
    outSobelMagn.close();
       
}

