#include <IRremote.h>   // Infrared module library
#include <U8g2lib.h>    // Display library
#include <ezBuzzer.h>   // Buzzer library

#include "Util.h"
#include "Menu.h"

// Initialize display (global variable)
/* 
    u8g2 object configured with 2 pages (for memory efficiency).
    Using "pages" means that the display is divided in two halves:
        - First half is first page
        - Second half is second page
    This allow us to load in memory only half display buffer
*/
U8G2_SSD1306_128X64_NONAME_2_HW_I2C u8g2(U8G2_R0);

// Initialize IR Receiver on pin 7 (global variable)
IRrecv irrecv(IR_PIN);
decode_results results;

ezBuzzer musicPlayer(BUZZER_PIN);

// Boolean used for enabling/disabling buzzer beep
bool gSpeakerOn = true;

Menu menu;

inline void DrawWelcome()
{
    u8g2.firstPage();
    do
    {
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
    Serial.begin(9600);

    irrecv.enableIRIn();

    u8g2.begin();
    DrawWelcome();
    delay(2000);
}

void loop(void)
{
    // Make buzzer wait for beeping (non blocking op.)
    musicPlayer.loop();
    // If receive something via IR ==> update input (i.e. update menu or games or buzzer "volume")
    if (irrecv.decode(&results))
    {
        irrecv.resume();
        // If the user pressed volume up key ==> enable buzzer
        if(results.value == VOL_UP_KEY) gSpeakerOn = true;
        // If the user pressed volume down key ==> disable buzzer
        else if (results.value == VOL_DOWN_KEY) gSpeakerOn = false;
        // Otherwise it's an input for menu/games
        else menu.Update(results.value);
    }
    // If not receiving anything ==> send invalid input
    else menu.Update(-2);
}
