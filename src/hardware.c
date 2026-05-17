#include "hardware.h"

volatile uint16_t distanta_cm = 0;
volatile uint8_t  echo_flag   = 0;
static volatile uint16_t timer_val = 0;

void init_hardware(void) {

    // =====================================================
    // LAB 3: PWM hardware Timer 1 pentru servomotoare
    // D9  = PB1 = OC1A = servo brat
    // D10 = PB2 = OC1B = servo sertar
    // Fast PWM, TOP=ICR1, prescaler 8
    // ICR1 = 16000000 / (8 * 50) - 1 = 39999 => 50Hz (20ms)
    // =====================================================
    DDRB |= (1 << PB1) | (1 << PB2);

    TCCR1A = (1 << COM1A1) | (1 << COM1B1) | (1 << WGM11);
    TCCR1B = (1 << WGM13)  | (1 << WGM12)  | (1 << CS11);
    ICR1   = 39999;

    OCR1A = 2000; // 0 grade initial
    OCR1B = 2000;

    // =====================================================
    // LAB 4: ADC pentru microfon pe A0 (PC0 = ADC0)
    // Referinta AVCC (5V), canal 0, prescaler 128
    // 16MHz / 128 = 125kHz (in intervalul recomandat)
    // =====================================================
    ADMUX  = (1 << REFS0);
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

    // =====================================================
    // LAB 2: Intrerupere externa INT0 pentru echo HC-SR04
    // D8 = PB0 = Trig (output)
    // D2 = PD2 = Echo (input, INT0)
    // Declanseaza la orice schimbare logica
    // =====================================================
    DDRB |=  (1 << TRIG_PIN);
    DDRD &= ~(1 << ECHO_PIN);

    EICRA |= (1 << ISC00);   // Any logical change pe INT0
    EIMSK |= (1 << INT0);

    // Timer0 pentru masurarea duratei ecoului
    // Prescaler 1024: 1 tick = 64us la 16MHz
    TCCR0B = (1 << CS02) | (1 << CS00);

    sei();
}

// unghi 0-180 grade => OCR 2000-4000 ticks (1ms-2ms)
void set_servo_brat(uint8_t unghi) {
    OCR1A = 2000 + ((uint32_t)unghi * 2000) / 180;
}

void set_servo_sertar(uint8_t unghi) {
    OCR1B = 2000 + ((uint32_t)unghi * 2000) / 180;
}

uint16_t citeste_microfon(void) {
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));
    return ADC;
}

uint16_t citeste_distanta(void) {
    echo_flag = 0;

    PORTB |=  (1 << TRIG_PIN);
    _delay_us(10);
    PORTB &= ~(1 << TRIG_PIN);

    uint16_t timeout = 0;
    while (echo_flag == 0) {
        _delay_us(100);
        if (++timeout > 300) return 0;
    }
    return distanta_cm;
}

// ISR ultrasonic: masoara durata pulsului Echo
ISR(INT0_vect) {
    if (PIND & (1 << ECHO_PIN)) {
        TCNT0 = 0;
    } else {
        timer_val = TCNT0;
        // 1 tick = 64us, viteza sunet = 343m/s
        // distanta_cm = (ticks * 64us * 34300cm/s) / (2 * 1000000)
        //             = (ticks * 109) / 100
        distanta_cm = (timer_val * 109) / 100;
        echo_flag = 1;
    }
}
