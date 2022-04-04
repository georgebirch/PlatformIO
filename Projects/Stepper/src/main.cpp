#include <Arduino.h>
#include <Stepper.h>
#include "OneButton.h"

// initialize the stepper library on pins 8 through 11:

const int stepsPerRevolution = 200; 
Stepper myStepper(stepsPerRevolution, 4, 5, 6, 7);

static int buttonPin = 15; // Analog pin A1.
OneButton pushButton(buttonPin, true);

int sensorPin = 14; // Pin A0.
int activationThreshold = 900;	// set the threshold voltage (in units * 5v / 1024)

int motorEnablePin = 2;

void PowerOff();
void PowerOn();

void CallWhenClicked();
void ReturntoHome();
bool atHome = false;

int stepCount = 0;  // number of steps the motor has taken
int motorSpeed = 50;

enum operatingModes
{
	InitialisingMode,
	WaitingforInputMode,
	RotatingMode,
};

int operatingMode = InitialisingMode;
int newOperatingMode = InitialisingMode;
bool justChangedMode = true;

void setup() {
	Serial.begin(9600);
	Serial.println("Stepper test!");
	myStepper.setSpeed(motorSpeed);
	Serial.println("Stepper test!");
	pushButton.attachClick( CallWhenClicked );
	PowerOn();
}

void loop() {
	pushButton.tick();

	atHome = analogRead(sensorPin) > activationThreshold ? true : false ;

	switch(operatingMode) {

		case InitialisingMode:
			if (justChangedMode) {
				justChangedMode = false;
				Serial.println("Initialising Mode" );
				Serial.println( analogRead(sensorPin) ) ;
			}
			if (atHome) {
				newOperatingMode = WaitingforInputMode;
			}
			else {
				ReturntoHome();
			}
			break;
		case WaitingforInputMode:
				if (justChangedMode) {
				justChangedMode = false;
				Serial.println("Waiting for Input Mode" );
				PowerOff();
			}
			break;
		case RotatingMode:
				if (justChangedMode) {
				justChangedMode = false;
				Serial.println("Rotating Mode" );
				PowerOn();
				delayMicroseconds(100);
			}
			if (!atHome) {
				ReturntoHome();
			}
			else{
				myStepper.step(stepsPerRevolution);
				newOperatingMode = WaitingforInputMode;
			}
			break;
	}

	if (newOperatingMode != operatingMode){
		operatingMode = newOperatingMode;
		justChangedMode = true;
		Serial.println("Just changed operating mode.");
	}
}

void CallWhenClicked() {
		/**
	* Turn the motor one revolution.
	*/
	Serial.println("Just clicked");
	newOperatingMode = RotatingMode;
}

void ReturntoHome(){
	/**
	* Turn until at the home position.
	*/
		if (!atHome) {
			myStepper.step(1);
		}
}

void PowerOff(){
	/**
	* Enable sleep mode on motor drive board
	*/
	digitalWrite(motorEnablePin, LOW);
	Serial.println("Power Off");
};

void PowerOn(){
	/**
	* Disable sleep mode on motor drivve board. 
	*/
	digitalWrite(motorEnablePin, HIGH);
	Serial.println("Power On");
}