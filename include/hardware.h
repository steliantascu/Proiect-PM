#ifndef HARDWARE_H
#define HARDWARE_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// =====================================================
// Mapare pini Arduino Uno -> registri ATmega328P
//
// Arduino D8  = PB0 = Trig HC-SR04
// Arduino D2  = PD2 = Echo HC-SR04 (INT0)
// Arduino D9  = PB1 = OC1A = Servo brat
// Arduino D10 = PB2 = OC1B = Servo sertar
// Arduino A0  = PC0 = ADC0 = Microfon KY-037
// Arduino A4  = PC4 = SDA  = LCD I2C
// Arduino A5  = PC5 = SCL  = LCD I2C
// =====================================================

#define TRIG_PIN  PB0   // Arduino D8
#define ECHO_PIN  PD2   // Arduino D2 - INT0

void init_hardware(void);
void set_servo_brat(uint8_t unghi);
void set_servo_sertar(uint8_t unghi);
uint16_t citeste_microfon(void);
uint16_t citeste_distanta(void);

extern volatile uint16_t distanta_cm;
extern volatile uint8_t  echo_flag;

#endif
