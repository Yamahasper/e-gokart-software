#include <Arduino.h>
#include <vesc_can_bus_arduino.h>
#include <SPI.h>
#include <mcp_can.h>

const int HallThrottlePin = 34;
const int HallBrakePin = 35;
const int numReadings = 5;
int throttleReadings[numReadings] = {};
int throttleIndex = 0;
int brakeReadings[numReadings] = {};
int brakeIndex = 0;

CAN can;

float currentSpeed = 0.0;
float decayRate = 0.00005f;

bool print_realtime_data;
long last_print_data;

float MotorValueForVESC() {
  throttleReadings[throttleIndex] = analogRead(HallThrottlePin);

  throttleIndex = (throttleIndex + 1) % numReadings;

  int sumThrottle = 0;
  for (int i = 0; i < numReadings; ++i) {
    sumThrottle += throttleReadings[i];
  }

  float averageThrottleValue = float(sumThrottle) / numReadings;

  float normalizedThrottleValue = float(averageThrottleValue - 940) / (3090 - 940);
  normalizedThrottleValue = constrain(normalizedThrottleValue, 0.0, 1.0);

  brakeReadings[brakeIndex] = analogRead(HallBrakePin);

  brakeIndex = (brakeIndex + 1) % numReadings;

  int sumBrake = 0;
  for (int i = 0; i < numReadings; ++i) {
    sumBrake += brakeReadings[i];
  }

  float averageBrakeValue = float(sumBrake) / numReadings;

  float normalizedBrakeValue = float(averageBrakeValue - 940) / (3060 - 940);
  normalizedBrakeValue = constrain(normalizedBrakeValue, 0.0, 1.0);

  if (normalizedThrottleValue > currentSpeed && normalizedBrakeValue == 0.00) {
    currentSpeed = normalizedThrottleValue;
  } else {
    currentSpeed -= decayRate;
  }
  if(normalizedBrakeValue > 0.00 && normalizedBrakeValue < 0.80){
    currentSpeed = currentSpeed - (normalizedBrakeValue * 0.0001);
  }
  if(normalizedBrakeValue > 0.90){
    currentSpeed = 0.00;
  }

  currentSpeed = constrain(currentSpeed, 0.0, 1.0);

  return currentSpeed;
}

void setup() {
  pinMode(CAN0_INT, INPUT);
  Serial.begin(115200);
  can.initialize();
  delay(1000);
}

void loop() {
  can.spin();

if (print_realtime_data == true){
      if (millis() - last_print_data > 200){
        Serial.print(can.erpm); 
        Serial.print(can.inpVoltage);
        Serial.print(can.dutyCycleNow);
        Serial.print(can.avgInputCurrent);
        Serial.print(can.avgMotorCurrent);
        Serial.print(can.tempFET);
        Serial.print(can.tempMotor);

        last_print_data = millis();
        }
    }

  //can.vesc_set_duty(MotorValueForVESC());
  //Serial.println(MotorValueForVESC());
}
