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
    E, // E (stands for empty): if the next move is in an empty valid cell
    A, // A (stands for apple): if the next move is eating an apple
    B  // B (stands for body collision): if the next move implies colliding with itself or a wall
};

// Apple class
class Apple
{
public:
    // Construct an apple object at random location in the map
    static Apple Spawn(const Map &snakeMap)
    {
        return Apple({static_cast<uint8_t>(random(snakeMap.pos.x, snakeMap.width - SIZE)), static_cast<uint8_t>(random(snakeMap.pos.y, snakeMap.height - SIZE))});
    }

    // Construct an apple object at a given position
    static Apple Spawn(const vec2i &position) { return Apple(position); }

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
    /*
    Snake constructor;
    Takes as argument:
        - snake start position
        - snake start direction
    Set initial speed to 1 and score to 0
    */
    Snake(const vec2i &startPosition, const vec2i &startDirection) : mSpeed(1), mScore(0)
    {
        // Add start position to a list of positions
        mPositions.Add(startPosition);
        // Set snake current direction
        mDirection = startDirection;
    }

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

    /*
    Returns a MoveType object, which is an enum of 3 values:
        - A (stands for apple): if the next move is eating an apple
        - B (stands for body collision): if the next move implies colliding with itself or a wall
        - E (stands for empty): if the next move is in an empty valid cell
*/
    MoveType GetNextMovementType(const Apple &apple, const Map &snakeMap)
    {
        const vec2i &nextPosition = GetNextPosition(snakeMap);

        // If collide with the apple
        if (apple.Collision(nextPosition))
            return MoveType::A;

        // Check if outside the map
        if (!inside(nextPosition, snakeMap.pos, snakeMap.width, snakeMap.height))
            return MoveType::B;
        // For each snake body position
        for (int8_t i = 1; i < mPositions.Count(); ++i)
        {
            // If next position is equal to another in the list of snake body positions ==> eat itself ==> body collision
            if (mPositions[i] == nextPosition)
                return MoveType::B;
            // This does the same thing as previous if, but it's more robust to floating point errors
            if (pointBetween(nextPosition, mPositions[i], mPositions[i - 1]))
                return MoveType::B;
        }
        // None of the previous if is true, so it's an empty valid cell
        return MoveType::E;
    }

    // Change snake direction
    void ChangeDirection(const vec2i &newDirection)
    {
        // Check if is a valid change of direction
        // e.g.: If snake is going right it cannot go left, otherwise it will eat himself causing gameover
        if ((mDirection + newDirection) != vec2i{0, 0})
            mDirection = newDirection;
    }
    /*
        Move snake in it's current mDirection;
        It's very easy the idea behind this function:
        at every frame we remove the last body position and we add the next position at the beginning of the list
        (like a queue)
    */
    void Move(const Map &snakeMap)
    {
        mPositions.Insert(GetNextPosition(snakeMap));
        mPositions.RemoveLast();
    }

    // Eat apple ==> snake body size increases
    void Eat(const Apple &apple, const Map &snakeMap)
    {
        // Play sound if speaker enabled
        if (gSpeakerOn)
            musicPlayer.beep(10);
        // Increase snake body
        mPositions.Insert(GetNextPosition(snakeMap));
        // Update score and speed based on score
        mScore++;
        if (mScore > 10)
            mSpeed = 2;
        if (mScore > 20)
            mSpeed = 3;
    }
    // Draw snake body
    void Draw() const
    {
        for (uint8_t i = 0; i < mPositions.Count(); ++i)
        {
            const vec2i &pos = mPositions[i];
            u8g2.drawBox(pos.x, pos.y, BODY_SIZE, BODY_SIZE);
        }
    }

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
    /*
        SnakeGame constructor implementation;
        Takes as argument the game map;
        Set current state as PLAYING and create Snake and Apple objects
    */
    SnakeGame(const Map &snakeMap) : Game(GameState::PLAYING),
                                     mSnakeMap(snakeMap),
                                     mSnake(Snake({10, 10}, {1, 0})),
                                     mApple(Apple::Spawn(snakeMap))
    {
    }
    // Update method for snake game, works like a state machine
    void Update(int input) override
    {
        // Playing state (the user is currently playing)
        if (mState == GameState::PLAYING)
        {
            switch (input)
            {
            // 4 ==> move left
            case KEY_4:
                mSnake.ChangeDirection({-1, 0});
                break;
            // 2 ==> move up
            case KEY_2: // 2
                mSnake.ChangeDirection({0, -1});
                break;
            // 8 ==> move down
            case KEY_8: // 8
                mSnake.ChangeDirection({0, 1});
                break;
            // 6 ==> move right
            case KEY_6: // 6
                mSnake.ChangeDirection({1, 0});
                break;
            // Play/Pause ==> pause game
            case PLAY_PAUSE_KEY:
                mState = GameState::PAUSE;
                break;
            // Powe key ==> quit game and go menu
            case POWER_KEY:
                // Quit the game
                mState = GameState::GO_MENU;
                break;
            }

            // Check if next position is either: empty or apple or body collision
            switch (mSnake.GetNextMovementType(mApple, snakeMap))
            {
            // If it's empty snake can move regularly
            case MoveType::E:
                mSnake.Move(snakeMap);
                break;
            // If it's an apple it should eat it and spawn a new apple
            case MoveType::A:
                mSnake.Eat(mApple, snakeMap);
                mApple = Apple::Spawn(snakeMap);
                break;
            // If it's a collision ==> GAME OVER!!!!
            case MoveType::B:
                mState = GameState::FINISHED; // GameOver
                break;
            }

            // Draw game scene
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
        // If the game is on GameOver state ==> draw gameover, and wait to press play to go on menu
        else if (mState == GameState::FINISHED)
        {
            DrawGameOver(mSnake.GetScore());
            if (input == PLAY_PAUSE_KEY)
                mState = GameState::GO_MENU;
        }
    }

private:
    // Game map
    Map mSnakeMap;
    // Snake object
    Snake mSnake;
    // Apple object
    Apple mApple;
    // Draw snake game scene
    void Draw() const
    {
        u8g2.firstPage(); // The first page shows
        do
        {
            // Draw map frame
            u8g2.drawFrame(mSnakeMap.pos.x, mSnakeMap.pos.y, mSnakeMap.width, mSnakeMap.height);
            // Draw score
            u8g2.setCursor(110, 13);
            u8g2.print(mSnake.GetScore());
            // Draw snake and the apple
            mSnake.Draw();
            mApple.Draw();
        } while (u8g2.nextPage());
    }
};

#endif
