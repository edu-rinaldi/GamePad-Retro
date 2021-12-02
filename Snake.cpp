#include "Snake.h"

Snake::Snake(const vec2i &startPosition, const vec2i &startDirection) : mSpeed(1), mScore(0)
{
    mPositions.Add(startPosition);
    mDirection = startDirection;
}

void Snake::ChangeDirection(const vec2i &newDirection)
{
    // Check if is a valid change of direction
    // e.g.: If snake is going right it cannot go left, otherwise it will eat himself causing gameover
    if ((mDirection + newDirection) != vec2i{0, 0})
        mDirection = newDirection;
}

MoveType Snake::GetNextMovementType(const Apple &apple, const Map &snakeMap)
{
    const vec2i &nextPosition = GetNextPosition(snakeMap);
    if (apple.Collision(nextPosition))
        return MoveType::A;

    if (!inside(nextPosition, snakeMap.pos, snakeMap.width, snakeMap.height))
        return MoveType::B;
    for (int8_t i = 1; i < mPositions.Count(); ++i)
    {
        if (mPositions[i] == nextPosition)
            return MoveType::B;
        if (pointBetween(nextPosition, mPositions[i], mPositions[i - 1]))
            return MoveType::B;
    }

    return MoveType::E;
}

void Snake::Move(const Map &snakeMap)
{
    mPositions.Insert(GetNextPosition(snakeMap));
    mPositions.RemoveLast();
}
void Snake::Eat(const Apple &apple, const Map &snakeMap)
{
    musicPlayer.beep(10);
    mPositions.Insert(GetNextPosition(snakeMap));
    mScore++;
    if (mScore > 10)
        mSpeed = 2;
    if (mScore > 20)
        mSpeed = 3;
}

void Snake::Draw() const
{
    for (uint8_t i = 0; i < mPositions.Count(); ++i)
    {
        const vec2i &pos = mPositions[i];
        u8g2.drawBox(pos.x, pos.y, BODY_SIZE, BODY_SIZE);
    }
}

Apple Apple::Spawn(const Map &snakeMap)
{
    return Apple({static_cast<uint8_t>(random(snakeMap.pos.x, snakeMap.width - SIZE)), static_cast<uint8_t>(random(snakeMap.pos.y, snakeMap.height - SIZE))});
}

Apple Apple::Spawn(const vec2i &position) { return Apple(position); }

// SNAKEGAME Implementation

// SnakeGame constructor implementation
SnakeGame::SnakeGame(const Map &snakeMap) : Game(0), mSnakeMap(snakeMap), mSnake(Snake({10, 10}, {1, 0})), mApple(Apple::Spawn(snakeMap))
{
}

void SnakeGame::Update(int input)
{
    // Playing state (the user is currently playing)
    if (mState == 0)
    {
        switch (input)
        {
        case KEY_4: // 4
            mSnake.ChangeDirection({-1, 0});
            break;
        case KEY_2: // 2
            mSnake.ChangeDirection({0, -1});
            break;
        case KEY_8: // 8
            mSnake.ChangeDirection({0, 1});
            break;
        case KEY_6: // 6
            mSnake.ChangeDirection({1, 0});
            break;
        case PLAY_PAUSE_KEY:
            mState = 1;
            break;
        case POWER_KEY:
            // Quit the game
            mState = 3;
            break;
        }

        // Check if next position is either: empty or apple or body collision
        switch (mSnake.GetNextMovementType(mApple, snakeMap))
        {
        case MoveType::E:
            mSnake.Move(snakeMap);
            break;
        case MoveType::A:
            mSnake.Eat(mApple, snakeMap);
            mApple = Apple::Spawn(snakeMap);
            break;
        case MoveType::B:
            mState = 2; // GameOver
            break;
        }

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
    // If the game is on GameOver state ==> draw gameover, and wait to press play to go on menu
    else if (mState == 2)
    {
        DrawGameOver(mSnake.GetScore());
        if (input == PLAY_PAUSE_KEY)
            mState = 3;
    }
}

void SnakeGame::Draw() const
{
    u8g2.firstPage(); // The first page shows
    do
    {
        u8g2.drawFrame(mSnakeMap.pos.x, mSnakeMap.pos.y, mSnakeMap.width, mSnakeMap.height);
        u8g2.setCursor(110, 13);
        u8g2.print(mSnake.GetScore());
        mSnake.Draw();
        mApple.Draw();
    } while (u8g2.nextPage());
}
