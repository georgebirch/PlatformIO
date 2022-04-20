#include <Arduino.h> // Comment out, if using Arduino IDE
#include <Stepper.h>
#include "OneButton.h" // Change to < ... > if using arduino IDE

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
void CallWhenPressed();
void ReturntoHome();

enum operatingModes
{	StandbyMode,
	InitialisingMode,
	WaitingforInputMode,
	RotatingMode,
};

int operatingMode = StandbyMode;
int newOperatingMode = StandbyMode;
bool justChangedMode = true;
unsigned long timer = 0;
int sleepTimer = 5000; // Length of time the motor remains powered during inactivity. 

void setup() {
	Serial.begin(9600);
	myStepper.setSpeed(motorSpeed);
	Serial.println("Stepper test!");
	pushButton.attachClick( CallWhenClicked );
	pushButton.attachLongPressStart( CallWhenPressed );
	PowerOff();
}

void loop() {
	pushButton.tick();
	atHome = analogRead(photoSensorPin) < activationThreshold ? true : false ;
	switch(operatingMode) {
	/**
	* Start by waiting for a button input. 
	* On first button input, Initialise by moving to home position, then wait for button input.
	* On button click input, rotate 2 full revolutions.
	* Clicking during rotation stops the motor.
	* Power is cut to motor if inactive for sleeptimer seconds. Motor will stop holding position. This prevents too much heat buildup. 
	* Power is cut to the motor if hoem position is not found within sleeptimer.
	* Motor can also return to home position with a long press
	**/
		case StandbyMode:
			if (justChangedMode){
				justChangedMode = false;
				Serial.println("Standby Mode Mode" );
				PowerOff();
			}
			break;
		case InitialisingMode: 
			if (justChangedMode){
				justChangedMode = false;
				Serial.println("Initialising Mode" );
				PowerOn();
				timer = millis();
			}
			if (atHome){
				newOperatingMode = WaitingforInputMode;
			}
			else {
				ReturntoHome();
			}
			if (millis() - timer > sleepTimer){ // In case home position never triggers, don't spin forever 
				newOperatingMode = StandbyMode;
			}
			break;
		case WaitingforInputMode:
			if (justChangedMode){
				justChangedMode = false;
				Serial.println("Waiting for Input Mode" );
				timer = millis();
				stepCount = 0;
			}
			if (millis() - timer > sleepTimer){ // Stay in same mode but power off motor. 
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
			myStepper.step(1);
			stepCount++;
			if (stepCount == stepsPerRevolution * revolutionsPerCycle){
				newOperatingMode = WaitingforInputMode;
				stepCount = 0;
			}
			break;
	}

	if (newOperatingMode != operatingMode){
		operatingMode = newOperatingMode;
		justChangedMode = true;
		Serial.println("Just changed operating mode to:");
		Serial.println();		
	}
}

void CallWhenClicked(){
	/**
	* Change state, depending on current state. 
	*/
	Serial.println("<Button click>");
	if (operatingMode == StandbyMode){
		newOperatingMode = InitialisingMode;
	}
	else if (operatingMode == WaitingforInputMode){
		newOperatingMode = RotatingMode;
	}
	else {newOperatingMode = WaitingforInputMode;}
	// if (operatingMode == RotatingMode){
	// 	stepCount = 0;
	// }
}

void CallWhenPressed(){
	/**
	* If the button is long-pressed, go home.
	*/
	Serial.println("<Button  press>");
	newOperatingMode = InitialisingMode;
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