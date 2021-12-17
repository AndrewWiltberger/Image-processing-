//usage: g++ average.cpp.cpp image.cpp
//./a.out input_image.pgm output_image.pgm7x7 output_image.pgm15x15
#include "image.h"
#include <iostream>
int readImageHeader(char[], int&, int&, int&, bool&);
int readImage(char[], ImageType&);
int writeImage(char[], ImageType&);
int averageFilter7(int p, int q, ImageType image);
int averageFilter15(int p, int q, ImageType image);

int main(int agrc, char *argv[])
{
	float temp;
	int i, j; 
	int M, N, Q;
	int val;
	bool type;
	
	//read image header
	readImageHeader(argv[1], N, M, Q, type);
	
	//allocate memory for the input image array
	ImageType image(N, M, Q);

	//Using the copy constructor to allocate memory for both the output image
	ImageType averagedImage7(image);
	ImageType averagedImage15(image);

	//read image
	readImage(argv[1], image);
	

	//set pixel values for output images 
	for(i=0; i<N; i++)
		for(j=0; j<M; j++)
		{
			int element = 0;
			averagedImage7.setPixelVal(i, j, averageFilter7(i, j, image));
			averagedImage15.setPixelVal(i, j, averageFilter15(i, j, image));
		}

	//to convert the matrix to .pgm file write both images
	writeImage(argv[2], averagedImage7);
	writeImage(argv[3], averagedImage15);
	return(1);
}


int averageFilter7(int p, int q, ImageType image)
{
	int temp = 0;
	//compute the mask 
	for(int i = p-3; i <= p+3; i++)
	{
		for(int j = q-3; j <= q+3; j++)
		{
			
			int val = 0;
			//will only consider pixels in the original image
			//padding w/ 0 otherwise
			if(i >= 0 && i < 256 && j >=0 && j < 256)
			{	
	
				image.getPixelVal(i, j, val);
			}
			temp += val;
		}
	}
	//divide by 49 to get an average
	temp = temp/49;
	return temp;
}

int averageFilter15(int p, int q, ImageType image)
{
	int temp = 0;
	for(int i = p-7; i <= p+7; i++)
	{
		for(int j = q-7; j <= q+7; j++)
		{
			
			int val = 0;
			
			if(i >= 0 && i < 256 && j >=0 && j < 256)
			{	
	

				image.getPixelVal(i, j, val);
			}
			temp += val;
		}
	}
	//divide by 196 to get an average
	temp = temp/225;
	return temp;
}