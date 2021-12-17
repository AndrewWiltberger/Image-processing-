#include <iostream>
#include <cmath>
#include <cfloat>
#include <algorithm>
#include "fft.h"
#include "image.h"

void rowfft(int N, float *real_Fuv, float *imag_Fuv, int isign)
{
    
	// This function is used to perfrom 1D FFT on each row of the image
    int nn = 2*N + 1;
    for (int i=0; i<N; i++)
    {
        float *tempdata = new float[nn];
        tempdata[0] = 0;
        for (int j=0; j<N; j++)
        {
            // Copying the real values of the image in odd locations of temp data
            tempdata[(2*j)+1] = real_Fuv[i*N+j];
            
            // copying the imaginary values of the image in even locations of temp data
            tempdata[(2*j)+2] = imag_Fuv[i*N+j];
        }
        
        // Calling the fft function on each row
        fft(tempdata, N, isign);
        for (int j=1; j<nn; j++)
        {
            // Checking the even and odd locations of the temp data
            if (j%2 != 0)
            {
                // if odd location then copy to the real valued matrix
                real_Fuv[i*N+int(j/2)] = tempdata[j];
            }
            else
            {
                // if even location copy to the imaginary valued matrix
                imag_Fuv[i*N+int((j-2)/2)] = tempdata[j];
            }
            
        }
        delete[] tempdata;
    }
}

void colfft(int N, float *real_Fuv, float *imag_Fuv, int isign)
{
    // The same steps are followed as the rowfft, but perroming these functions on
    // each column and finally dividing the result with N
    int nn = 2*N + 1;
    for (int i=0; i<N; i++)
    {
        float *tempdata = new float[nn];
        tempdata[0] = 0;
        for (int j=0; j<N; j++)
        {
            tempdata[(2*j)+1] = real_Fuv[j*N+i];
            tempdata[(2*j)+2] = imag_Fuv[j*N+i];
        }
        fft(tempdata, N, isign);
        for (int j=1; j<nn; j++)
        {
            if (j%2 != 0)
            {
                real_Fuv[int(j/2)*N+i] = tempdata[j]/N;
            }
            else
            {
                imag_Fuv[int((j-2)/2)*N+i] = tempdata[j]/N;
            }
            
        }
        delete[] tempdata;
    }
}

void fft2D(int N, int M, float *real_Fuv, float *imag_Fuv, int isign)
{
    // This function is used to perfrom FFT on the rows using the fft function
    rowfft(N, real_Fuv, imag_Fuv, isign);
    
    // This function is used to perform FFT on the columns using the fft function
    colfft(N, real_Fuv, imag_Fuv, isign);
}

int check_dimensions(int N, int M)
{
    if (N == M)
    // Checking if N == M
    {
        if(ceil(std::log2(N)) == floor(std::log2(N)))
        // To check if N is a power of 2, if yes return N
        {return N;}

        // else return the number which is closer to the power of 2 for zero padding
        else {return(std::pow(2, ceil(std::log2(N))));}
    }
    else
    {
        // if dimensions are not equal
        int max_dimension = std::max(N, M);
        if(ceil(std::log2(max_dimension)) == floor(std::log2(max_dimension)))
        // Check if the max dimension is a power of 2 and return max dimension
        {return max_dimension;}
        // else return the number which is closer to the power of 2 for zero padding
        else {return (std::pow(2, ceil(std::log2(max_dimension))));}
    }
    
}

void setMagnitude(int N, float *real_Fuv, float *imag_Fuv)
{
	for (int i=0; i<N; i++)
	{
		for (int j=0; j<N; j++)
		{
			// setting the phase equal to zero by assigning the 
			// real valued matrix with the magnitude value and setting
			// the imaginary matrix to zero
			 real_Fuv[i*N+j] = std::pow(std::pow(real_Fuv[i*N+j],2) + std::pow(imag_Fuv[i*N+j],2),0.5);
			 imag_Fuv[i*N+j] = 0;
		}
	}
}

void setPhase(int N, float *real_Fuv, float *imag_Fuv)
{
	for (int i=0; i<N; i++)
	{
		for (int j=0; j<N; j++)
		{
			// seting the magnitude equal to 1
			// This is done by setting the real values cos(theta)
			// and iamginary value as sin(theta), where theta = taninv(imag/real)
			 real_Fuv[i*N+j] = std::sin(std::atan2(imag_Fuv[i*N+j],real_Fuv[i*N+j]));
			 imag_Fuv[i*N+j] = std::cos(std::atan2(imag_Fuv[i*N+j],real_Fuv[i*N+j]));
		}
	}
}

int * magnitude(int N, float *real, float *imag, ImageType &image)
{
    static int minMax [2];
    int val;
    float minPixel = FLT_MAX;
    float maxPixel = -FLT_MAX;
    for (int i=0; i<N; i++)
    {
        for (int j=0; j<N; j++)
        {
            // calculating the magnitude by taking
            // the sum of square of the real and the imaginary values
            val = int(std::pow(std::pow(int(real[i*N+j]), 2) + std::pow(int(imag[i*N+j]), 2), 0.5));
            image.setPixelVal(i, j, val);
            // keeping track of the min and max pixels for the purpose of mapping
            if (val <= minPixel){minPixel = val;}
            if (val >= maxPixel){maxPixel = val;}
        }
    }
    minMax[0] = minPixel;
    minMax[1] = maxPixel;
    // appending the min and max to array and returning the address of the array
    return minMax;
}

