#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <wiringPi.h>
#include <stdlib.h>

typedef struct {
	int temp;
	int perc;
} tempItem;

tempItem profile[100];

const int PWM_pin = 1; //GPIO 1 as per WiringPi, GPIO 18 as per BCM
const uint8_t sleepTime = 10; // seconds

uint8_t readConfig(char *fileName, tempItem *item);
void setupPwm();
double readTemperature();
void writeToPwm(tempItem *item, uint8_t *profileSize, double *tValue);

void main(int argc, char *argv[])
{
	// read config
	uint8_t profileLineCount = readConfig(argv[1], profile);
	if(profileLineCount == 0)
	{
		printf("Configuration file is empty. Exiting...\n");
		exit(1);
	}
	
	// setup PWM
	setupPwm();
	pwmWrite(PWM_pin, 0);
	sleep(sleepTime);
	
	// main loop	
	while(1)
	{	
		// get temperature value
		double tValue = readTemperature();
		
		// calculate and write pwm value		
		writeToPwm(profile, &profileLineCount, &tValue);
		
		sleep(sleepTime);
	}
}

uint8_t readConfig(char *fileName, tempItem *item)
{
	FILE *config;
	
	printf(fileName);
	
	// config file open
	config = fopen(fileName, "r");
	if(config == NULL)
	{
		printf("Configuration file can not be read. Exiting...\n");
		exit(1);
	}
	
	// config read
	uint8_t counter = 0;
	while(fscanf(config, "%i,%i\n", &(item[counter].temp), &(item[counter].perc)) != EOF)
	{
		counter++;
	}
	
	fclose(config);
	return counter;
}


void setupPwm()
{
	// PWM setup
	if(wiringPiSetup() == -1)
	{
		printf("WiringPiSetup failure. Exiting...\n");
		exit(1);
	}
	pinMode(PWM_pin, PWM_OUTPUT);
}


double readTemperature()
{
	FILE *temperatureFile;
	double tValue;
	temperatureFile = fopen("/sys/class/thermal/thermal_zone0/temp", "r");
	if(temperatureFile == NULL)
	{
		printf("File value is NULL");
		exit(1);
	}
	fscanf(temperatureFile, "%lf", &tValue);
	fclose(temperatureFile);
	tValue /= 1000;
	
	return tValue;
}

void writeToPwm(tempItem *item, uint8_t *profileSize, double *tValue)
{
	uint8_t i;
	for(i = 0; i <= *profileSize; i++)
	{	
		if(item[i].temp > *tValue)
		{	uint16_t pwmValue  = 1023 * item[i].perc / 100;
			printf("temp: %6.2f C -> fan: %d -> PWM: %d\n", *tValue, item[i].perc, pwmValue);
			pwmWrite(PWM_pin, pwmValue);
			return;
		}
	}
	pwmWrite(PWM_pin, 1023); // fail-safe in case profile was not matched (spin fan at 100%)	
}