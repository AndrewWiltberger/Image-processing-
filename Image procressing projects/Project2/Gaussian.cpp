//Guassian smoothing
//Andrew Wiltberger
#include "image.h"
#include <iostream>
#include <math.h> 
int readImageHeader(char[], int&, int&, int&, bool&);
int readImage(char[], ImageType&);
int writeImage(char[], ImageType&);
int gauss7(int p, int q, ImageType image, int Mask[][7]);
int gauss15(int p, int q, ImageType image, int Mask[][15]);



int main(int agrc, char *argv[])
{
	float temp;
	int i, j; 
	int M, N, Q;
	int val;
	bool type;
	
	int Mask7[7][7] =
	{
	{1,1,2,2,2,1,1},
	{1,2,2,4,2,2,1},
	{2,2,4,8,4,2,2},
	{2,4,8,16,8,4,2},
	{2,2,4,8,4,2,2},
	{1,2,2,4,2,2,1},
	{1,1,2,2,2,1,1},
	};

	int Mask15[15][15] = 
	{
    {2 ,2, 3, 4, 5, 5, 6, 6, 6, 5, 5, 4, 3, 2, 2},
    {2, 3, 4, 5, 7, 7, 8, 8, 8, 7, 7, 5, 4, 3, 2},
    {3, 4, 6, 7, 9,10,10,11,10,10, 9, 7, 6, 4, 3},
    {4, 5, 7, 9,10,12,13,13,13,12,10, 9, 7, 5, 4},
    {5, 7, 9,11,13,14,15,16,15,14,13,11, 9, 7, 5},
    {5, 7,10,12,14,16,17,18,17,16,14,12,10, 7, 5},
    {6, 8,10,13,15,17,19,19,19,17,15,13,10, 8, 6},
    {6, 8,11,13,16,18,19,20,19,18,16,13,11, 8, 6},
    {6, 8,10,13,15,17,19,19,19,17,15,13,10, 8, 6},
    {5, 7,10,12,14,16,17,18,17,16,14,12,10, 7, 5},
    {5, 7, 9,11,13,14,15,16,15,14,13,11, 9, 7, 5},
    {4, 5, 7, 9,10,12,13,13,13,12,10, 9, 7, 5, 4},
    {3, 4, 6, 7, 9,10,10,11,10,10, 9, 7, 6, 4, 3},
    {2, 3, 4, 5, 7, 7, 8, 8, 8, 7, 7, 5, 4, 3, 2},
    {2 ,2, 3, 4, 5, 5, 6, 6, 6, 5, 5, 4, 3, 2, 2}
	};

	
	//read image header
	readImageHeader(argv[1], N, M, Q, type);
	
	//allocate memory for the input image array
	ImageType image(N, M, Q);

	//Using the copy constructor to allocate memory for both the output image
	ImageType averagedImage7(image);
	ImageType averagedImage15(image);

	//read image
	readImage(argv[1], image);
	
	//int jim = gauss7(16, 16, image, Mask7);
	//std::cout<<"Jim:" <<jim;
	//set pixel values for output images 
	for(i=0; i<N; i++)
		for(j=0; j<M; j++)
		{
			
			averagedImage7.setPixelVal(i, j, gauss7(i, j, image, Mask7));
			averagedImage15.setPixelVal(i, j, gauss15(i, j, image, Mask15));
		}

	//to convert the matrix to .pgm file
	writeImage(argv[2], averagedImage7);
	writeImage(argv[3], averagedImage15);

	return(1);
}


int gauss7(int p, int q, ImageType image, int Mask[][7])
{
	//x and y will be index for the mask 
	int x = 0;
	int y = 0;
	double sum = 0;
	int total = 0;
	//compute the mask 
	for(int i = p-3; i <= p+3; i++)
	{
		for(int j = q-3; j <= q+3; j++)
		{
			//adding up weights in the mask
			total += Mask[x][y];
			//applying mask to pixels
			if(i >= 0 && i < 256 && j >=0 && j < 256)
			{	

				int val = 0;
				image.getPixelVal(i, j, val);
				double temp = val*(Mask[x][y]);
				sum += temp;
			}
		y++;
		}
	y = 0;
	x++;
	}

	sum = sum/total;
	int done = int(sum);
	return done;
}

int gauss15(int p, int q, ImageType image, int Mask[][15])
{
	//x and y will be index for the mask 
	int x = 0;
	int y = 0;
	double sum = 0;
	int total = 0;
	//compute the mask 
	for(int i = p-7; i <= p+7; i++)
	{
		for(int j = q-7; j <= q+7; j++)
		{
			//adding up weights in the mask
			total += Mask[x][y];
			//applying mask to pixels
			if(i >= 0 && i < 256 && j >=0 && j < 256)
			{	

				int val = 0;
				image.getPixelVal(i, j, val);
				double temp = val*(Mask[x][y]);
				sum += temp;
			}
		y++;
		}
	y = 0;
	x++;
	}
	
	sum = sum/total;
	int done = int(sum);
	return done;
}