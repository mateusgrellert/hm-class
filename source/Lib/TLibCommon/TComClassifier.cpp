#include <fstream>
#include <sstream>
#include "TComClassifier.h"

using namespace std;

double** TComClassifier::grad;
double** TComClassifier::magnitude;
double** TComClassifier::avg_grad;
double** TComClassifier::avg_magnitude;

ofstream TComClassifier::outSobelGrad;
ofstream TComClassifier::outSobelMagn;
int TComClassifier::picWidth;
int TComClassifier::picHeight;

void TComClassifier::init(int picw, int pich){
    picWidth = picw;
    picHeight = pich;
   
    grad = new double*[picHeight];
    magnitude = new double*[picHeight];
    avg_grad = new double*[picHeight];
    avg_magnitude = new double*[picHeight];

    for(int i = 0; i < picHeight; i++){
        grad[i] = new double[picWidth];
        magnitude[i] = new double[picWidth];
        avg_grad[i] = new double[picWidth];
        avg_magnitude[i] = new double[picWidth];       
    }
    
    outSobelGrad.open("SobelGrad_0.out",ofstream::out);
    outSobelMagn.open("SobelMagn_0.out",ofstream::out);

}

void TComClassifier::calcCUSobel(TComDataCU*& cu, TComYuv* origYuv){
    TComPic* pcPic = cu->getPic();
    int picWidth = pcPic->getPicYuvOrg()->getWidth();
    int picHeight = pcPic->getPicYuvOrg()->getHeight();

    UInt cuWidth = origYuv->getWidth();
    UInt cuX = cu->getCUPelX();
    UInt cuY = cu->getCUPelY();
    UInt stride = pcPic->getPicYuvOrg()->getWidth();

    double luma[64][64];
    

            
    Pel* lumaPointer = origYuv->getLumaAddr();
    lumaPointer += cuX*cuWidth + cuY*stride;
    
    for(int i = 0; i < cuWidth and (cuY+i) < picHeight; i++){
        for(int j = 0; j < cuWidth and  (cuX+j) < picWidth; j++){
            luma[i][j] = lumaPointer[0];
            
            lumaPointer++;
        }
      //  lumaPointer += stride;
    }
    
    
    for(int i = 0; i < cuWidth and (cuY+i) < picHeight; i++){
        for(int j = 0; j < cuWidth and  (cuX+j) < picWidth; j++){
            int s00 = (i-1<0  or j-1<0)                 ? 0 : luma[i-1][j-1];
            int s01 = (i-1<0)                           ? 0 : luma[i-1][j];
            int s02 = (i-1<0  or j+1>=picWidth)         ? 0 : luma[i-1][j+1];
            int s10 = (j-1<0)                           ? 0 : luma[i][j-1];
            int s11 =                                         luma[i][j];
            int s12 = (j+1>=picWidth)                   ? 0 : luma[i][j+1];
            int s20 = (i+1>=picHeight or j-1<0)         ? 0 : luma[i+1][j-1];
            int s21 = (i+1>=picHeight)                  ? 0 : luma[i+1][j];
            int s22 = (i+1>=picHeight or j+1>=picWidth) ? 0 : luma[i+1][j+1];
            
            float gv = -1.0*s00 - 2.0*s01 - 1.0*s02 + \
                          0*s10 +   0*s11 +   0*s12 + \
                        1.0*s20 + 2.0*s21 + 1.0*s22;
            
            float gh = -1.0*s00 -   0*s01 + 1.0*s02 + \
                        -2.0*s10 +  0*s11 + 2.0*s12 + \
                        -1.0*s20 +  0*s21 + 1.0*s22;
            
           
          /*  int gv = -1*luma[i-1][j-1] - 2*luma[i-1][j] - 1*luma[i-1][j+1] + \
                      0*luma[i][j-1]         + 0*luma[i][j]         + 0*luma[i][j+1] + \
                      1*luma[i+1][j-1]  + 2*luma[i+1][j]  + 1*luma[i+1][j+1];
            
            int gh = -1*luma[i-1][j-1] + 0*luma[i-1][j] + 1*luma[i-1][j+1] + \
                     -2*luma[i][j-1]         + 0*luma[i][j]         + 2*luma[i][j+1] + \
                     -1*luma[i+1][j-1]  + 0*luma[i+1][j]  + 1*luma[i+1][j+1];            
            */
            //cout << gv << " " << gh << " " << sqrt(gv*gv + gh*gh) << endl;
            grad[cuY+i][cuX+j] = sqrt(gv*gv + gh*gh);
            magnitude[cuY+i][cuX+j] = atan2(gv,gh);
        }
    }
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
                  float avgg, avgm;
                  int bsize = 8;
    for(int i = 0; i < picHeight; i+=bsize){
        for(int j = 0; j < picWidth; j+=bsize){
            avgg = 0;
            avgm = 0;
            for(int k = 0; k < bsize; k++){
                for(int l = 0; l < bsize; l++){
                    avgg += grad[i+k][j+l];
                    avgm += magnitude[i+k][j+l];

                }
            }
            for(int k = 0; k < bsize; k++){
                for(int l = 0; l < bsize; l++){
                   avg_grad[i+k][j+l] = avgg;
                   avg_grad[i+k][j+l] = avgm;

                }
            }
        }
    }
                  
    for(int i = 0; i < picHeight; i++){
        for(int j = 0; j < picWidth; j++){
           
            outSobelGrad << avg_grad[i][j]/(bsize*bsize) << " ";
            outSobelMagn << avg_magnitude[i][j]/(bsize*bsize) << " ";

        }
        outSobelGrad << endl;
        outSobelMagn << endl;
    }
    outSobelGrad.close();
    outSobelMagn.close();
       
}

/*
     ofstream outSobel;
    if (cuX == 0 and cuY == 0){
        outSobel.open("Sobel.txt",ofstream::out);

        for(int i = 0; i < cuWidth; i++){
            for(int j = 0; j < cuWidth; j++){
                outSobel << grad[i][j] << "\t";
            }
            outSobel << endl;
        }

        outSobel << endl;

        for(int i = 0; i < cuWidth; i++){
            for(int j = 0; j < cuWidth; j++){
                outSobel << magnitude[i][j] << "\t";
            }
            outSobel << endl;
        }
        outSobel.close();
    }
 */