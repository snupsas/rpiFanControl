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
const uint8_t sleepTime = 5; // seconds

uint8_t readConfig(tempItem *item);
void setupPwm();
double readTemperature();
void writeToPwm(tempItem *item, uint8_t *profileSize, double *tValue);

void main()
{
	// read config
	uint8_t profileLineCount = readConfig(profile);
	if(profileLineCount == 0)
	{
		printf("Configuration file is empty. Exiting...\n");
		exit(1);
	}
	
	// setup PWM
	setupPwm();
	
	// main loop	
	while(1)
	{	
		// get temperature value
		double tValue = readTemperature();
		
		// calculate and write pwm value		
		writeToPwm(profile, &profileLineCount, &tValue);
		
		printf("%6.3f C.\n", tValue);
		sleep(sleepTime);
	}
}

uint8_t readConfig(tempItem *item)
{
	FILE *config;
	
	// config file open
	config = fopen("fanControl.cfg", "r");
	if(config == NULL)
	{
		printf("Configuration file can not be read. Exiting...\n");
		exit(1);
	}
	
	// config read
	uint8_t counter = 0;
	while(fscanf(config, "%i,%i\n", &(item->temp), &(item->perc)) != EOF)
	{
		counter++;
		item++;
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
	tempItem *profileValue;
	
	for(i = 0; i <= *profileSize; i++)
	{	
		profileValue = item + i;
		if(profileValue->temp > *tValue)
		{	uint16_t pwmValue  = 1023 * profileValue->perc / 100;
			printf("fan: %d -> PWM: %d\n", profileValue->perc, pwmValue);
			pwmWrite(PWM_pin, pwmValue);
			return;
		}
	}
	pwmWrite(PWM_pin, 100); // fail-safe in case profile was not matched (spin fan at 100%)	
}