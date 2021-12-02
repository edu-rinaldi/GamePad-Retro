#ifndef UTIL_H
#define UTIL_H

// 1 Byte unsigned integer (we can use this for almost the whole project)
// Done for memory efficiency
using uint8_t = unsigned char;

// -- IR KEYS --
#define UP_KEY 0xFFFF906F
#define DOWN_KEY 0xFFFFE01F
#define POWER_KEY 0xFFFFA25D
#define VOL_UP_KEY 0xFF629D
#define VOL_DOWN_KEY 0xFFA857
#define PLAY_PAUSE_KEY 0x2FD
#define KEY_0 0x6897
#define KEY_1 0x30CF
#define KEY_2 0xFF18E7
#define KEY_3 0x7A85
#define KEY_4 0xFF10EF
#define KEY_5 0x38C7
#define KEY_6 0x5AA5
#define KEY_7 0x42BD
#define KEY_8 0xFF4AB5
#define KEY_9 0x52AD

#define HOLDING 0xFFFFFF

// -- END IR KEYS --

// IR Receiver pin
#define IR_PIN 7

// BUZZER PIN
#define BUZZER_PIN 3

#endif
