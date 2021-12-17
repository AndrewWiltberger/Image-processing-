//usage: g++ correlation.cpp image.cpp
// ./a.out pattern.pgm inputimage.pgm outputimage.pgm 
#include "image.h"
#include <iostream>
#include <vector>
int readImageHeader(char[], int&, int&, int&, bool&);
int readImage(char[], ImageType&);
int writeImage(char[], ImageType&);

int correlation(int i, int j, int N1, int M1, ImageType mask, ImageType tempBuffer);


int main(int agrc, char *argv[])
{
	int i, j, M, N, Q, N1, M1, Q1; 
    bool type;
 


    //read mask headder 
    readImageHeader(argv[1], N1, M1, Q1, type);
   	//allocate memeory for the mask 
    ImageType mask(N1, M1, Q1);
    //read input mask image 
    readImage(argv[1], mask);


    //read image header 
    readImageHeader(argv[2], N, M, Q, type);
    //allocate memory for the input image 
    ImageType inputImage(N, M, Q);
    //read input image 
    readImage(argv[2], inputImage);

    //allocating memeory for tempBuffer which will deal with edgecases
    ImageType tempBuffer(N+N1, M+M1, Q);


    //copy image into the buffer which will be used to compute the convolution
    for(i = 0; i < N; i++)
    {
    	for(j = 0; j < M; j++)
    	{	
    		int temp = 0;
    		inputImage.getPixelVal(i,j, temp);
    		tempBuffer.setPixelVal(i, j, temp);
    	}
    }


    int normalize = 0;
    int val = 0;
    double correlationArray[N][M];
 	
 	//start correlation 
    for(i = 0; i < N ; i++)
    {
        for(j = 0; j < M; j++)
        {
        	//finding value of correlation on every pixel
            int newVal = correlation(i, j, N1, M1, mask, tempBuffer);
           
            //normalize will keep track of the largest correlation value to normalize with
	        if(newVal > normalize)
            {
            	normalize = newVal;
            }

          	correlationArray[i][j] = newVal;
        }  
    }  


    //normalize every pixel in the image
    for(i = 0; i < N ; i++)
    {
        for(j = 0; j < M; j++)
        {
            correlationArray[i][j] /= normalize;
            correlationArray[i][j] *= 255;
        }  
    }  

   	//setting orignial image pixels correlated values 
    for(i = 0; i < N; i++)
    {
        for(j = 0; j < M; j++)
        {
            inputImage.setPixelVal(i, j, int(correlationArray[i][j]));
        }
    }
      // write image
        writeImage(argv[3], inputImage);
    

	return(1);
}


int correlation(int i, int j, int N1, int M1, ImageType mask, ImageType tempBuffer)
{
	int maskVal = 0;
	int newVal = 0;
	int val = 0;
 	for(int maskRow = 0; maskRow < N1; maskRow++)
    {
    	for(int maskCol = 0; maskCol < M1; maskCol++)
        {
            tempBuffer.getPixelVal(i+maskRow, j+maskCol, val);
            mask.getPixelVal(maskRow, maskCol, maskVal); 
            newVal += val*maskVal;
        }
    }
    return newVal;
}
