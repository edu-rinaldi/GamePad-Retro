#ifndef SNAKE_H
#define SNAKE_H

#include "ListLib.h"
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

class Apple
{
public:
    static Apple Spawn(const Map &snakeMap);
    static Apple Spawn(const vec2i &position);
    
    inline vec2i GetPosition() const { return mPosition; }
    inline void Draw() const { u8g2.drawBox(mPosition.x, mPosition.y, SIZE, SIZE); }
    inline bool Collision(const vec2i &position) const { return inside(position, mPosition, SIZE, SIZE); }

private:
    Apple(const vec2i &position) : mPosition(position) {}
    vec2i mPosition;
    static const uint8_t SIZE = 5;
};

class Snake
{
public:
    Snake(const vec2i &startPosition, const vec2i &startDirection);
    
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

    inline uint8_t GetScore() const { return mScore; }

    MoveType GetNextMovementType(const Apple &apple, const Map &snakeMap);
    void ChangeDirection(const vec2i &newDirection);
    void Move(const Map &snakeMap);
    void Eat(const Apple &apple, const Map &snakeMap);
    void Draw() const;

private:
    List<vec2i> mPositions; // List of snake body cells positions
    vec2i mDirection;
    uint8_t mSpeed;
    uint8_t mScore;
    const uint8_t BODY_SIZE = 1;
};


class SnakeGame : public Game
{
public:
    SnakeGame(const Map &snakeMap);
    void Update(int input) override;

private:
    Map mSnakeMap;
    Snake mSnake;
    Apple mApple;
    void Draw() const;
};

#endif
