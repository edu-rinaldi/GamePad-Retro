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
    // Constructor
    Paddle(const vec2i &position, bool isPlayer);
    // Move the paddle up or down
    void Move(bool up);
    // Draw the paddle
    void Draw();
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
    // Constructor
    Ball(const vec2i &position);
    // Returns the ball current position
    inline vec2i GetPosition() const { return mPosition; }
    // Increase ball speed
    inline void IncreaseSpeed() { mSpeed += 1; }
    // Move the ball and check for collision
    bool Move(const Map &pongMap, const Paddle &playerPaddle, const Paddle &botPaddle);
    // Draw the ball
    void Draw();
    // Check if ball collide with a given paddle
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
    // Constructor
    PongGame(const Map &pongMap);
    // Update the game current frame and states
    void Update(int input) override;

private:
    Map mPongMap;           // Map of the game
    Paddle mPlayer;         // Player paddle
    Paddle mBot;            // Bot paddle
    Ball mBall;             // Ball
    bool mPreviousMoveUp;   // Previous player move in previous frame
    uint8_t mPlayerScore;   // Player score
    uint8_t mBotScore;      // Bot score

    // Draw the game scene
    void Draw() const;
    // Get initial player/bot position
    vec2i GetInitialPosition(bool isPlayer) const;
    // Restart the game match (not the whole game)
    void RestartGame();
    // Mini AI for moving Bot paddle
    void MoveBotPaddle();
};

#endif
