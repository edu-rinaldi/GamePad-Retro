#ifndef SNAKE_H
#define SNAKE_H

// Include external library
#include "ListLib.h"

// Inlcude internal libraries
#include "Math.h"
#include "Util.h"
#include "Game.h"


// Define next move type
enum struct MoveType
{
    E,  // E (stands for empty): if the next move is in an empty valid cell
    A,  // A (stands for apple): if the next move is eating an apple
    B   // B (stands for body collision): if the next move implies colliding with itself or a wall 
};

// Apple class
class Apple
{
public:
    // Static method for creating new apples
    static Apple Spawn(const Map &snakeMap);
    static Apple Spawn(const vec2i &position);
    
    // Returns apple position
    inline vec2i GetPosition() const { return mPosition; }
    // Draw the apple
    inline void Draw() const { u8g2.drawBox(mPosition.x, mPosition.y, SIZE, SIZE); }
    // Check if there's a collision between apple and a point at position "position"
    inline bool Collision(const vec2i &position) const { return inside(position, mPosition, SIZE, SIZE); }

private:
    // Apple constructor
    Apple(const vec2i &position) : mPosition(position) {}
    // Apple position
    vec2i mPosition;
    // Apple size
    static const uint8_t SIZE = 5;
};

// Snake class
class Snake
{
public:
    // Snake constructor
    Snake(const vec2i &startPosition, const vec2i &startDirection);
    
    // Return the snake head position
    inline vec2i GetHeadPosition() const { return mPositions.First(); }

    // Returns the next head position of snake body for the next move
    inline vec2i GetNextPosition(const Map &snakeMap) const
    {
        const vec2i &newPos = mPositions.First() + mDirection * mSpeed;
        // Following two lines to enable if you want snake to go from left side to right side (modulo func.)
        // newPos.x = posmod(newPos.x - snakeMap.pos.x, snakeMap.width) + snakeMap.pos.x;
        // newPos.y = posmod(newPos.y - snakeMap.pos.y, snakeMap.height) + snakeMap.pos.y;
        return newPos;
    }

    // Returns snake score
    inline uint8_t GetScore() const { return mScore; }

    // Returns the next movement type
    MoveType GetNextMovementType(const Apple &apple, const Map &snakeMap);
    // Change snake direction
    void ChangeDirection(const vec2i &newDirection);
    // Move snake
    void Move(const Map &snakeMap);
    // Eat apple and grow snake body
    void Eat(const Apple &apple, const Map &snakeMap);
    // Draw snake
    void Draw() const;

private:
    // List of snake body cells positions
    List<vec2i> mPositions;
    // Snake current direction
    vec2i mDirection;
    // Snake current speed
    uint8_t mSpeed;
    // Snake current score
    uint8_t mScore;
    // Snake body size
    const uint8_t BODY_SIZE = 1;
};


// Snake game class
class SnakeGame : public Game
{
public:
    // Constructor
    SnakeGame(const Map &snakeMap);
    // Update method
    void Update(int input) override;

private:
    // Game map
    Map mSnakeMap;
    // Snake object
    Snake mSnake;
    // Apple object
    Apple mApple;
    // Draw the game scene
    void Draw() const;
};

#endif
