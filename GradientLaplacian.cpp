// usage g++ -o GradientLaplacian.o GradientLaplacian.cpp image.cpp
// GradientLaplacian.o input_image.pgm prewitt.pgm prewitt_added.pgm sobel.pgm
// sobel_added.pgm laplacian.pgm laplacian_added.pgm  
#include <iostream>
#include <vector>
#include <numeric>
#include <cmath>
#include <climits>
#include "image.h"

void mapper(ImageType &image, int pixel_min, int pixel_max, int N, int M)
{
    // This function is used to map the pixel values ranging from [pixel_min, pixel_max]
    // to [0, 255] using the equation y = mx + c
    int val;
    for (int i=0; i<N; i++)
    {
        for (int j=0; j<M; j++)
        {
            image.getPixelVal(i, j, val);
            val = int((255.0/(pixel_max - pixel_min))*(val - pixel_min));
            image.setPixelVal(i, j, val);
        }
    }
}

void adder(ImageType input_image, ImageType &output_image, int N, int M)
{
    // This function is used to add the edge map to the original map
    // Then we apply the mapper function to bring the range between [0, 255]
    int pixel_min = 255;
    int pixel_max = 0;
    int input_val, output_val;
    for (int i=0; i<N; i++)
    {
        for (int j=0; j<M; j++)
        {
            input_image.getPixelVal(i, j, input_val);
            output_image.getPixelVal(i, j, output_val);
            output_val += input_val;
            output_image.setPixelVal(i, j, output_val);
            if (output_val <= pixel_min) {pixel_min = output_val;}
            if (output_val >= pixel_max) {pixel_max = output_val;}
        }
    }
    mapper(output_image, pixel_min, pixel_max, N, M);
}

int sliding(ImageType input_image, int mask[3][3], int i, int j, int N, int M, int &pixel_min, int &pixel_max)
{
    // This function is used to perform correlation between between the mask
    // and the local neighborhood of pixels. The mask size is fixed to be 
    // three. The pixels centered around (i,j) should iterate from i-1 to i+1
    // and j-1 to j+1 to calculate the correlation. If any of the co-ordinate
    // values are out of bound we pad zero.
    int mask_row = 0, mask_col = 0;
    int mask_sum;
    int val;
    float push;
    std::vector<float> pixel_vector;

    for (int row=i-1; row<=i+1; row++)
    {
        mask_col = 0;
        for (int col=j-1; col<=j+1; col++)
        {
            if (row < 0 || col < 0 || row >= N || col >= M) {pixel_vector.push_back(0.0);}
            else
            {
                input_image.getPixelVal(row, col, val);
                push = (val*mask[mask_row][mask_col])/9.0;
                pixel_vector.push_back(push);
            }
            mask_col++;
            
        }
        mask_row++;
    }
    // accumulate function to find the sum of the vector
    mask_sum = int(std::accumulate(pixel_vector.begin(), pixel_vector.end(), 0));
    if (mask_sum <= pixel_min) {pixel_min = mask_sum;}
    if (mask_sum >= pixel_max) {pixel_max = mask_sum;}
    return (mask_sum);
}

void correlation(ImageType input_image, ImageType &output_image, int mask[3][3], int N, int M, int &pixel_min, int &pixel_max)
{
    // Iterating thorugh the indices of the input image dimensions
    // Calling the function sliding which will perform correlation between
    // the mask and the local neighborhood of the pixels
    int pixel_val;
    pixel_min = 255;
    pixel_max = 0;
    for (int i=0; i<N; i++)
    {
        for (int j=0; j<M; j++)
        {
            pixel_val = sliding(input_image, mask, i, j, N, M, pixel_min, pixel_max);
            output_image.setPixelVal(i, j, pixel_val);
            if (pixel_val <= pixel_min) {pixel_min = pixel_val;}
            if (pixel_val >= pixel_max) {pixel_max = pixel_val;}
        }
    }
}

void magnitude(ImageType image_X, ImageType image_Y, ImageType &output_image, int N, int M, int &pixel_min, int &pixel_max)
{
    // This function is used to calculate the magnitude map from the 
    // gradient maps. Then we apply the mapper function to bring it 
    // in the range of [0, 255]
    int x_val, y_val, mag;
    pixel_min = 255;
    pixel_max = 0;
    for (int i=0; i<N; i++)
    {
        for (int j=0; j<M; j++)
        {
            image_X.getPixelVal(i, j, x_val);
            image_Y.getPixelVal(i, j, y_val);
            mag = int(std::pow((std::pow(x_val,2) + std::pow(y_val,2)), 0.5));
            output_image.setPixelVal(i, j, mag);
            if (mag <= pixel_min) {pixel_min = mag;}
            if (mag >= pixel_max) {pixel_max = mag;}
        }
    }
}

