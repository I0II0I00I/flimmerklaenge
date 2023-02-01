////////////////////////////////////////////
//Flickermeter for Max/MSP by Juri Rendler//
////////////////////////////////////////////

//import necessary library
#include <Statistical.h>

//init constants
const int interval = 100;  // interval at which to print results i.e. every second
const int numberReadings = 400; // number of readings (300 is maximum of available memory)
//init variables 
uint16_t freq = 0; // current frequency measurement
uint16_t value = 0; // current voltage measument
int probeNumer = 0; // probe counter
float mean = 0; // mean
// init arrays
uint16_t readings[numberReadings]; // array to store measurements
int readingsIndex = 0; // index for indexing into readings array
uint16_t freqMessung[numberReadings]; // array to store frequency measurements (300 is maximum of available memory)
int frequencyIndex = 0; // index for indexing into frequency array
// "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0; 
unsigned long freqTime = 0;
unsigned long measurementTime = 0;

// function setup for 
void setup() 
{
  Serial.begin(9600); // setup baudrate, default = 9600
}

// function infinite loop over measurements: reading, storing and calculations
void loop()
{
  unsigned long currentMillis = millis(); // get current time

  // three condition to check if the current measurement is above or below the current mean. 
  // the current mean is calculated over 300 measurements. This can be adjusted in line 10 
  // with "numberReadings"
  value = analogRead(A0); // read sensor 0 i.e. A0 (perform first measurement)
  if (value >= mean && probeNumer == 0) // at any time when our measurement is greater than the current mean a frequency measurement starts (frequency = 1/T , T - time of one cycle)
    {
    freqTime = millis(); // start measuring duration of a cycle
    probeNumer = 1; // set probe counter to 1 i.e. the circle begins
    }

  value = analogRead(A0); // read sensor 0 i.e. A0 (perform second measurement)
  if(value <= mean && probeNumer == 1)
    {
    probeNumer = 2; // set probe counter to 2 i.e. we are somewhere but surely in the middle of the circle
    }

  value = analogRead(A0); // read sensor 0 i.e. A0 (perform third measurement)
  if (value >= mean && probeNumer == 2)
    {
    freqTime = millis() - freqTime; // stop measureing, the circle is complete i.e. we have the duration of one circle
    probeNumer = 0; // reset probe number
    freq = 1000 / freqTime; // calculate frequency (nominator determined by millis() i.e. measures in milliseconds) 
    freqMessung[frequencyIndex] = freq; // write reading into array
    frequencyIndex ++; //inrease index by one
    
    if (frequencyIndex > sizeof(freqMessung)/2) // if index is larger than arraysize, reset index
      {
      frequencyIndex = 0; //reset array index
      }
    }

  readings[readingsIndex] = value; // write reading into array
  readingsIndex++; // increase readingsIndex by one
  //if readingsIndex is larger than arraysize, reset index
  if (readingsIndex > sizeof(readings)/2) 
    {
    readingsIndex = 0; // reset array index
    if (currentMillis - previousMillis >= interval) // check if measurement interval is reached
      {
      previousMillis = currentMillis; // save output time 
      // using statistics libary for readings and freqMessung array
      Array_Stats<uint16_t> StatisticsReadings(readings, sizeof(readings) / sizeof(readings[0])); //calculate statistics of readings
      mean = StatisticsReadings.Average(StatisticsReadings.Arithmetic_Avg) ; // calculate mean of readings
      Array_Stats<uint16_t> StatisticsFrequency(freqMessung, sizeof(freqMessung) / sizeof(freqMessung[0])); // calculate statistic of frequency

      // OUTPUT TO SERIAL
      Serial.print(mean);
      Serial.print(" ");
      Serial.print(StatisticsReadings.Min());
      Serial.print(" ");
      Serial.print(StatisticsReadings.Max());
      Serial.print(" ");
      Serial.print(value);
      Serial.print(" ");
      Serial.println(StatisticsFrequency.Quartile(2));
      }
    }  
} 