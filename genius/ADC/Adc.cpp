#include "Adc.h"

ADC::ADC(AINx id)
{
	this->adcNumber = id;
	this->baseDir = "/sys/bus/iio/devices/iio:device0/in_voltage" + std::to_string(this->adcNumber) + string("_raw");
	loadAdcConfig();
}

void ADC::loadAdcConfig()
{
	//TODO Adicionar clausula que checa se comando ja foi executado
	system("sudo sh -c \"echo 'BB-ADC' > /sys/devices/platform/bone_capemgr/slots\"");
}

int ADC::getIntValue()
{
	int retorno;
	fstream fs;
	fs.open(this->baseDir.c_str(), fstream::in);
	fs >> retorno;
	fs.close();
	return retorno;
}

float ADC::getFloatValue()
{
	int temp = getIntValue();
	float retorno;
	retorno = (temp*1.8f)/4096.0;
	return retorno;
}

float ADC::getPercentValue()
{
	int temp = getIntValue();
	float retorno;
	retorno = (temp*100.0f)/4096.0;
	return retorno;
}

