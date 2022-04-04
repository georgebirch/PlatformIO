#include <Arduino.h>

#include <Stepper.h>

const int stepsPerRevolution = 200;  // change this to fit the number of steps per revolution
// for your motor


// initialize the stepper library on pins 8 through 11:
Stepper myStepper(stepsPerRevolution, 4, 5, 6, 7);

int stepCount = 0;  // number of steps the motor has taken
int speed = 5;
void setup() {
  // nothing to do inside the setup
    Serial.begin(9600);
    Serial.println("Stepper test!");
}

void loop() {
  // // read the sensor value:
  // int sensorReading = analogRead(A0);
  // // map it to a range from 0 to 100:
  // int motorSpeed = map(sensorReading, 0, 1023, 0, 100);
  // // set the motor speed:
  // if (motorSpeed > 0) {
  //   myStepper.setSpeed(motorSpeed);
  //   // step 1/100 of a revolution:
  //   myStepper.step(stepsPerRevolution / 100);
  // }
    // for (int i=1; i < 40; i++) {
    //   myStepper.setSpeed(i);
    //   myStepper.step(100 * i / stepsPerRevolution);
    //   Serial.println("Stepped: ");
    //   Serial.println(i);
    // }

    myStepper.setSpeed(50);
    myStepper.step(speed);

    // speed = speed + 1;
    // if (speed > 100) {
    //   speed = 10;
    // }



    // step 1/100 of a revolution:

}
