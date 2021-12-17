
#include <math.h>
#include <iostream>

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


int main()
{

	//data[0] not used
	//odd index are real parts
	//even are imagiary parts 
	unsigned long nn = 4;
	int forward = -1;
	//data given by assignment 
	float data[9] = {0, 2, 0, 3, 0, 4, 0, 4, 0,};
	std::cout<< "Original Sample" <<std::endl;
	for(int i = 1; i <= 8; i++)
	{
		//
		//data[i] = data[i] * .25;
		if(i%2 != 0)
		{

			std::cout << data[i];
		}
		else
		{
			std::cout << " + " << data[i] << "j" <<std::endl;
			//double mag = sqrt((data[i-1]*data[i-1]) + (data[i]*data[i]));
			//std::cout << " Magnitude: " << mag << std::endl;
		}

	}

	//compute the forward fft f(x) -> F(u)
	fft(data, nn, forward);

	//print result forward dft
	std::cout << "F(u)" << std::endl;
	for(int i = 1; i <= 8; i++)
	{
		//
		data[i] = data[i] * .25;
		if(i%2 != 0)
		{

			std::cout << data[i];
		}
		else
		{
			std::cout << " + " << data[i] << "j";
			double mag = sqrt((data[i-1]*data[i-1]) + (data[i]*data[i]));
			std::cout << " Magnitude: " << mag << std::endl;
		}

	}
	//compute the inverse fft F(u) -> f(x)
	//we should get the original array back

	fft(data, nn, 1);


	std::cout << "f(x)" << std::endl;
	for(int i = 1; i <= 8; i++)
	{
		if(i%2 == 0)
		{
			if(data[i] < 0.00001)
			{
				std::cout << " + " << "0" << "j" << std::endl;
			}
			else
			{
				std::cout << " + " << data[i] << "j" << std::endl;
			}	
		}
		else
		{
			std::cout << data[i];
		}
	}



	return(1);
}