#include "Arduino.h"
#include "Menu.h"
#include "Util.h"
#include "Math.h"
#include "Snake.h"
#include "Pong.h"

// Set current state to PLAYING (it means we're currently using menu)
Menu::Menu() : Game(GameState::PLAYING), mSelectedGame(0)
{
    // Available game titles
    mTitles[0] = "Snake";
    mTitles[1] = "Pong";
}

/* 
    This function is called in loop()
    It takes as argument an int input; input is the IR signal received.
    This function, works as a state machine (states define in GameState structure)
*/
void Menu::Update(int input)
{
    if (mState == GameState::PLAYING)
    {       
        switch (input)
        {
        // move up the "<" cursor
        case UP_KEY:
            mSelectedGame = posmod(--mSelectedGame, NUMBER_OF_GAMES);
            if(gSpeakerOn) musicPlayer.beep(10);
            break;
        // analog to UP_KEY case
        case DOWN_KEY:
            mSelectedGame = posmod(++mSelectedGame, NUMBER_OF_GAMES);
            if(gSpeakerOn) musicPlayer.beep(10);
            break;
        // play selected game
        case PLAY_PAUSE_KEY:
            if(gSpeakerOn) musicPlayer.beep(10);
            if (mGame != NULL)
                delete mGame;
            
            if (mSelectedGame == 0) // snake
                mGame = new SnakeGame(snakeMap);
            else if (mSelectedGame == 1)        
                mGame = new PongGame(snakeMap); 
            // Pause menu and start selected game
            mState = GameState::PAUSE;
            break;
        }
        Draw();
    }
    else if (mState == GameState::PAUSE)
    {
        mGame->Update(input);
        if (mGame->GetState() == GameState::GO_MENU)
            mState = GameState::PLAYING;
    }
}

void Menu::Draw()
{
    u8g2.firstPage();
    do
    {
        u8g2.setCursor(20, 13);
        u8g2.print(F("Choose a game:"));
        // Draw the menu
        for (uint8_t i = 0; i < NUMBER_OF_GAMES; ++i)
        {
            u8g2.setCursor(20, 13 * (i + 2));
            // If it's the game pointed by cursor (i.e. mSelectedGame) draw also " <" pointer near game name
            if (i == mSelectedGame)
            {
                u8g2.print(mTitles[i]);
                u8g2.print(F(" <"));
            }
            else u8g2.print(mTitles[i]);
        }
    } while (u8g2.nextPage());
}
