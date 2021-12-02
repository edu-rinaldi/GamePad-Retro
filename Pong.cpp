#include "Pong.h"

Paddle::Paddle(const vec2i &position, bool isPlayer) : mPosition(position), mSpeed(2), mIsPlayer(isPlayer) {}

void Paddle::Move(bool up)
{
    mPosition = mPosition + vec2i{0, up ? -1 : 1} * mSpeed;
}

void Paddle::Draw()
{
    u8g2.drawBox(mPosition.x, mPosition.y, PADDLE_WIDTH, PADDLE_HEIGHT);
}

// Ball
Ball::Ball(const vec2i &position) : mPosition(position),
                                    mDirection(GetRandomDirection()),
                                    mSpeed(1),
                                    mBounces(0)
{
}

bool Ball::Move(const Map &pongMap, const Paddle &playerPaddle, const Paddle &botPaddle)
{
    mPosition = mPosition + mDirection * mSpeed;
    // Check if the ball hit top or bottom of the map, is so ==> bounce (vertical)
    if (mPosition.y <= pongMap.pos.y || mPosition.y >= pongMap.pos.y + pongMap.height)
    {
        mDirection.y = -mDirection.y;
    }
    // Check if the ball hit one of the two paddles, is so ==> bounce (horizontal)
    else if (CollidePaddle(playerPaddle) || CollidePaddle(botPaddle))
    {
        musicPlayer.beep(10);
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

bool Ball::CollidePaddle(const Paddle &paddle)
{
    const vec2i &paddlePosition = paddle.GetPosition();
    return (mPosition.x > paddlePosition.x && mPosition.x < paddlePosition.x + PADDLE_WIDTH) &&
           (mPosition.y > paddlePosition.y && mPosition.y < paddlePosition.y + PADDLE_HEIGHT);
}

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

PongGame::PongGame(const Map &pongMap) : Game(0),
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
    // Playing state
    if (mState == 0)
    {
        // Move paddles based on input key
        switch (input)
        {
        case UP_KEY:
            mPlayer.Move(true);
            mPreviousMoveUp = true;
            break;
        case DOWN_KEY:
            mPlayer.Move(false);
            mPreviousMoveUp = false;
            break;
        case PLAY_PAUSE_KEY:
            mState = 1;
            break;
        case 0xFFFFFF:
            mPlayer.Move(mPreviousMoveUp);
            break;
        case POWER_KEY:
            // Quit the game
            mState = 3;
            break;
        }

        // Move bot paddle
        MoveBotPaddle();
        // Move the ball
        if (!mBall.Move(mPongMap, mPlayer, mBot))
            // If ball cant move it means that we have a collision on vertical walls ==> increase score state
            mState = 4;

        Draw();
    }
    // If the game is on Pause state ==> draw pause page
    else if (mState == 1)
    {
        DrawPauseScreen();
        // wait to press play to go again in "Playing" state
        if (input == PLAY_PAUSE_KEY)
            mState = 0;
    }
    // Game finished state ==> draw gameover/winning, and wait to press play to go on menu
    else if (mState == 2)
    {
        if (mPlayerScore == MAX_SCORE_PONG)
            DrawYouWin(mPlayerScore);
        else
            DrawGameOver(mPlayerScore);
        if (input == PLAY_PAUSE_KEY)
            mState = 3;
    }
    // Match ended
    else if (mState == 4)
    {
        // If ball is on left side ==> increase player score
        if (mBall.GetPosition().x < (mPongMap.pos.x + (mPongMap.width / 2)))
            mPlayerScore++;
        else
            mBotScore++;

        // If we win or lose (i.e. score == 10) ==> end game
        if (mPlayerScore == MAX_SCORE_PONG || mBotScore == MAX_SCORE_PONG)
            mState = 2;
        // play another match by restarting the game
        else
        {
            RestartGame();
            mState = 0;
        }
    }
}

void PongGame::Draw() const
{
    u8g2.firstPage(); // The first page shows
    do
    {
        u8g2.drawFrame(mPongMap.pos.x, mPongMap.pos.y, mPongMap.width, mPongMap.height);
        u8g2.setCursor(110, 13);
        u8g2.print(mPlayerScore);
        u8g2.setCursor(18, 13);
        u8g2.print(mBotScore);
        mBall.Draw();
        mBot.Draw();
        mPlayer.Draw();
    } while (u8g2.nextPage());
}

vec2i PongGame::GetInitialPosition(bool isPlayer) const
{
    if (isPlayer)
    {
        vec2i playerPosition = (mPongMap.pos + vec2i{mPongMap.width, mPongMap.height});
        playerPosition.y /= 2;
        playerPosition.x -= 10;
        return playerPosition;
    }
    else
    {
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

void PongGame::MoveBotPaddle()
{
    const vec2i &ballPosition = mBall.GetPosition();
    const vec2i &botPosition = mBot.GetPosition() + vec2i{0, PADDLE_HEIGHT / 2};
    short botToBall = ballPosition.y - botPosition.y;
    // If ball above you
    if (botToBall < 0)
        mBot.Move(true); // Move up
    // If ball below you
    else if (botToBall > 0)
        mBot.Move(false); // Move down
                          // If it's at the same level dont move
}
