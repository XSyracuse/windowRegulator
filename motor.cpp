/*
 * brush motor functions
 */
#include "motor.h"
#include<avr/io.h>
#include<avr/interrupt.h>

//uint8_t motor_direction = 0;
//uint8_t motor_speed     = 0;

void motor_init()
// Timer 2 is set to phase correct pwm mode
// TOP = 0xFF
// OCRx updates at TOP
// TOV flag set on BOTTOM
{
  //motor_direction = 0;
  //motor_speed = 0;
  
  //set Timer 0 output compare pin OCRA to off
  TCCR0A &= ~(1<<COM0A1 | 1<<COM0A0);

  //motor_set_dir(0);
  
  motor_set_speed(0x00);
  motor_set_pwm_clk(0);
  
  TCCR2A &= ~(1<<WGM21 | 1<<WGM20);
  TCCR2B &= ~(1<<WGM22);
  TCCR2A |= (1<<WGM20);

  
  MOTOR_DIR_PORT &= ~(1<<MOTOR_DIR_PIN);
  MOTOR_DIR_DDR  |=  (1<<MOTOR_DIR_PIN);
  
  MOTOR_PWM_PORT &= ~(1<<MOTOR_PWM_PIN);
  MOTOR_PWM_DDR  |=  (1<<MOTOR_PWM_PIN);
  
  //digitalWrite(MOTOR_DIR, LOW);
  //digitalWrite(MOTOR_PWM, LOW);
  
  //  After, the port is set up
  //  set the direction
  
  motor_set_dir(0);
  
  //start output compare 2B interrupt
  //the interrupt service routine synchronizes the
  //change of direction with the pwm
  TIMSK2 |= 1 << OCIE2B;
  
}

void motor_set_dir(uint8_t motor_direction)
{  
  if(motor_direction == 0) {
    
    //inverting mode
    TCCR2A |= 1<<COM2B1;
    TCCR2A |= 1<<COM2B0;
    MOTOR_DIR_PORT |= (1<<MOTOR_DIR_PIN);
    
  }
  else {

    TCCR2A |= 1<<COM2B1;
    TCCR2A &= ~(1<<COM2B0);
    MOTOR_DIR_PORT &= ~(1<<MOTOR_DIR_PIN);
    
  }
  
}
void motor_set_speed(uint8_t motor_speed) {
  OCR2B = motor_speed;
}

void motor_set_pwm_clk(uint8_t freq) 

//  when freq = 1 Timer 2 clock = clk/8
//  when freq = 2 Timer 2 clock = clk
//  when other    Timer 2 clock = clk/64

{
  
  TCCR2B &= ~((1 << CS22) | (1 << CS21) | (1 << CS20));
  
  if(freq == 1) 
    TCCR2B |= (1 << CS21);
  else if(freq == 2)
    TCCR2B |= (1 << CS20);  
  else
    TCCR2B |= (1 << CS22);

}

ISR(TIMER2_COMPB_vect) {
  //motor_set_dir();
}
