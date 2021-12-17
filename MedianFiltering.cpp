// usage: g++ -o MedianFiltering.o MedianFiltering.cpp image.cpp
// ./MedianFiltering.o input_image.pgm corrupted_image.pgm filtered_image.pgm
#include <iostream>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include "image.h"

void corrupt_image(ImageType &image, int N, int M, int Q, int X)
{   
    // Function to corrupt the image
    // The input to this function are the image, dimensions, Quantization
    // level and the percentage of corruption.

    int rand_col, rand_row, rand_pixel;

    // Initilaizing the seed to generate a random number
    std::srand (0);

    for (int i=0; i<int((N*M)*(X/100.0));  i++)
    {
        // For example if there are 16 pixels in an image and we want to
        //randomly corrupt 50% of the image; we have to corrupt 8 pixels.
        // This for loop keeps track of the amount of pixels being corrupted.
        // The "rand" function is used to generate a random integer number [0, N), [0, M)
        // and [0, 1]. "rand_row" selects random row, "rand_col" selects randow column
        // rand_pixel either generates 0 or 1 randomly and this result is multipled with 
        // with 255 to randomly assign black or white pixels.
        std::vector<std::vector <int>> vec;
        // Vector to keep track of the coordinates
        std::vector<int> coord;
        while (std::find(vec.begin(), vec.end(), coord) != vec.end())
        {
            // This while loop make sures that no pixel co-ordinates are repeated
            // So we get the exact % of pixels to be corrupted
            coord.clear();
            rand_row = std::rand() % N;
            coord.push_back(rand_row);
            rand_col = std::rand() % M;
            coord.push_back(rand_col);
        }
        vec.push_back(coord);
        rand_pixel = std::rand() % 2;
        image.setPixelVal(rand_row, rand_col, rand_pixel*Q);
    }
}

int compute_median(ImageType input_image, int i, int j, int N, int M, int filter_size)
{
    // The vector is used to store the nieghborhood around the pixel coordinates i, j
    std::vector<int> pixel_vector;
    int size;
    int val, median_pixel;
    for (int row = i-(filter_size-1)/2; row <= i+(filter_size-1)/2; row++)
    {
        for (int col = j-(filter_size-1)/2; col <= j+(filter_size-1)/2; col++)
        {
            // both the outer and inner for loop iterates across the width and height of the filter window
            if (row < 0 || col < 0 || row >=N || col >= M);
            // if the "row" or the "col" variable is negative or >= N and M respectively
            // it indicates the filter is out of our image dimensions
            // We can either pad zeros or just copy the central pixel value
            else 
            {
                input_image.getPixelVal(row, col, val);
                pixel_vector.push_back(val);
            }

        }
    }
    size = pixel_vector.size();
    // We sort our pixel vector using the sort command
    std::sort(pixel_vector.begin(), pixel_vector.end());
    // As our filter_size is always odd the size of our vector should also be odd
    // But if the size of the vector is even (when the filter is out of bound) then we take the average of the two
    // center values and return them
    if (size %2 == 0)
    {
        // This indicates the size of our vector is even
        median_pixel = int((pixel_vector[(size-1)/2] + pixel_vector[size/2])/2);
    }
    else {median_pixel = pixel_vector[size/2];}
    return median_pixel;

}

void median_filtering(ImageType input_image, ImageType &output_image, int N, int M, int filter_size)
{
    int median_pixel;
    int i, j;
    // We parse through each of the input image co-ordinates
    for (i=0; i<N; i++)
    {
        for(j=0; j<M; j++)
        {
            // The "compute_median" filtering function identifies the neighborhood of 
            // pixels centered around (i, j) and identifies the median of that neighborhood
            // and returns the median. We set the median values to our output image co-ordinates
            median_pixel = compute_median(input_image, i, j, N, M, filter_size);
            output_image.setPixelVal(i, j, median_pixel);
        }
    }
}

int main(int argc, char *argv[])
{
    int N, M, Q;
    bool type;
    int filter_size = 0;
    int X = 30;

    // Prompting user to enter the size of the median filter
    // Asking user to enter the size until its a odd number
    while (filter_size % 2 == 0)
    {
        std::cout <<"Enter the size of the median filter"<<std::endl;
        std::cout<<"You can enter only an odd number"<<std::endl;
        std::cin>> filter_size;
    }
    // read image header
    readImageHeader(argv[1], N, M, Q, type);

    // allocate memory for the input image array
    ImageType input_image(N, M, Q);

    // Using th copy constructor to allocate memory for the output image
    ImageType output_image(input_image);
    
    // read image
    readImage(argv[1], input_image);

    // Calling the "corrupt_image" function to corrupt the image with the salt and pepper noise
    corrupt_image(input_image, N, M, Q, X);

    // To convert the corrupted image matrix to .pgm files
    writeImage(argv[2], input_image);

    // Function call to perform median filtering
    median_filtering(input_image, output_image, N, M, filter_size);

    // To convert the corrupted image matrix to .pgm files
    writeImage(argv[3], output_image);
    
    return (0);
}