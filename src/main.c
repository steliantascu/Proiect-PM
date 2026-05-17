#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include "hardware.h"
#include "lcd_i2c.h"

// =====================================================
// CALIBRARE - Modifica dupa testare
//
// PRAG_ZGOMOT: ADC 0-1023. In liniste ~510.
//   Bataie din palme la 50cm => ~700-900.
//   Incepe cu 600, urca daca e prea sensibil.
//
// DISTANTA_MAX: cat de aproape trebuie sa fii (cm)
//   ca robotul sa se trezeasca.
//
// PAROLA_BATAI: cate batai = parola corecta
// FEREASTRA_MS: cat timp asteapta parola (ms)
// DEBOUNCE_MS:  ignorare dupa fiecare bataie (ms)
// =====================================================
#define PRAG_ZGOMOT   600
#define DISTANTA_MAX   50
#define PAROLA_BATAI    3
#define FEREASTRA_MS 3000
#define DEBOUNCE_MS   200

typedef enum {
    SLEEP,
    IDLE_AWAKE,
    GREETING,
    LISTENING_FOR_PASSWORD,
    DRAWER_OPEN,
    DRAWER_CLOSING
} State;

int main(void) {
    init_hardware();
    lcd_init();

    State    state      = SLEEP;
    uint8_t  clap_count = 0;
    uint16_t distanta   = 0;
    uint16_t nivel      = 0;

    // Mesaj de pornire
    lcd_clear();
    lcd_set_cursor(0, 1);
    lcd_print("Desktop Comp.");
    lcd_set_cursor(1, 3);
    lcd_print("Loading...");
    _delay_ms(1500);

    while (1) {

        distanta = citeste_distanta();

        switch (state) {

            // ------------------------------------------
            // SLEEP: nimeni detectat, sistem in repaus
            // Trezire cand cineva se apropie la <50cm
            // ------------------------------------------
            case SLEEP:
                lcd_set_cursor(0, 0);
                lcd_print("  ( -__- )      ");
                lcd_set_cursor(1, 0);
                lcd_print("    ZzZz        ");

                if (distanta > 0 && distanta < DISTANTA_MAX) {
                    state = IDLE_AWAKE;
                    lcd_clear();
                }
                _delay_ms(300);
                break;

            // ------------------------------------------
            // IDLE_AWAKE: prezenta detectata
            // Asculta microfon. Un varf => GREETING
            // Daca pleci => SLEEP
            // ------------------------------------------
            case IDLE_AWAKE:
                lcd_set_cursor(0, 0);
                lcd_print("  ( ^__^ )      ");
                lcd_set_cursor(1, 0);
                lcd_print("Sunt treaz!     ");

                if (distanta == 0 || distanta > DISTANTA_MAX) {
                    state = SLEEP;
                    lcd_clear();
                    break;
                }

                nivel = citeste_microfon();
                if (nivel > PRAG_ZGOMOT) {
                    state = GREETING;
                }
                break;

            // ------------------------------------------
            // GREETING: salut cu bratul servo
            // 0 -> 90 -> 0 grade, apoi asteapta parola
            // ------------------------------------------
            case GREETING:
                lcd_clear();
                lcd_set_cursor(0, 0);
                lcd_print("  Salut, Sefu!  ");

                set_servo_brat(90);
                _delay_ms(600);
                set_servo_brat(0);
                _delay_ms(400);

                clap_count = 0;
                state = LISTENING_FOR_PASSWORD;

                lcd_clear();
                lcd_set_cursor(0, 0);
                lcd_print("Bat din palme:");
                lcd_set_cursor(1, 0);
                lcd_print("_ _ _           ");
                _delay_ms(500);
                break;

            // ------------------------------------------
            // LISTENING_FOR_PASSWORD: fereastra 3 secunde
            // Numara varfuri audio. 3 batai => DRAWER_OPEN
            // Timp expirat sau < 3 batai => IDLE_AWAKE
            // ------------------------------------------
            case LISTENING_FOR_PASSWORD: {
                uint16_t timp_ms = 0;
                clap_count = 0;

                while (timp_ms < FEREASTRA_MS) {
                    nivel = citeste_microfon();

                    if (nivel > PRAG_ZGOMOT) {
                        clap_count++;

                        lcd_set_cursor(1, 0);
                        if (clap_count == 1) lcd_print("*               ");
                        if (clap_count == 2) lcd_print("* *             ");
                        if (clap_count >= 3) lcd_print("* * *           ");

                        _delay_ms(DEBOUNCE_MS);
                        timp_ms += DEBOUNCE_MS;

                        if (clap_count >= PAROLA_BATAI) break;
                    }

                    _delay_ms(10);
                    timp_ms += 10;
                }

                if (clap_count >= PAROLA_BATAI) {
                    state = DRAWER_OPEN;
                } else {
                    state = IDLE_AWAKE;
                    lcd_clear();
                }
                break;
            }

            // ------------------------------------------
            // DRAWER_OPEN: deschide sertarul
            // Servo sertar la 90 grade, numara invers 5s
            // ------------------------------------------
            case DRAWER_OPEN:
                lcd_clear();
                lcd_set_cursor(0, 0);
                lcd_print("Sertar deschis!");

                set_servo_sertar(90);

                for (int i = 5; i > 0; i--) {
                    lcd_set_cursor(1, 0);
                    lcd_print("Inchidere in: ");
                    lcd_send_data('0' + i);
                    _delay_ms(1000);
                }

                state = DRAWER_CLOSING;
                break;

            // ------------------------------------------
            // DRAWER_CLOSING: inchide sertarul
            // Servo sertar la 0 grade => IDLE_AWAKE
            // ------------------------------------------
            case DRAWER_CLOSING:
                lcd_clear();
                lcd_set_cursor(0, 0);
                lcd_print("Se inchide...   ");

                set_servo_sertar(0);
                _delay_ms(800);

                state = IDLE_AWAKE;
                lcd_clear();
                break;
        }
    }

    return 0;
}
