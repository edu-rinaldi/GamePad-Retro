#ifndef PONG_H
#define PONG_H

#include "Game.h"

// Pong max score for a game
#define MAX_SCORE_PONG (uint8_t)5

// Paddle sizes
#define PADDLE_WIDTH (uint8_t)3
#define PADDLE_HEIGHT (uint8_t)10


class Paddle
{
public:
    Paddle(const vec2i &position, bool isPlayer);
    void Move(bool up);
    void Draw();
    inline bool IsPlayer() const { return mIsPlayer; }
    inline vec2i GetPosition() const { return mPosition; }

private:
    vec2i mPosition;
    uint8_t mSpeed;
    bool mIsPlayer;
};

#define BALL_RADIUS (uint8_t)2

class Ball
{
public:
    Ball(const vec2i &position);
    inline vec2i GetPosition() const { return mPosition; }
    inline void IncreaseSpeed() { mSpeed += 1; }
    bool Move(const Map &pongMap, const Paddle &playerPaddle, const Paddle &botPaddle);
    void Draw();
    bool CollidePaddle(const Paddle &paddle);

private:
    vec2i mPosition;
    vec2i mDirection;
    uint8_t mSpeed;
    uint8_t mBounces;
    vec2i GetRandomDirection() const;
};

class PongGame : public Game
{
public:
    PongGame(const Map &pongMap);
    void Update(int input) override;

private:
    Map mPongMap;
    Paddle mPlayer;
    Paddle mBot;
    Ball mBall;
    bool mPreviousMoveUp; // Previous player move in previous frame
    uint8_t mPlayerScore;
    uint8_t mBotScore;

    void Draw() const;
    vec2i GetInitialPosition(bool isPlayer) const;
    void RestartGame();
    void MoveBotPaddle();
};

#endif
