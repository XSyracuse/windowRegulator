#include <CAN.h>
#include <Servo.h>
#include <util/atomic.h>

#include "motor.h"


#define SERVO0 4
#define SERVO1 5
#define SERVO2 9

Servo AuxControl0;
Servo AuxControl1;
Servo AuxControl2;

uint8_t servo_angle[] = {90,90,90};
#define WIND_OFF  0x10
#define WIND_UP   0x11
#define WIND_DOWN 0x02

#define CAN_WIND_OFF  0x00
#define CAN_WIND_UP   0x01
#define CAN_WIND_DOWN 0x02

// can_wind_command and wind_command(switch input)
// both can wind the window
// however switch input has priority

uint8_t  can_wind_command = CAN_WIND_OFF;

uint8_t  wind_command = WIND_OFF;

uint8_t  wind_status = WIND_OFF;
uint16_t wind_count = 0;

uint8_t motor_pwm_value = 0x7f;

// input definitions
#define WIND_UP_DOWN A0
#define SERVO_BUTTON A1

#define MOTOR_CURRENT A5
#define MAX_MOTOR_CURRENT 768

void setup() {
  
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("CAN Winder");

  // CAN Controller Setup
  CAN.setPins(10,2);
    
  if(!CAN.begin(100E3)) {
    Serial.println("CAN Failed");
    while(1);
  }
  
  CAN.onReceive(onReceive);

  // Servo Setup
  
  AuxControl0.attach(SERVO0);
  AuxControl1.attach(SERVO1);
  AuxControl2.attach(SERVO2);
  
  AuxControl0.write(90);
  AuxControl1.write(90);
  AuxControl2.write(90);
  
  motor_init();
  
}

void loop() {
  
  // pulse servo to move to current servo position
  AuxControl0.write(servo_angle[0]);
  AuxControl1.write(servo_angle[1]);
  AuxControl2.write(servo_angle[2]);

  // usually the switch position controls the wind
  // however when the switch (wind_command) is in the off position
  // CAN can control the wind
  
  wind_status = wind_command;

  
  if(wind_command == WIND_OFF) {
    
      ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {  
      switch(can_wind_command) {
          case CAN_WIND_OFF:
              wind_status = WIND_OFF;
              break;
          case CAN_WIND_UP:
              wind_status = WIND_UP;
              break;
          case CAN_WIND_DOWN:
              wind_status = WIND_DOWN;
              break;
          default:
              break;  
    }//switch
    }//Atomic
    
  }


  if(wind_status == WIND_OFF) {
    motor_set_speed(0x00); 
    wind_count = 0;
  }
  if(wind_status == WIND_UP) {
    motor_set_dir(0);
    motor_set_speed(motor_pwm_value);
    wind_count++;
  }
  if(wind_status == WIND_DOWN) {
    motor_set_dir(1);
    motor_set_speed(motor_pwm_value);
    wind_count++;
  }
  
  // about ten times per second
  delay(100);

  // limit wind up or down to 5 seconds
  if(wind_count > 50) {
      wind_status = WIND_OFF;
      can_wind_command = CAN_WIND_OFF;
  }

  // check if motor is stalled
  // and if stalled cut off motor drive
  
  uint16_t motor_current = analogRead(MOTOR_CURRENT);
  
  if(motor_current > MAX_MOTOR_CURRENT) {
    can_wind_command = CAN_WIND_OFF;
    wind_status = WIND_OFF;
  }

  // read switches
  
  uint16_t adc_value = analogRead(WIND_UP_DOWN);

  if(adc_value > 768) {
    wind_command = WIND_OFF;
  }
  if(adc_value < 768 && adc_value>256) {
    wind_command = WIND_UP;
  }
  if(adc_value < 256) {
    wind_command = WIND_DOWN;
  }

  adc_value = analogRead(SERVO_BUTTON);
  
  if(adc_value > 768) {
    // nothing to do...
  }
  if(adc_value < 768 && adc_value>256) {
    servo_angle[0] += 2;
  }
  if(adc_value < 256) {
    
    if(servo_angle[0] > 0){
      servo_angle[0] -= 2;
    }
    
  }
  
  if(servo_angle[0] > 180) {
    servo_angle[0] = 180;
  }
  
}

void onReceive(int packetSize) {

  Serial.print("Rx: ");

  if(CAN.packetRtr()) {
    Serial.print("RTR ");
  }
  
  Serial.print("packet with id 0x");
  Serial.print(CAN.packetId(), HEX);

  if(CAN.packetRtr()) {
    Serial.print(" and requested length ");
    Serial.println(CAN.packetDlc());
  }
  else {
    Serial.print(" and length ");
    Serial.println(packetSize);

    while(CAN.available()) {
      
      char ch = (char)CAN.read();
      
      if(ch==0x00) can_wind_command = CAN_WIND_OFF;
      if(ch==0x01) can_wind_command = CAN_WIND_UP;
      if(ch==0x02) can_wind_command = CAN_WIND_DOWN;
      
      if(ch==0x03) {
         ch = (char)CAN.read();
         servo_angle[0] = ch;
      }
      
      if(ch==0x04) {
         ch = (char)CAN.read();
         servo_angle[1] = ch;  
      }

      if(ch==0x05) {
        ch = (char)CAN.read();
        servo_angle[2] = ch;
      }
      
      if(ch==0x10) {
        ch = (char)CAN.read();
        motor_pwm_value = ch;
      }

      if(ch==0x11) {
        ch = (char)CAN.read();
        motor_set_pwm_clk(ch);
      }
      Serial.print(ch);
    }
    Serial.println();
  }

  Serial.println();
}
