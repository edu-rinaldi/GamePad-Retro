#include "Pong.h"

/* 
    Paddle object constructor;
    Takes as argument:
        - Paddle position
        - boolean that identify player's paddle and bot paddle
*/
Paddle::Paddle(const vec2i &position, bool isPlayer) : mPosition(position), mSpeed(2), mIsPlayer(isPlayer) {}

// Move the paddle position up or down by a factor mSpeed
void Paddle::Move(bool up)
{
    mPosition = mPosition + vec2i{0, up ? -1 : 1} * mSpeed;
}

void Paddle::Draw()
{
    u8g2.drawBox(mPosition.x, mPosition.y, PADDLE_WIDTH, PADDLE_HEIGHT);
}

/* 
    Ball object constructor;
    Takes as argument ball starting position, and set as initial direction a random one
*/
Ball::Ball(const vec2i &position) : mPosition(position),
                                    mDirection(GetRandomDirection()),
                                    mSpeed(1),
                                    mBounces(0)
{
}

/*
    Move the ball also taking into account collisions;
    Takes as argument:
        - The game map
        - Player paddle object
        - Bot paddle object
    return false if by moving the ball hits on of the two vertical walls otherwise true
*/
bool Ball::Move(const Map &pongMap, const Paddle &playerPaddle, const Paddle &botPaddle)
{
    // Move the ball in mDirection by a factor mSpeed
    mPosition = mPosition + mDirection * mSpeed;

    // Check if the ball hit top or bottom of the map, is so ==> bounce (vertical)
    if (mPosition.y <= pongMap.pos.y || mPosition.y >= pongMap.pos.y + pongMap.height)
        mDirection.y = -mDirection.y;

    // Check if the ball hit one of the two paddles, if so ==> bounce (horizontal)
    else if (CollidePaddle(playerPaddle) || CollidePaddle(botPaddle))
    {
        if(gSpeakerOn) musicPlayer.beep(10);
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

void Ball::Draw()
{
    u8g2.drawCircle(mPosition.x, mPosition.y, BALL_RADIUS, U8G2_DRAW_ALL);
}

/* 
    Check the ball-paddle collision;
    Takes as argument the paddle for which to verify the collision;
    Returns true if there's a collision, false otherwise
*/
bool Ball::CollidePaddle(const Paddle &paddle)
{
    const vec2i &paddlePosition = paddle.GetPosition();
    // Check if ball position is inside paddle area
    return (mPosition.x > paddlePosition.x && mPosition.x < paddlePosition.x + PADDLE_WIDTH) &&
           (mPosition.y > paddlePosition.y && mPosition.y < paddlePosition.y + PADDLE_HEIGHT);
}

// Private method used for generating a random valid direction
vec2i Ball::GetRandomDirection() const
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

/*
    PongGame object constructor;
    Takes as argument the game map;
    Set the starting state as "PLAYING" and it creates Paddles and Ball objects
*/
PongGame::PongGame(const Map &pongMap) : Game(GameState::PLAYING),
                                         mPongMap(pongMap),
                                         mPlayerScore(0),
                                         mBotScore(0),
                                         mPlayer(GetInitialPosition(true), true),
                                         mBot(GetInitialPosition(false), false),
                                         mBall((pongMap.pos + vec2i{pongMap.width, pongMap.height}) / vec2i{2, 2})
{
}

// Update method for PongGame, it works like a state machine
void PongGame::Update(int input)
{
    // Playing state
    if (mState == GameState::PLAYING)
    {
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

        MoveBotPaddle();
        // Move the ball
        if (!mBall.Move(mPongMap, mPlayer, mBot))
            // If ball cant move it means that we have a collision on vertical walls ==> current match ended
            mState = GameState::MATCH_ENDED;
        
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
        if (mPlayerScore == MAX_SCORE_PONG) DrawYouWin(mPlayerScore);
        // Otherwise bot win!
        else DrawGameOver(mPlayerScore);

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

void PongGame::Draw() const
{
    u8g2.firstPage(); 
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
vec2i PongGame::GetInitialPosition(bool isPlayer) const
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

void PongGame::RestartGame()
{
    // Create a new ball object
    mBall = Ball((mPongMap.pos + vec2i{mPongMap.width, mPongMap.height}) / vec2i{2, 2});
    // Create new Paddle objects (this operation place them again in the middle)
    mPlayer = Paddle(GetInitialPosition(true), true);
    mBot = Paddle(GetInitialPosition(false), false);
}

// Mini AI that moves bot paddle according to ball position
// It's not thought to always win
void PongGame::MoveBotPaddle()
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
