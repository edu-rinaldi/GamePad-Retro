#ifndef PONG_H
#define PONG_H

// Include Game header file
#include "Game.h"

// Pong max score for a game
#define MAX_SCORE_PONG (uint8_t)5

// Paddle sizes
#define PADDLE_WIDTH (uint8_t)3
#define PADDLE_HEIGHT (uint8_t)10

// Paddle class
class Paddle
{
public:
    /*
        Paddle object constructor;
        Takes as argument:
            - Paddle position
            - boolean that identify player's paddle and bot paddle
    */
    Paddle(const vec2i &position, bool isPlayer) : mPosition(position), mSpeed(2), mIsPlayer(isPlayer) {}

    // Move the paddle position up or down by a factor mSpeed
    void Move(bool up)
    {
        mPosition = mPosition + vec2i{0, up ? -1 : 1} * mSpeed;
    }

    // Draw the paddle
    void Draw()
    {
        u8g2.drawBox(mPosition.x, mPosition.y, PADDLE_WIDTH, PADDLE_HEIGHT);
    }

    // Tells if it's player's paddle or bot's paddle
    inline bool IsPlayer() const { return mIsPlayer; }
    // Return the paddle current position
    inline vec2i GetPosition() const { return mPosition; }

private:
    // Paddle current position
    vec2i mPosition;
    // Paddle moving speed
    uint8_t mSpeed;
    // Player paddle?
    bool mIsPlayer;
};

// Define the ball radius
#define BALL_RADIUS (uint8_t)2

// Ball class
class Ball
{
public:
    /*
        Ball object constructor;
        Takes as argument ball starting position, and set as initial direction a random one
    */
    Ball(const vec2i &position) : mPosition(position),
                                  mDirection(GetRandomDirection()),
                                  mSpeed(1),
                                  mBounces(0)
    {
    }

    // Returns the ball current position
    inline vec2i GetPosition() const { return mPosition; }
    // Increase ball speed
    inline void IncreaseSpeed() { mSpeed += 1; }

    /*
        Move the ball also taking into account collisions;
        Takes as argument:
            - The game map
            - Player paddle object
            - Bot paddle object
        return false if by moving the ball hits on of the two vertical walls otherwise true
    */
    bool Move(const Map &pongMap, const Paddle &playerPaddle, const Paddle &botPaddle)
    {
        // Move the ball in mDirection by a factor mSpeed
        mPosition = mPosition + mDirection * mSpeed;

        // Check if the ball hit top or bottom of the map, is so ==> bounce (vertical)
        if (mPosition.y <= pongMap.pos.y || mPosition.y >= pongMap.pos.y + pongMap.height)
            mDirection.y = -mDirection.y;

        // Check if the ball hit one of the two paddles, if so ==> bounce (horizontal)
        else if (CollidePaddle(playerPaddle) || CollidePaddle(botPaddle))
        {
            // If speaker is enabled beep
            if (gSpeakerOn)
                musicPlayer.beep(10);
            // Invert direction on x axis
            mDirection.x = -mDirection.x;
            // Every 3 bounces increase speed
            if (++mBounces % 3 == 0)
                this->IncreaseSpeed();
        }

        // Check if ball hit left or right of the map, if so ==> increase point state
        if (mPosition.x <= pongMap.pos.x || mPosition.x >= pongMap.pos.x + pongMap.width)
            return false;

        return true;
    }

    // Draw the ball as a circle
    void Draw()
    {
        u8g2.drawCircle(mPosition.x, mPosition.y, BALL_RADIUS, U8G2_DRAW_ALL);
    }

    /*
        Check the ball-paddle collision;
        Takes as argument the paddle for which to verify the collision;
        Returns true if there's a collision, false otherwise
    */
    bool CollidePaddle(const Paddle &paddle)
    {
        const vec2i &paddlePosition = paddle.GetPosition();
        // Check if ball position is inside paddle area
        return (mPosition.x > paddlePosition.x && mPosition.x < paddlePosition.x + PADDLE_WIDTH) &&
               (mPosition.y > paddlePosition.y && mPosition.y < paddlePosition.y + PADDLE_HEIGHT);
    }

private:
    // Ball position
    vec2i mPosition;
    // Ball direction
    vec2i mDirection;
    // Ball speed
    uint8_t mSpeed;
    // Number of bounces done by ball
    uint8_t mBounces;

    // Private method used for generating a random valid direction
    vec2i GetRandomDirection() const
    {
        vec2i randomDir = {0, 0};
        // If the random value is 0 ==> generate a new random value
        do
        {
            randomDir.x = random(-1, 2);
        } while (randomDir.x == 0);
        // If the random value is 0 ==> generate a new random value
        do
        {
            randomDir.y = random(-1, 2);
        } while (randomDir.y == 0);
        return randomDir;
    }
};

// Pong game class extends Game superclass
class PongGame : public Game
{
public:
    /*
        PongGame object constructor;
        Takes as argument the game map;
        Set the starting state as "PLAYING" and it creates Paddles and Ball objects
    */
    PongGame(const Map &pongMap) : Game(GameState::PLAYING),
                                   mPongMap(pongMap),
                                   mPlayerScore(0),
                                   mBotScore(0),
                                   mPlayer(GetInitialPosition(true), true),
                                   mBot(GetInitialPosition(false), false),
                                   mBall((pongMap.pos + vec2i{pongMap.width, pongMap.height}) / vec2i{2, 2})
    {
    }

