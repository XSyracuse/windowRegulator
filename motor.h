//  when MOTOR_DIR is set, MOTOR_PWM low pulse-width time controls motor speed
//  i.e. COM2B0 = 1 (inverting mode)
//  when MOTOR_DIR is clear, MOTOR_PWM high time controls motor speed
//  i.e. COM2B0 = 0 (non-inverting mode)
//  also, COM2B1 = 1
//  disable COM2B1:COM2B0 = 00

//  motor code in ATmega328 specific
//  motor uses Timer 2 for timing


#ifndef _MOTOR_H_
#define _MOTOR_H_

#include <stdint.h>

#define MOTOR_PWM 3 // OC2B, uses waveform generator PD3
#define MOTOR_DIR 6 // set or clear for direction    PD6

#define MOTOR_PWM_DDR  DDRD
#define MOTOR_PWM_PORT PORTD
#define MOTOR_PWM_PIN  3

#define MOTOR_DIR_DDR  DDRD
#define MOTOR_DIR_PORT PORTD
#define MOTOR_DIR_PIN  6

void motor_init();
void motor_set_dir(uint8_t motor_direction);
void motor_set_speed(uint8_t motor_speed);
void motor_set_pwm_clk(uint8_t freq);

#endif
