#ifndef GAME_H
#define GAME_H

#include "Util.h"
#include "Math.h"
#include "Music.h"
#include <U8g2lib.h>

// Game maps size
#define MAP_WIDTH 124
#define MAP_HEIGHT 62

// Import 2 global vars from "GamePad.ino"
extern U8G2_SSD1306_128X64_NONAME_2_HW_I2C u8g2;
extern ezBuzzer musicPlayer;

// Map (defined as a rectangle)
struct Map
{
    vec2i pos;          // Position on the display
    uint8_t width;      // Width of the map
    uint8_t height;     // Height of the map
};

// map for snake (happen to be the same for pong game)
const Map snakeMap = {vec2i{2, 2}, MAP_WIDTH, MAP_HEIGHT};

// TODO: test a casa
// struct enum GameState
// {
//     PLAYING = 0,
//     PAUSE,
//     FINISHED,
//     GO_MENU,
//     MATCH_ENDED
// };

// Abstract class for a generic game
class Game
{
public:
    // Default destructor
    virtual ~Game() = default;
    // Update method to be defined by subclasses
    virtual void Update(int input) = 0;         
    // Obtain current game state        
    inline uint8_t GetState() const { return mState; }

protected:
    uint8_t mState;
    Game(uint8_t state) : mState(state) {}
};

// Draw pause page on screen
inline void DrawPauseScreen()
{
    u8g2.firstPage(); // The first page shows
    do
    {
        u8g2.setFont(u8g2_font_profont22_tf);
        u8g2.setCursor(5, 15);
        u8g2.print(F("Pause!"));
        u8g2.setFont(u8g2_font_BitTypeWriter_tr);
        u8g2.setCursor(5, 35);
        u8g2.print(F("Press play to resume"));
    } while (u8g2.nextPage());
}

// Draw the gameover page on screen with the score
inline void DrawGameOver(uint8_t score)
{
    u8g2.firstPage(); // The first page shows
    do
    {
        u8g2.setFont(u8g2_font_profont22_tf);
        u8g2.setCursor(5, 15);
        u8g2.print(F("Game over!"));
        u8g2.setFont(u8g2_font_BitTypeWriter_tr);
        u8g2.setCursor(5, 35);
        u8g2.print(F("Score "));
        u8g2.print(score);
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
        u8g2.setFont(u8g2_font_profont22_tf);
        u8g2.setCursor(5, 15);
        u8g2.print(F("You win!"));
        u8g2.setFont(u8g2_font_BitTypeWriter_tr);
        u8g2.setCursor(5, 35);
        u8g2.print(F("Score "));
        u8g2.print(score);
        u8g2.setCursor(5, 50);
        u8g2.print(F("'play' -> menu"));
    } while (u8g2.nextPage());
}

#endif
