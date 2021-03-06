#include "BLC.h"

//#define SERIAL_DEBUG

#define MAX_RPM 100
#define PWM_PIN_L 7
#define PWM_PIN_R 8
#define PWM_L 800
#define PWM_M 1500
#define PWM_H 2200
#define PWM_DZ 10

BrushlessWheels BW;

float pwm2rpm(int pwm)
{
  if (pwm < PWM_L || pwm > PWM_H)
    return 0.0;

  float rpm;

  if(abs(pwm-PWM_M) < PWM_DZ)
    rpm = 0.0;
  else if(pwm < PWM_M)
    rpm = (float)(pwm-PWM_M)/(float)(PWM_M-PWM_L);
  else
    rpm = (float)(pwm-PWM_M)/(float)(PWM_H-PWM_M);

  return rpm * (float)MAX_RPM;
}

void setup()
{
  pinMode(PWM_PIN_L, INPUT);
  pinMode(PWM_PIN_R, INPUT);

  BW.Init();

#ifdef SERIAL_DEBUG
  Serial.begin(115200);
#endif  
}

void loop()
{
  int pwmL = pulseIn(PWM_PIN_L, HIGH);
  int pwmR = pulseIn(PWM_PIN_R, HIGH);
  
  float rpmL = pwm2rpm(pwmL);
  float rpmR = pwm2rpm(pwmR);

  BW.DriveWheels(rpmL, rpmR);

#ifdef SERIAL_DEBUG
  Serial.print("pwm: ");
  Serial.print(pwmL);
  Serial.print(", ");
  Serial.print(pwmR);
  
  Serial.print("; rpm: ");
  Serial.print(rpmL);
  Serial.print(", ");
  Serial.print(rpmR);
  Serial.println();
#endif

}