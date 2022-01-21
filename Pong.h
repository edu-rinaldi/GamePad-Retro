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
    // Paddle current position
    vec2i mPosition;
    // Paddle moving speed
    uint8_t mSpeed;
    // Player paddle?
    bool mIsPlayer;
};

// Define the ball radius
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
    // Ball position
    vec2i mPosition;
    // Ball direction
    vec2i mDirection;
    // Ball speed
    uint8_t mSpeed;
    // Number of bounces done by ball
    uint8_t mBounces;
    // Generate a random valid direction
    vec2i GetRandomDirection() const;
};

// Pong game class extends Game superclass
class PongGame : public Game
{
public:
    PongGame(const Map &pongMap);
    void Update(int input) override;

private:
    Map mPongMap;           // Map of the game
    Paddle mPlayer;         // Player paddle
    Paddle mBot;            // Bot paddle
    Ball mBall;             // Ball
    bool mPreviousMoveUp;   // Previous player move in previous frame
    uint8_t mPlayerScore;   // Player score
    uint8_t mBotScore;      // Bot score

    void Draw() const;
    vec2i GetInitialPosition(bool isPlayer) const;
    void RestartGame();
    void MoveBotPaddle();
};

#endif
