#ifndef ADC_H_
#define ADC_H_

#include<iostream>
#include<fstream>
#include<string>
#include<stdio.h>
#include<sstream>

using namespace std;

enum AINx {
	AIN0 = 0,
	AIN1 = 1,
	AIN2 = 2,
	AIN3 = 3,
	AIN4 = 4,
	AIN5 = 5,
	AIN6 = 6
};

class ADC
{
	private:
		int adcNumber;
		string baseDir;

		void loadAdcConfig();

	public:
		ADC(AINx id);
		int getIntValue();
		float getFloatValue();
		float getPercentValue();
};

#endif /* ADC_H_ */
