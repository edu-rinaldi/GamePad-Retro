#include "Arduino.h"
#include "Menu.h"
#include "Util.h"
#include "Math.h"
#include "Snake.h"
#include "Pong.h"

// Menu class object constructor
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
    // If we are in "PLAYING" state, it means the user is using the menu
    if (mState == GameState::PLAYING)
    {
        
        switch (input)
        {
        // If we receive UP_KEY ==> We should move up the "<" cursor in menu
        case UP_KEY:
            mSelectedGame = posmod(--mSelectedGame, NUMBER_OF_GAMES);
            if(gSpeakerOn) musicPlayer.beep(10);
            break;
        // If we receive DOWN_KEY ==> analog to UP_KEY case
        case DOWN_KEY:
            mSelectedGame = posmod(++mSelectedGame, NUMBER_OF_GAMES);
            if(gSpeakerOn) musicPlayer.beep(10);
            break;
        // If we receive PLAY key ==> play the currently selected game
        case PLAY_PAUSE_KEY:
            if(gSpeakerOn) musicPlayer.beep(10);
            // If mGame pointer not point to NULL it means that it still points to the previous game
            if (mGame != NULL)
                delete mGame;
            // If selected game is 0 ==> create SnakeGame object
            if (mSelectedGame == 0) // snake
                mGame = new SnakeGame(snakeMap);
            // If selected game is 1 ==> create PongGame object
            else if (mSelectedGame == 1)        
                mGame = new PongGame(snakeMap); 
            // If play button is pressed in menu page ==> Pause menu and start selected game
            mState = GameState::PAUSE;
            break;
        }
        // Draw the menu
        Draw();
    }
    // If we're in a game
    else if (mState == GameState::PAUSE)
    {
        // Call update function for selected game
        mGame->Update(input);
        // If the game is in "go menu" state it means that we should set Menu in "Playing state"
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