int main(int argc, char *argv[])
{
    int N, M, Q;
    bool type;
    int pixel_min, pixel_max;

    //read image header
    readImageHeader(argv[1], N, M, Q, type);

    // allocate memory for the input image array
    ImageType input_image(N, M, Q);

    // Using the copy constructor to allocate memory for the output images
    ImageType prewitt_x(input_image);
    ImageType prewitt_y(input_image);
    ImageType prewitt_magnitude(input_image);
    ImageType sobel_x(input_image);
    ImageType sobel_y(input_image);
    ImageType sobel_magnitude(input_image);
    ImageType laplacian(input_image);

    // initializing the weights for each of the filters
    int prewitt_x_mask[3][3] = {{-1, -1, -1}, {0, 0, 0}, {1, 1, 1}};
    int prewitt_y_mask[3][3] = {{-1, 0, 1}, {-1, 0, 1}, {-1, 0, 1}};
    int sobel_x_mask[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};
    int sobel_y_mask[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    int laplacian_mask[3][3] = {{0, 1, 0}, {1, -4, 1}, {0, 1, 0}};

    // read image
    readImage(argv[1], input_image);

    // Performing correlation on the input image using the prewitt mask
    // calculating the gradient magnitude of the image using the
    // prewitt mask
    correlation(input_image, prewitt_x, prewitt_x_mask, N, M, pixel_min, pixel_max);
    correlation(input_image, prewitt_y, prewitt_y_mask, N, M, pixel_min, pixel_max);
    magnitude(prewitt_x, prewitt_y, prewitt_magnitude, N, M, pixel_min, pixel_max);

    // Copying the magnitude values to different object for the purpsoe of visualization
    ImageType prewitt_added(prewitt_magnitude);

    // Calling the mapper function on the original object to bring it to range [0, 255]
    mapper(prewitt_magnitude, pixel_min, pixel_max, N, M);
    
    // to convert the prewitt matrix to .pgm file
    writeImage(argv[2], prewitt_magnitude);

    // adding the input image to the copied magnitude values which are not in the range of [0, 255]
    // We are exclusively not calling mapper function after adder as the adder function calls the 
    // mapper function automatically
    adder(input_image, prewitt_added, N, M);

    // to convert the added images to .pgm file
    writeImage(argv[3], prewitt_added);

    // Performing correlation on the input image using the sobel mask
    // calculating the gradient magnitude of the image using the
    // sobel mask
    correlation(input_image, sobel_x, sobel_x_mask, N, M, pixel_min, pixel_max);
    correlation(input_image, sobel_y, sobel_y_mask, N, M, pixel_min, pixel_max);
    magnitude(sobel_x, sobel_y, sobel_magnitude, N, M, pixel_min, pixel_max);

    // Copying the magnitude values to different object for the purpsoe of visualization
    ImageType sobel_added(sobel_magnitude);

    // Calling the mapper function on the original object to bring it to range [0, 255]
    mapper(sobel_magnitude, pixel_min, pixel_max, N, M);

    // to convert the sobel matrix to .pgm file
    writeImage(argv[4], sobel_magnitude);

    // adding the input image to the copied magnitude values which are not in the range of [0, 255]
    adder(input_image, sobel_added, N, M);

    // to convert the added matrix to .pgm file
    writeImage(argv[5], sobel_added);

    // Performing correlation on the input image using the Laplacian
    // No need of calculating the gradient magnitude separately
    correlation(input_image, laplacian, laplacian_mask, N, M, pixel_min, pixel_max);

    // Copying the laplacian values to a different object for the purpose of visualization
    ImageType laplacian_added(laplacian);

    //Calling the mapper function on the original object to bring the range of [0, 255]
    mapper(laplacian, pixel_min, pixel_max, N, M);

    // to convert the laplacian matrix to .pgm file
    writeImage(argv[6], laplacian);

    // adding the laplacian matrix which is not in the range of [0, 255] to the input image
    adder(input_image, laplacian_added, N, M);

    // to convert the added matrix to .pgm files
    writeImage(argv[7], laplacian_added);
    
    return(0);
}