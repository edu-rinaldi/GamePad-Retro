#include "Arduino.h"
#include "Menu.h"
#include "Util.h"
#include "Math.h"
#include "Snake.h"
#include "Pong.h"

// Menu constructor
Menu::Menu() : Game(0), mSelectedGame(0)
{
    // Available game titles
    mTitles[0] = "Snake";
    mTitles[1] = "Pong";
}

void Menu::Update(int input)
{
    // We're on menu
    if (mState == 0)
    {
        
        switch (input)
        {
        case UP_KEY:
            mSelectedGame = posmod(--mSelectedGame, NUMBER_OF_GAMES);
            musicPlayer.beep(10);
            break;
        case DOWN_KEY:
            mSelectedGame = posmod(++mSelectedGame, NUMBER_OF_GAMES);
            musicPlayer.beep(10);
            break;
        case PLAY_PAUSE_KEY:
            musicPlayer.beep(10);
            if (mGame != NULL)
                delete mGame;
            if (mSelectedGame == 0) // snake
                mGame = new SnakeGame(snakeMap);
            else if (mSelectedGame == 1)        // pong
                mGame = new PongGame(snakeMap); // TODO: Change in pong
            mState = 1;
            break;
        }
        Draw();
    }
    // We're in game
    else if (mState == 1)
    {
        mGame->Update(input);
        // We exit from game
        if (mGame->GetState() == 3)
            mState = 0;
    }
}

void Menu::Draw()
{
    u8g2.firstPage(); // The first page shows
    do
    {
        u8g2.setCursor(20, 13);
        u8g2.print(F("Choose a game:"));
        for (uint8_t i = 0; i < NUMBER_OF_GAMES; ++i)
        {
            u8g2.setCursor(20, 13 * (i + 2));
            if (i == mSelectedGame)
            {
                u8g2.print(mTitles[i]);
                u8g2.print(F(" <"));
            }
            else
                u8g2.print(mTitles[i]);
        }
    } while (u8g2.nextPage());
}
