#include <CAN.h>

#define WIND_OFF  0x00
#define WIND_UP   0x01
#define WIND_DOWN 0x02

void setup() {
  // put your setup code here, to run once:
  
  Serial.begin(115200);
  Serial.println("CAN Controller Program");

  CAN.setPins(10,2);
  
  if(!CAN.begin(100E3)) {
    Serial.println("CAN Failed");
    while(1);
  }
  
}

void send_short_can_msg(uint8_t id, uint8_t data) {

  CAN.beginPacket(id);
  CAN.write(data);
  CAN.endPacket();
  
}

void send_can_msg_2(uint8_t id, uint8_t data0, uint8_t data1) {
  
  CAN.beginPacket(id);
  CAN.write(data0);
  CAN.write(data1);
  CAN.endPacket();
  
}

void loop() {
  
  // put your main code here, to run repeatedly:
  int input = 0;
  char* serial_msg;
  uint8_t id = 0x12;
  
  if(Serial.available() > 0) {
    
      input = Serial.read();
      
  }
  if(input) {
    
    switch(input) {
      case 'u': 
        serial_msg = "Wind Up";
        send_short_can_msg(id,WIND_UP);
        break;
      case 'd':
        serial_msg = "Wind Down";
        send_short_can_msg(id,WIND_DOWN);
        break;
      case 'o':
        serial_msg = "Wind Off";
        send_short_can_msg(id,WIND_OFF);
        break;
      case 'q':
        serial_msg = "Servo[2] : 0";
        send_can_msg_2(id,0x05,0);
        break;
      case 'w':
        serial_msg = "Servo[2] : 90";
        send_can_msg_2(id,0x05,90);
        break;
      case 'e':
        serial_msg = "Servo[2] : 180";
        send_can_msg_2(id,0x05,180);
        break;
      case 'j':
        serial_msg = "Servo[1] : 0";
        send_can_msg_2(id,0x04,0);
        break;
      case 'k':
        serial_msg = "Servo[1] : 90";
        send_can_msg_2(id,0x04,90);
        break;
      case 'l':
        serial_msg = "Servo[1] : 180";
        send_can_msg_2(id,0x04,180);
        break;
      case 'z':
        serial_msg = "Servo[0] : 0";
        send_can_msg_2(id,0x03,0);
        break;
      case 'x':
        serial_msg = "Servo[0] : 90";
        send_can_msg_2(id,0x03,90);
        break;
      case 'c':
        serial_msg = "Servo[0] : 180";
        send_can_msg_2(id,0x03,180);
        break; 
      case 'B':
        serial_msg = "Set PWM : 192";
        send_can_msg_2(id,0x10,192);
        break;
      case 'b':
        serial_msg = "Set PWM : 64";
        send_can_msg_2(id,0x10,64);
        break;
      case 'n':
        serial_msg = "Set PWM : 128";
        send_can_msg_2(id,0x10,128);
        break;
      default:
        serial_msg = "Unknown Func";
        break; 
    }//switch

    Serial.print("Sent : ");
    Serial.println(serial_msg);
    
    
  }
  /*
  Serial.println("Sending CAN packet ...");
  
  CAN.beginPacket(0x12);
  CAN.write('c');
  CAN.write('a');
  CAN.write('n');
  CAN.write(' ');
  CAN.write('d');
  CAN.write('a');
  CAN.write('t');
  CAN.write('a');
  CAN.endPacket();
  
  delay(12);
  */
}
