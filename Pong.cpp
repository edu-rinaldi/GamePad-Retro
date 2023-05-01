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

Ball::Ball(const vec2i &position) : mPosition(position),
                                    mDirection(GetRandomDirection()),
                                    mSpeed(1),
                                    mBounces(0)
{
}

/*
    Moves the ball also taking into account collisions;
    Takes as argument:
        - The game map
        - Player paddle object
        - Bot paddle object
    return false if by moving the ball hits on of the two vertical walls otherwise true
*/
bool Ball::Move(const Map &pongMap, const Paddle &playerPaddle, const Paddle &botPaddle)
{
    mPosition = mPosition + mDirection * mSpeed;

    // Check if the ball hit top or bottom of the map, is so ==> bounce (vertical)
    if (mPosition.y <= pongMap.pos.y || mPosition.y >= pongMap.pos.y + pongMap.height)
        mDirection.y = -mDirection.y;

    // Check if the ball hit one of the two paddles, if so ==> bounce (horizontal)
    else if (CollidePaddle(playerPaddle) || CollidePaddle(botPaddle))
    {
        if(gSpeakerOn) musicPlayer.beep(10);
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

// Ball-Paddle collision
bool Ball::CollidePaddle(const Paddle &paddle)
{
    const vec2i &paddlePosition = paddle.GetPosition();
    return (mPosition.x > paddlePosition.x && mPosition.x < paddlePosition.x + PADDLE_WIDTH) &&
           (mPosition.y > paddlePosition.y && mPosition.y < paddlePosition.y + PADDLE_HEIGHT);
}

// Private method used for generating a random valid direction
vec2i Ball::GetRandomDirection() const
{
    vec2i randomDir = {0, 0};
    do
    {
        randomDir.x = random(-1, 2);
    } while (randomDir.x == 0);
    
    do
    {
        randomDir.y = random(-1, 2);
    } while (randomDir.y == 0);
    return randomDir;
}

PongGame::PongGame(const Map &pongMap) : Game(GameState::PLAYING),
                                         mPongMap(pongMap),
                                         mPlayerScore(0),
                                         mBotScore(0),
                                         mPlayer(GetInitialPosition(true), true),
                                         mBot(GetInitialPosition(false), false),
                                         mBall((pongMap.pos + vec2i{pongMap.width, pongMap.height}) / vec2i{2, 2})
{
}

void PongGame::Update(int input)
{
    if (mState == GameState::PLAYING)
    {
        switch (input)
        {
        // move up
        case UP_KEY:
            mPlayer.Move(true);
            mPreviousMoveUp = true;
            break;
        // move down
        case DOWN_KEY:
            mPlayer.Move(false);
            mPreviousMoveUp = false;
            break;
        // pause the game
        case PLAY_PAUSE_KEY:
            mState = GameState::PAUSE;
            break;
        // Special input for handling "while keypressed" event (IR natively does not support this feature)
        // e.g.: if user is holding up key whe should move up until key is pressed
        case 0xFFFFFF:
            mPlayer.Move(mPreviousMoveUp);
            break;
        // quit the game
        case POWER_KEY:
            mState = GameState::GO_MENU;
            break;
        }

        MoveBotPaddle();
        // If ball cant move it means that we have a collision on vertical walls ==> current match ended
        if (!mBall.Move(mPongMap, mPlayer, mBot))
            mState = GameState::MATCH_ENDED;
        
        Draw();
    }
    else if (mState == GameState::PAUSE)
    {
        DrawPauseScreen();
        if (input == PLAY_PAUSE_KEY)
            mState = GameState::PLAYING;
    }
    // draw gameover/winning, and wait to press play to go on menu
    else if (mState == GameState::FINISHED)
    {
        if (mPlayerScore == MAX_SCORE_PONG) DrawYouWin(mPlayerScore);
        else DrawGameOver(mPlayerScore);

        if (input == PLAY_PAUSE_KEY)
            mState = GameState::GO_MENU;
    }
    else if (mState == GameState::MATCH_ENDED)
    {
        // If ball is on left side ==> increase player score
        if (mBall.GetPosition().x < (mPongMap.pos.x + (mPongMap.width / 2)))
            mPlayerScore++;
        else
            mBotScore++;

        // win or lose ==> end game
        if (mPlayerScore == MAX_SCORE_PONG || mBotScore == MAX_SCORE_PONG)
            mState = GameState::FINISHED;
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
    mBall = Ball((mPongMap.pos + vec2i{mPongMap.width, mPongMap.height}) / vec2i{2, 2});

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

    short botToBall = ballPosition.y - botPosition.y;
    // If ball above you
    if (botToBall < 0)
        mBot.Move(true); // Move up
    // If ball below you
    else if (botToBall > 0)
        mBot.Move(false); // Move down
}
