#ifndef GAME_H
#define GAME_H

#include "Util.h"
#include "Math.h"

#include <U8g2lib.h>
#include <ezBuzzer.h>

// Game maps size
#define MAP_WIDTH 124
#define MAP_HEIGHT 62

extern U8G2_SSD1306_128X64_NONAME_2_HW_I2C u8g2;
extern ezBuzzer musicPlayer;
extern bool gSpeakerOn;

// Map (defined as a rectangle)
struct Map
{
    vec2i pos;  
    uint8_t width;
    uint8_t height;
};

// map for snake (same for pong game)
const Map snakeMap = {vec2i{2, 2}, MAP_WIDTH, MAP_HEIGHT};

// current game state (used also for menu)
enum struct GameState
{
    PLAYING     = 0,    // For Menu it means that it's on menu
    PAUSE       = 1,    // For Menu it menas that it's not in menu (so it's playing a game)
    FINISHED    = 2,
    GO_MENU     = 3,
    MATCH_ENDED = 4
};

class Game
{
public:
    virtual ~Game() = default;
    virtual void Update(int input) = 0;

    inline GameState GetState() const { return mState; }

protected:
    GameState mState;
    Game(GameState state) : mState(state) {}
};

inline void DrawPauseScreen()
{
    // Display is divided in two pages (half screen is frist page, other half is second page)
    u8g2.firstPage(); 
    do
    {
        // Set font
        u8g2.setFont(u8g2_font_profont22_tf);
        u8g2.setCursor(5, 15);

        u8g2.print(F("Pause!"));

        // Set font
        u8g2.setFont(u8g2_font_BitTypeWriter_tr);
        u8g2.setCursor(5, 35);

        u8g2.print(F("Press play to resume"));
    } while (u8g2.nextPage());
}

inline void DrawGameOver(uint8_t score)
{
    u8g2.firstPage();
    do
    {
        // Set font
        u8g2.setFont(u8g2_font_profont22_tf);
        u8g2.setCursor(5, 15);

        u8g2.print(F("Game over!"));
        
        // Set font
        u8g2.setFont(u8g2_font_BitTypeWriter_tr);
        u8g2.setCursor(5, 35);

        u8g2.print(F("Score "));
        u8g2.print(score);

        u8g2.setCursor(5, 50);
        u8g2.print(F("'play' -> menu"));
    } while (u8g2.nextPage());
}

inline void DrawYouWin(uint8_t score)
{
    u8g2.firstPage();
    do
    {
        // Set font
        u8g2.setFont(u8g2_font_profont22_tf);
        u8g2.setCursor(5, 15);

        u8g2.print(F("You win!"));

        // Set font
        u8g2.setFont(u8g2_font_BitTypeWriter_tr);
        u8g2.setCursor(5, 35);

        u8g2.print(F("Score "));
        u8g2.print(score);

        u8g2.setCursor(5, 50);
        u8g2.print(F("'play' -> menu"));
    } while (u8g2.nextPage());
}

#endif
