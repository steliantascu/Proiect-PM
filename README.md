# Desktop Companion — Robot interactiv cu analiza de zgomot si sertar mecanic

**Autor:** Tascu Stelian-Andrei | **Grupa:** 331CA | **PM, UPB 2026**

## Descriere

Robot de birou bazat pe Arduino Uno (ATmega328P) care:
- Detecteaza prezenta utilizatorului prin senzor ultrasonic HC-SR04
- Se trezeste si executa un gest de salut la primul zgomot detectat
- Deschide un sertar secret la 3 batai din palme consecutive
- Afiseaza starea curenta pe un LCD 16x2 I2C

## Conectarea pinilor

| Componenta | Pin componenta | Pin Arduino Uno |
|---|---|---|
| HC-SR04 | VCC | 5V |
| HC-SR04 | GND | GND |
| HC-SR04 | Trig | D8 |
| HC-SR04 | Echo | D2 |
| KY-037 | VCC | 5V |
| KY-037 | GND | GND |
| KY-037 | A0 | A0 |
| Servo brat | semnal | D9 |
| Servo sertar | semnal | D10 |
| Servo ambele | VCC rosu | 5V breadboard |
| Servo ambele | GND maro | GND breadboard |
| LCD I2C | VCC | 5V |
| LCD I2C | GND | GND |
| LCD I2C | SDA | A4 |
| LCD I2C | SCL | A5 |
| Condensator + | — | sina rosie 5V |
| Condensator - | — | sina albastra GND |

## Structura cod

```
desktop_companion/
├── platformio.ini
├── src/
│   ├── main.c        FSM cu 6 stari
│   ├── hardware.c    PWM servo, ADC microfon, INT0 ultrasonic
│   └── lcd_i2c.c     driver TWI manual + LCD HD44780 4-bit
└── include/
    ├── hardware.h
    └── lcd_i2c.h
```

## Upload cu PlatformIO

```bash
pio run --target upload
```

## Calibrare

In `src/main.c` modifica:

```c
#define PRAG_ZGOMOT   600   // creste daca e prea sensibil
#define DISTANTA_MAX   50   // distanta trezire in cm
#define PAROLA_BATAI    3   // numar batai pentru parola
#define FEREASTRA_MS 3000   // timp asteptare parola (ms)
```

## Laboratoare acoperite

| Lab | Ce face in proiect |
|---|---|
| Lab 2 | INT0 pentru masurarea ecoului HC-SR04 |
| Lab 3 | Timer 1 Fast PWM 50Hz pentru servomotoare |
| Lab 4 | ADC canal 0 pentru microfon KY-037 |
| Lab 6 | Driver TWI manual pentru LCD cu PCF8574 |
