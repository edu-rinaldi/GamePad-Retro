#ifndef MENU_H
#define MENU_H

#include "Game.h"

// Number of games available
#define NUMBER_OF_GAMES 2

class Menu : public Game
{
public:
    Menu();
    void Update(int input) override;

private:
    String mTitles[NUMBER_OF_GAMES];
    uint8_t mSelectedGame; // Currently selected game on menu (not necessary the one playing)
    Game *mGame;
    
    void Draw();
};

#endif
