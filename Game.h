#ifndef GAME_H
#define GAME_H

// Including interal libraries
#include "Util.h"
#include "Math.h"

// Including external library
#include <U8g2lib.h>
#include <ezBuzzer.h>

// Game maps size
#define MAP_WIDTH 124
#define MAP_HEIGHT 62

// Import 3 global vars from "GamePad.ino"
extern U8G2_SSD1306_128X64_NONAME_2_HW_I2C u8g2;
extern ezBuzzer musicPlayer;
extern bool gSpeakerOn;

// Map (defined as a rectangle)
struct Map
{
    vec2i pos;          // Position on the display
    uint8_t width;      // Width of the map
    uint8_t height;     // Height of the map
};

// map for snake (happen to be the same for pong game)
const Map snakeMap = {vec2i{2, 2}, MAP_WIDTH, MAP_HEIGHT};

// Define the current game state (used also for menu)
enum struct GameState
{
    PLAYING     = 0,    // For Menu it means that it's on menu
    PAUSE       = 1,    // For Menu it menas that it's not in menu (so it's playing a game)
    FINISHED    = 2,
    GO_MENU     = 3,
    MATCH_ENDED = 4
};

// Abstract class for a generic game
class Game
{
public:
    // Default destructor
    virtual ~Game() = default;
    // Update method to be defined by subclasses
    virtual void Update(int input) = 0;         
    // Obtain current game state        
    inline GameState GetState() const { return mState; }

protected:
    // Game current state
    GameState mState;
    // Game constructor
    Game(GameState state) : mState(state) {}
};

// Draw pause page on screen
inline void DrawPauseScreen()
{
    // Display is divided in two pages (half screen is frist page, other half is second page)
    u8g2.firstPage(); // The first page shows
    do
    {
        // Set font
        u8g2.setFont(u8g2_font_profont22_tf);
        u8g2.setCursor(5, 15);
        // Draw "pause!" text at (5, 15) coordinates
        u8g2.print(F("Pause!"));
        // Set font
        u8g2.setFont(u8g2_font_BitTypeWriter_tr);
        u8g2.setCursor(5, 35);
        // Draw "Press play to resume" text at (5, 35) coordinates
        u8g2.print(F("Press play to resume"));
    } while (u8g2.nextPage());
}

// Draw the gameover page on screen with the score
inline void DrawGameOver(uint8_t score)
{
    u8g2.firstPage(); // The first page shows
    do
    {
        // Set font
        u8g2.setFont(u8g2_font_profont22_tf);
        u8g2.setCursor(5, 15);
        // Draw "Game over!" text at (5, 15) coordinates
        u8g2.print(F("Game over!"));
        
        // Set font
        u8g2.setFont(u8g2_font_BitTypeWriter_tr);
        u8g2.setCursor(5, 35);
        // Draw the score at (5, 35) coordinates
        u8g2.print(F("Score "));
        u8g2.print(score);

        // Draw "Play-> Menu" text at (5, 50) coordinates
        u8g2.setCursor(5, 50);
        u8g2.print(F("'play' -> menu"));
    } while (u8g2.nextPage());
}

// Draw the "you win" page on screen with score
inline void DrawYouWin(uint8_t score)
{
    u8g2.firstPage(); // The first page shows
    do
    {
        // Set font
        u8g2.setFont(u8g2_font_profont22_tf);
        u8g2.setCursor(5, 15);
        // Draw "You win!" text at (5, 15) coordinates
        u8g2.print(F("You win!"));

        // Set font
        u8g2.setFont(u8g2_font_BitTypeWriter_tr);
        u8g2.setCursor(5, 35);
        // Draw the score at (5, 35) coordinates
        u8g2.print(F("Score "));
        u8g2.print(score);
        // Draw "Play-> Menu" text at (5, 50) coordinates
        u8g2.setCursor(5, 50);
        u8g2.print(F("'play' -> menu"));
    } while (u8g2.nextPage());
}

#endif
