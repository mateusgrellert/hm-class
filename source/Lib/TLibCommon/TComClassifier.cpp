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

int** TComClassifier::predictedDepthMap;
int** TComClassifier::actualDepthMap;

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
    
    predictedDepthMap = new int*[picHeight/64+1];
    actualDepthMap = new int*[picHeight/64+1];

    for(int i = 0; i < picHeight/64+1; i++){
        predictedDepthMap[i] = new int[picWidth/64+1];
        actualDepthMap[i] = new int[picWidth/64+1];
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
    
    xcalcCTUDivisions(local_max_grad, cuX, cuY, cuWidth, cuHeight);

}  


void TComClassifier::xcalcCTUDivisions(double local_max_grad, int cuX, int cuY, int cuWidth, int cuHeight){
    //double noise = 0.5;
    double th = local_max_grad/4.0;
    double acum = 0.0;
    for(int i = 0; i < cuHeight and cuY+i < picHeight  ;i ++){
        for(int j = 0; j < cuWidth and cuX+j < picWidth ;j ++){
            if (grad[cuY+i][cuX+j] == -1)
                continue;
            //acum += round(grad[cuY+i][cuX+j]/(local_max_grad/MAX_SOBEL))*round(grad[cuY+i][cuX+j]/(local_max_grad/MAX_SOBEL));
            acum += (grad[cuY+i][cuX+j]);
        }
    }
    int minW = min(cuWidth, picWidth-cuX+cuWidth );
    int minH = min(cuHeight, picHeight-cuY+cuHeight );

    double avg = acum / (minW*minH);
    int maxDepth;
    if (avg > th){
        maxDepth = 4;
    }
    else if (avg > 0.25*th){
        maxDepth = 3;
    }
    else if (avg > 0.1*th){
        maxDepth = 2;
    }
    else
        maxDepth = 1;

    predictedDepthMap[cuY/64][cuX/64] = maxDepth-1;
        
}

void TComClassifier::setEncCTUDepth(TComDataCU* pu, UInt absIdx){
    int depth = pu->getDepth(absIdx);
    int cuX = pu->getCUPelX();
    int cuY = pu->getCUPelY();
    actualDepthMap[cuY/64][cuX/64] = depth;
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

void TComClassifier::printHitMissCTUPrediction(){
    int hit_count, miss_count, total;
    hit_count = miss_count = total = 0;
    cout << endl;
    for (int i = 0; i < picHeight/64; i++){
        for(int j = 0; j < picWidth/64; j++){
            if (predictedDepthMap[i][j] == actualDepthMap[i][j])
                hit_count++;
            else
                miss_count++;
            total++;
            cout << predictedDepthMap[i][j] << ' ';
        }
        cout << "\t\t"; 
        for(int j = 0; j < picWidth/64; j++){
            cout << actualDepthMap[i][j] << ' ';
        }
        
        cout << endl;
    }
    cout << "\tHits: " << double(hit_count)/total << "\t";
    cout << "Misses: " << double(miss_count)/total << endl;
}