    // Update method for PongGame, it works like a state machine
    void Update(int input) override
    {
        // Playing state
        if (mState == GameState::PLAYING)
        {
            // Move paddles based on input key
            switch (input)
            {
            // up key ==> move up
            case UP_KEY:
                mPlayer.Move(true);
                mPreviousMoveUp = true;
                break;
            // down key ==> move down
            case DOWN_KEY:
                mPlayer.Move(false);
                mPreviousMoveUp = false;
                break;
            // Pause key ==> go in pause state and pause the game
            case PLAY_PAUSE_KEY:
                mState = GameState::PAUSE;
                break;
            // Special input for handling "while keypressed" event (IR natively does not support this feature)
            // e.g.: if user is holding up key whe should move up until key is pressed
            case 0xFFFFFF:
                mPlayer.Move(mPreviousMoveUp);
                break;
            // If power key is pressed ==> Go in "Go menu" state and quit the game
            case POWER_KEY:
                // Quit the game
                mState = GameState::GO_MENU;
                break;
            }

            // Move bot paddle
            MoveBotPaddle();
            // Move the ball
            if (!mBall.Move(mPongMap, mPlayer, mBot))
                // If ball cant move it means that we have a collision on vertical walls ==> current match ended
                mState = GameState::MATCH_ENDED;

            // Draw the game scene
            Draw();
        }
        // If the game is on Pause state ==> draw pause page
        else if (mState == GameState::PAUSE)
        {
            DrawPauseScreen();
            // wait to press play to go again in "Playing" state
            if (input == PLAY_PAUSE_KEY)
                mState = GameState::PLAYING;
        }
        // Game finished state ==> draw gameover/winning, and wait to press play to go on menu
        else if (mState == GameState::FINISHED)
        {
            // If player reach max score ==> you win!
            if (mPlayerScore == MAX_SCORE_PONG)
                DrawYouWin(mPlayerScore);
            // Otherwise bot win!
            else
                DrawGameOver(mPlayerScore);

            // wait to press play to go to menu
            if (input == PLAY_PAUSE_KEY)
                mState = GameState::GO_MENU;
        }
        // Match ended
        else if (mState == GameState::MATCH_ENDED)
        {
            // If ball is on left side ==> increase player score
            if (mBall.GetPosition().x < (mPongMap.pos.x + (mPongMap.width / 2)))
                mPlayerScore++;
            else
                mBotScore++;

            // If we win or lose (i.e. score == 10) ==> end game
            if (mPlayerScore == MAX_SCORE_PONG || mBotScore == MAX_SCORE_PONG)
                mState = GameState::FINISHED;
            // play another match by restarting the game
            else
            {
                RestartGame();
                mState = GameState::PLAYING;
            }
        }
    }

private:
    Map mPongMap;         // Map of the game
    Paddle mPlayer;       // Player paddle
    Paddle mBot;          // Bot paddle
    Ball mBall;           // Ball
    bool mPreviousMoveUp; // Previous player move in previous frame
    uint8_t mPlayerScore; // Player score
    uint8_t mBotScore;    // Bot score

    // Draw the game scene
    void Draw() const
    {
        u8g2.firstPage(); // The first page shows
        do
        {
            // Draw the map frame
            u8g2.drawFrame(mPongMap.pos.x, mPongMap.pos.y, mPongMap.width, mPongMap.height);

            // Print player and bot scores
            u8g2.setCursor(110, 13);
            u8g2.print(mPlayerScore);
            u8g2.setCursor(18, 13);
            u8g2.print(mBotScore);

            // Draw ball and paddles
            mBall.Draw();
            mBot.Draw();
            mPlayer.Draw();
        } while (u8g2.nextPage());
    }

    /*
        Private method for calculating paddle initial positions;
        Takes as argument a boolean that tells if it's player's paddle or bot's paddle
    */
    vec2i GetInitialPosition(bool isPlayer) const
    {
        if (isPlayer)
        {
            // Player position is 10 pixels before map's right wall for x axis
            // And in the middle of the screen for y axis
            vec2i playerPosition = (mPongMap.pos + vec2i{mPongMap.width, mPongMap.height});
            playerPosition.y /= 2;
            playerPosition.x -= 10;
            return playerPosition;
        }
        else
        {
            // Analog to player paddle
            vec2i botPosition = (mPongMap.pos + vec2i{0, mPongMap.height});
            botPosition.y /= 2;
            botPosition.x += 10;
            return botPosition;
        }
    }

    // Restart the game match (not the whole game)
    void RestartGame()
    {
        // Create a new ball object
        mBall = Ball((mPongMap.pos + vec2i{mPongMap.width, mPongMap.height}) / vec2i{2, 2});
        // Create new Paddle objects (this operation place them again in the middle)
        mPlayer = Paddle(GetInitialPosition(true), true);
        mBot = Paddle(GetInitialPosition(false), false);
    }

    // Mini AI that moves bot paddle according to ball position
    // It's not thought to always win
    void MoveBotPaddle()
    {
        // Take ball and bot positions (for bot we take the center of the paddle as reference position)
        const vec2i &ballPosition = mBall.GetPosition();
        const vec2i &botPosition = mBot.GetPosition() + vec2i{0, PADDLE_HEIGHT / 2};

        // Bot to ball direction (but taking only y axis)
        short botToBall = ballPosition.y - botPosition.y;
        // If ball above you
        if (botToBall < 0)
            mBot.Move(true); // Move up
        // If ball below you
        else if (botToBall > 0)
            mBot.Move(false); // Move down
        // If it's at the same level dont move
    }
};

#endif