void mapper(ImageType &image, int *minMaxArray, int N)
{
    // This function is used to map the pixel values ranging from [pixel_min, pixel_max]
    // to [0, 255] using the equation y = mx + c
    int val;
    for (int i=0; i<N; i++)
    {
        for (int j=0; j<N; j++)
        {
            image.getPixelVal(i, j, val);
            val = int((255.0/(minMaxArray[1] - minMaxArray[0]))*(val - minMaxArray[0]));
            image.setPixelVal(i, j, val);
        }
    }
}

int main(int argc, char *argv[])
{
    int N, M, Q;
    bool type;
    int row_count, col_count;
    int row_strt_idx, row_stop_idx;
    int col_strt_idx, col_stop_idx;
    int val;

    // reading image header
    readImageHeader(argv[1], N, M, Q, type);
    
    // using the check dimensions to get the dimensions which is the power of 2
    const int pwr2_dim = check_dimensions(N, M);
    
    // Allocating memory to the input image
    ImageType input_image(N, M, Q);
    
    // read image 
    readImage(argv[1], input_image);

    // allocating memory for the imaginary and real matrices
    float *real_img_mag0 = new float[pwr2_dim * pwr2_dim];
    float *imag_img_mag0 = new float[pwr2_dim * pwr2_dim];

    float *real_img_phase0 = new float[pwr2_dim * pwr2_dim];
    float *imag_img_phase0 = new float[pwr2_dim * pwr2_dim];

    // calculating the indices where the actual image should be copied
    // and rest of the indices are zero padded
    row_strt_idx = int((pwr2_dim - N)/2);
    row_stop_idx = row_strt_idx + N;

    col_strt_idx = int((pwr2_dim - M)/2);
    col_stop_idx = col_strt_idx + M;

    row_count = -1;
    int i=0, j=0;
    for (i=0; i<pwr2_dim; i++)
    {
        col_count = 0;
        if(i >= row_strt_idx && i < row_stop_idx)
        {
        	row_count++;
        }
        for (j=0; j<pwr2_dim; j++)
        {
            // Initiliazing the imaginary matrix to zero
            imag_img_mag0[i*pwr2_dim+j] = 0;
            imag_img_phase0[i*pwr2_dim+j] = 0;
            
            // if the index i and j is within the range then the input image is copied
            if(i >= row_strt_idx && i < row_stop_idx && j>= col_strt_idx && j < col_stop_idx)
            {
                input_image.getPixelVal(row_count, col_count, val);
                real_img_mag0[i*pwr2_dim+j] = val;
                real_img_phase0[i*pwr2_dim+j] = val;
                col_count ++;
            }
            else
            {
                // if the value i and j is not in the desired range then pad zeros
                real_img_mag0[i*pwr2_dim+j] = 0;
                real_img_phase0[i*pwr2_dim+j] = 0;
            }
        }
    }
    
    // calling the 2D FFT function
    fft2D(pwr2_dim, pwr2_dim, real_img_mag0, imag_img_mag0, -1);
    
    // setting the phase to 0 by assigning the magnitude value to the
    // real matrix and zero to the imaginary matrix
    setMagnitude(pwr2_dim, real_img_mag0, imag_img_mag0);
    
    // performing inverse 2D FFT
    fft2D(pwr2_dim, pwr2_dim, real_img_mag0, imag_img_mag0, 1);
    
	// Allocating memory for the output_image with the dimensions
	// which is a power of 2
    ImageType output_image_mag0(pwr2_dim, pwr2_dim, 255);
    
    // Calculating the magnitude of the result of inverse fft
    // for the purpose of visualization
    int *magMinMax = magnitude(pwr2_dim, real_img_mag0, imag_img_mag0, output_image_mag0);
    
    // converting the matrix to .pgm file
    writeImage(argv[2], output_image_mag0);

    // Calculating the 2d fft for the input image
    fft2D(pwr2_dim, pwr2_dim, real_img_phase0, imag_img_phase0, -1);
    
	// setting the magnitude to 1
    setPhase(pwr2_dim, real_img_phase0, imag_img_phase0);

    // calculating the inverse fft after setting the magnitude to 1
    fft2D(pwr2_dim, pwr2_dim, real_img_phase0, imag_img_phase0, 1);
    
    // allocating memory for the output_image, when the magnitude
    // is 1 with the dimensions which is a power of 2
    ImageType output_image_phase0(pwr2_dim, pwr2_dim, 255);
    
    // Calculating the magnitude of the inverse for the purpose of visualization
    int *phaseMinMax = magnitude(pwr2_dim, real_img_phase0, imag_img_phase0, output_image_phase0);
    
	// using the mapper function to bring the range of pixel values
	// between 0 and 255
    mapper(output_image_phase0, phaseMinMax, pwr2_dim);
    
    // Converting the matrix to .pgm file
    writeImage(argv[3], output_image_phase0);

    delete[] real_img_mag0;
    delete[] imag_img_mag0;
    delete[] real_img_phase0;
    delete[] imag_img_phase0;
}