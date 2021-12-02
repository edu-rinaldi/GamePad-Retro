#ifndef MENU_H
#define MENU_H

#include "Game.h"
#include "Music.h"

#define NUMBER_OF_GAMES 2

class Menu : public Game
{
public:
    Menu();
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
