
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <math.h>



# define M_PI 3.14159265358979323846
#define SWAP(a,b) tempr=(a);(a)=(b);(b)=tempr

void fft(float data[], unsigned long nn, int isign)
{
	unsigned long n,mmax,m,j,istep,i;
	double wtemp,wr,wpr,wpi,wi,theta;
	float tempr,tempi;

	n=nn << 1;
	j=1;
	for (i=1;i<n;i+=2) {
		if (j > i) {
			SWAP(data[j],data[i]);
			SWAP(data[j+1],data[i+1]);
		}
		m=n >> 1;
		while (m >= 2 && j > m) {
			j -= m;
			m >>= 1;
		}
		j += m;
	}
	mmax=2;
	while (n > mmax) {
		istep=mmax << 1;
		theta=isign*(6.28318530717959/mmax);
		wtemp=sin(0.5*theta);
		wpr = -2.0*wtemp*wtemp;
		wpi=sin(theta);
		wr=1.0;
		wi=0.0;
		for (m=1;m<mmax;m+=2) {
			for (i=m;i<=n;i+=istep) {
				j=i+mmax;
				tempr=wr*data[j]-wi*data[j+1];
				tempi=wr*data[j+1]+wi*data[j];
				data[j]=data[i]-tempr;
				data[j+1]=data[i+1]-tempi;
				data[i] += tempr;
				data[i+1] += tempi;
			}
			wr=(wtemp=wr)*wpr-wi*wpi+wr;
			wi=wi*wpr+wtemp*wpi+wi;
		}
		mmax=istep;
	}
}
#undef SWAP


void sampleCos(std::vector<float> & signal) {
    const int N = 128, u = 8;
    signal.resize(2 * N);

    int j = 0;
    for (int i = 0; i < N; ++i) {
        signal[j] = std::cos(2 * M_PI * u * i / N); //add real part
        j++;
        signal[j] = 0; //add imaginary part 0 for now
        j++;
    }
}

int main(){
	std::vector<float> samples;

	sampleCos(samples); //sample cos(2pi8x)

	//flip to visualize a whole period
	for (unsigned int i = 2; i < samples.size(); i += 4)
	{
		   samples[i] *= -1;
	}

	fft(&samples[0]-1, samples.size()/2, -1);


	for(int i = 0; i < samples.size(); i++)	//to normalize / every element by N
	{
		samples[i] /= samples.size()/2;
	}


	std::ofstream Real, Imaginary, Phase, Magnitude;
	Real.open("Real.txt");
	Imaginary.open("Imaginary.txt");
	Phase.open("Phase.txt");
	Magnitude.open("Magnitude.txt");

	for(int i = 0; i < samples.size(); i++)
	{
		if(samples[i] < 1.0e-6 && samples[i] > -1.0e-6)
		{
			samples[i] = 0;
		}
		std::cout<< samples[i] << std::endl;


		if(i == 0 || i%2 == 0)
		{
			Real << samples[i] << std::endl;

			//compute the magnitude parts 
			float mag = (std::sqrt(samples[i]*samples[i]) + samples[i+1]*samples[i+1]);
			Magnitude << mag <<std::endl;
			float phase = std::atan2(samples[i + 1], samples[i]);
			Phase << phase << std::endl;

		}
		else if(i%2 != 0)
		{
			Imaginary << samples[i] << std::endl;
		}

	}

	Real.close();
	Imaginary.close();
	Magnitude.close();
	Phase.close();






	return 0;
}
