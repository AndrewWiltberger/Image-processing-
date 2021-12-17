#include <iostream>
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

void amplification(ImageType &image, float A, int N, int M)
{
    // This function is used to amplify the input image
    // each value is multiplied with the amplification 
    // factor A
    int val;
    for (int i=0; i<N; i++)
    {
        for (int j=0; j<M; j++)
        {
            image.getPixelVal(i, j, val);
            val *= A;
            image.setPixelVal(i, j, val);
        }
    }
}

void subtraction(ImageType &input_image, ImageType ref_image, int N, int M)
{   
    // Subtracting the input image with blurred reference image
    // to create a high pass image
    int input_val, ref_val;
    int min_pixel = 255;
    int max_pixel = 0;
    for (int i=0; i<N; i++)
    {
        for (int j=0; j<M; j++)
        {
            input_image.getPixelVal(i, j, input_val);
            ref_image.getPixelVal(i, j, ref_val);
            input_val -= ref_val;
            input_image.setPixelVal(i, j, input_val);
            if (input_val <= min_pixel) {min_pixel = input_val;}
            if (input_val >= max_pixel) {max_pixel = input_val;}
        }
    }
    mapper(input_image, min_pixel, max_pixel, N, M);
}

int main(int argc, char *argv[])
{
    int N, M, Q;
    bool type;
    float A;

    // read image header
    readImageHeader(argv[1], N, M, Q, type);

    // allocate memory for the input image array
    ImageType input_image(N, M, Q);
    ImageType ref_image(input_image);

    // read image
    readImage(argv[1], input_image);
    readImage(argv[2], ref_image);

    // Prompting user to input the value of A
    std::cout<<"Enter the value of Amplification (A)"<<std::endl;
    std::cout<<"If A == 1, then the algorithm performs Unsharp Masking"<<std::endl;
    std::cout<<"If A > 1, then the algorithm performs High Boost Filtering"<<std::endl;
    std::cin >> A;

    amplification(input_image, A, N, M);
    subtraction(input_image, ref_image, N, M);

    writeImage(argv[3], input_image);

}