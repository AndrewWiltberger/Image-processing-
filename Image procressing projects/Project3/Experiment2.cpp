#include <iostream>
#include "image.h"
#include <cmath>
#include "fft.h"

void rowfft(int N, float (*real_Fuv)[512], float (*imag_Fuv)[512], int isign)
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
            tempdata[(2*j)+1] = real_Fuv[i][j];
            
            // copying the imaginary values of the image in even locations of temp data
            tempdata[(2*j)+2] = imag_Fuv[i][j];
        }
        
        // Calling the fft function on each row
        fft(tempdata, N, isign);
        for (int j=1; j<nn; j++)
        {
            // Checking the even and odd locations of the temp data
            if (j%2 != 0)
            {
                // if odd location then copy to the real valued matrix
                real_Fuv[i][int(j/2)] = tempdata[j];
            }
            else
            {
                // if even location copy to the imaginary valued matrix
                imag_Fuv[i][int((j-2)/2)] = tempdata[j];
            }
            
        }
        delete[] tempdata;
    }
}

void colfft(int N, float (*real_Fuv)[512], float (*imag_Fuv)[512], int isign)
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
            tempdata[(2*j)+1] = real_Fuv[j][i];
            tempdata[(2*j)+2] = imag_Fuv[j][i];
        }
        fft(tempdata, N, isign);
        for (int j=1; j<nn; j++)
        {
            if (j%2 != 0)
            {
                real_Fuv[int(j/2)][i] = tempdata[j]/N;
            }
            else
            {
                imag_Fuv[int((j-2)/2)][i] = tempdata[j]/N;
            }
            
        }
        delete[] tempdata;
    }
}

void fft2D(int N, int M, float (*real_Fuv)[512], float (*imag_Fuv)[512], int isign)
{
    // This function is used to perfrom FFT on the rows using the fft function
    rowfft(N, real_Fuv, imag_Fuv, isign);
    
    // This function is used to perform FFT on the columns using the fft function
    colfft(N, real_Fuv, imag_Fuv, isign);
}

void magnitude(int N, int C, float (*real)[512], float (*imag)[512], float (*mag)[512], ImageType &image)
{
    for (int i=0; i<N; i++)
    {
        for (int j=0; j<N; j++)
        {
            mag[i][j] = int(std::pow(std::pow(int(real[i][j]), 2) + std::pow(int(imag[i][j]), 2), 0.5));
            mag[i][j] = C * std::log2(1 + mag[i][j]);
            image.setPixelVal(i, j, int(mag[i][j]));
        }
    }
}

int main(int argc, char *argv[])
{
    const int size = 512;
    float square_real [size][size];
    float square_imag [size][size];
    float square_mag [size][size];
    float square_real_shifted [size][size];
    float square_imag_shifted [size][size];
    float square_mag_shifted [size][size];
    int square_size;
    int C;
    
    // ImageType object to create the output images
    ImageType output_image(size, size, 255);
    ImageType output_image_shifted(size, size, 255);
    
    // Prompting the user to enter the white square size
    std::cout<<"Enter the size of the white square: ";
    std::cin >> square_size;
    
    // Prompting the user to enter the constant value for contrast stretching
    std::cout<<"Enter the constant for contrast stretching: ";
    std::cin >> C;
    
    // Calculating the start and the stop index based on the specified formula
    int start_index = (size - square_size)/2;
    int stop_index = start_index + square_size;
    
    // Using the for loop for creating the 2D image with and without shifting
    for (int i=0; i<size; i++)
    {
        for (int j=0; j<size; j++)
        {
            // Initilaizing the imag values to 0
            square_imag[i][j] = 0;
            square_imag_shifted[i][j] = 0;
            
            // if the index value lie with in start and stop range then set to white
            if ((i >= start_index) && (i < stop_index) && (j >= start_index) && (j < stop_index))
            {
                square_real[i][j] = 255;
                
                // Using the property of frequency shifting for shifting the 
                // frequency at (N/2, N/2)
                square_real_shifted[i][j] = 255 * std::pow(-1, i+j);
            }
            else 
            {
                square_real[i][j] = 0;
                square_real_shifted[i][j] = 0;
            }
        }
    }
    
    // calling the 2D FFT function on non shifted image to perform 2D FFT
    fft2D(size, size, square_real, square_imag, -1);
    
    // Calling the magnitude function to calculate the magnitude and perform log based
    // contrast stretching 
    magnitude(size, C, square_real, square_imag, square_mag, output_image);
    
    // using the writeImage function to convert the matrix to .pgm file
    writeImage(argv[1], output_image);
    
    // Repeating the same steps on the shifted image
    fft2D(size, size, square_real_shifted, square_imag_shifted, -1);
    magnitude(size, C, square_real_shifted, square_imag_shifted, square_mag_shifted, output_image_shifted);
    writeImage(argv[2], output_image_shifted);
}