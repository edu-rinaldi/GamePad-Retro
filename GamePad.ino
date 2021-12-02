#include <IRremote.h>
#include <U8g2lib.h>
#include <ezBuzzer.h>

#include "Util.h"
#include "Menu.h"
#include "Snake.h"

// Initialize display (global variable)
// u8g2 object configured with 2 pages (for memory efficiency)
U8G2_SSD1306_128X64_NONAME_2_HW_I2C u8g2(U8G2_R0);

// Initialize IR Receiver on pin 7 (global variable)
IRrecv irrecv(IR_PIN);
decode_results results;     // Variable that will contain IR received value

// Buzzer (global variable)
ezBuzzer musicPlayer(BUZZER_PIN);

// Initialize Menu object (global variable)
Menu menu;

// Draw welcome page
inline void DrawWelcome()
{
    u8g2.firstPage(); // The first page shows
    do
    {
        // Print on screen welcome page
        u8g2.setFont(u8g2_font_profont22_tf);
        u8g2.setCursor(5, 20);
        u8g2.print(F("GamePad"));
        u8g2.setFont(u8g2_font_BitTypeWriter_tr);
        u8g2.setCursor(38, 55);
        u8g2.print(F("E. Rinaldi"));
    } while (u8g2.nextPage());
}

void setup(void)
{
    // Initialize Serial
    Serial.begin(9600);
    // Enabling IR receiving
    irrecv.enableIRIn();
    // Enabling u8g2 library for display
    u8g2.begin();
    // Draw welcome page and wait 2000ms ( 2s )
    DrawWelcome();
    delay(2000);
}

void loop(void)
{
    // Make buzzer wait for beeping (non blocking op.)
    musicPlayer.loop();
    // If receive something via IR ==> update input (i.e. update menu or games)
    if (irrecv.decode(&results))
    {
        irrecv.resume();
        menu.Update(results.value);
    }
    // If not receiving anything ==> send invalid input
    else menu.Update(-2);
}
