#ifndef MENU_H
#define MENU_H

// See Menu.cpp for functions / method definitions

// Include Game
#include "Game.h"

// Number of games available
#define NUMBER_OF_GAMES 2

// Menu extends Game object
class Menu : public Game
{
public:
    // Menu constructor
    Menu();
    // Update method
    void Update(int input) override;

private:
    // Game titles
    String mTitles[NUMBER_OF_GAMES];
    // Currently selected game on menu (not necessary the one playing)
    uint8_t mSelectedGame;
    // Game instance pointer
    Game *mGame;
    // Draw the menu
    void Draw();
};

#endif
