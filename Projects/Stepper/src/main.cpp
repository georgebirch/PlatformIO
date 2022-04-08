#include <Arduino.h>
#include <Stepper.h>
#include "OneButton.h"

const int stepsPerRevolution = 200; 
const int revolutionsPerCycle = 2;
Stepper myStepper(stepsPerRevolution, 4, 5, 6, 7);
int motorEnablePin = 2;
int stepCount = 0;  // number of steps the motor has taken
int motorSpeed = 45;

static int buttonPin = 3; // Analog pin A1.
OneButton pushButton(buttonPin, true);

int photoSensorPin = 14; // Pin A0.
int activationThreshold = 150;	// Threshold voltage for the optical sensor (in units * 5v / 1024)
bool atHome = false;
int AverageAnalogRead(int inputPin, int numberOfSamples);

void PowerOff();
void PowerOn();

void CallWhenClicked();
void ReturntoHome();

enum operatingModes
{
	InitialisingMode,
	WaitingforInputMode,
	RotatingMode,
};
  
int operatingMode = InitialisingMode;
int newOperatingMode = InitialisingMode;
bool justChangedMode = true;
unsigned long timer = 0;
int sleepTimer = 3000; // Length of time the motor remains powered during inactivity. 

void setup() {
	Serial.begin(9600);
	myStepper.setSpeed(motorSpeed);
	Serial.println("Stepper test!");
	pushButton.attachClick( CallWhenClicked );
	PowerOn();
}

void loop() {
	pushButton.tick();
	atHome = analogRead(photoSensorPin) < activationThreshold ? true : false ;
	switch(operatingMode) {
	/**
	* Initialise by moving to home position, then wait for button input.
	* Pressing the button also cancels movement.
	* Power is cut to motor if inactive for 3s. 
	**/
		case InitialisingMode: 
			if (justChangedMode){
				justChangedMode = false;
				Serial.println("Initialising Mode" );
			}
			if (atHome){
				newOperatingMode = WaitingforInputMode;
			}
			else {
				ReturntoHome();
			}
			break;
		case WaitingforInputMode:
			if (justChangedMode){
				justChangedMode = false;
				Serial.println("Waiting for Input Mode" );
				timer = millis();
				Serial.print(timer);
			}
			if (millis() - timer > sleepTimer){
				PowerOff();
			}
			break;
		case RotatingMode:
			if (justChangedMode){
				justChangedMode = false;
				Serial.println("Rotating Mode" );
				PowerOn();
				delayMicroseconds(100);
			}
			if (!atHome){	// Check if rotor is in home position, if not return to home. 
				ReturntoHome();
			}
			else {
				myStepper.step(1);
				stepCount++;
				if (stepCount == stepsPerRevolution * revolutionsPerCycle){
					newOperatingMode = WaitingforInputMode;
					stepCount = 0;
				}
			}
			break;
	}

	if (newOperatingMode != operatingMode){
		operatingMode = newOperatingMode;
		justChangedMode = true;
		Serial.println("Just changed operating mode.");
	}
}

void CallWhenClicked(){
	/**
	* Initiate or cancel the motor, depending on current state. 
	*/
	Serial.println("Just clicked");
	if (operatingMode == WaitingforInputMode){
		newOperatingMode = RotatingMode;
	}
	else {newOperatingMode = WaitingforInputMode;}
	if (operatingMode == RotatingMode){
		stepCount = 0;
	}
}

void ReturntoHome(){
	/**
	* Turn until at the home position.
	*/
		if (!atHome){
			myStepper.step(1);
		}
}

void PowerOff(){
	/**
	* Enable sleep mode on motor drive board
	*/
	digitalWrite(motorEnablePin, LOW);
	// Serial.println("Power Off");
}

void PowerOn(){
	/**
	* Disable sleep mode on motor drivve board. 
	*/
	digitalWrite(motorEnablePin, HIGH);
	// Serial.println("Power On");
}

int AverageAnalogRead(int inputPin, int numberOfSamples){
	/**
	* Take an average sampling of the analog input. 
	* Can be used to help set the activationthreshold. 
	*/
	int sum = 0;
	for (int i = 0; i < numberOfSamples; i++){
		sum = sum + analogRead(inputPin);
	}
	return sum / numberOfSamples;
}