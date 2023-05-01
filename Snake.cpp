// Include header file
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

    // Check if outside the map
    if (!inside(nextPosition, snakeMap.pos, snakeMap.width, snakeMap.height))
        return MoveType::B;
    
    // For each snake's body part
    for (int8_t i = 1; i < mPositions.Count(); ++i)
    {
        // body collision
        if (mPositions[i] == nextPosition)
            return MoveType::B;
        // Fallback: same check but robust to floating point errors
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
    if(gSpeakerOn) musicPlayer.beep(10);
    
    // Increase snake body
    mPositions.Insert(GetNextPosition(snakeMap));
    
    // Update score and speed based on score
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

// Construct an apple object at random location in the map
Apple Apple::Spawn(const Map &snakeMap)
{
    return Apple({static_cast<uint8_t>(random(snakeMap.pos.x, snakeMap.width - SIZE)), static_cast<uint8_t>(random(snakeMap.pos.y, snakeMap.height - SIZE))});
}

Apple Apple::Spawn(const vec2i &position) { return Apple(position); }

// SNAKEGAME Implementation
SnakeGame::SnakeGame(const Map &snakeMap) : 
    Game(GameState::PLAYING), 
    mSnakeMap(snakeMap), 
    mSnake(Snake({10, 10}, {1, 0})), 
    mApple(Apple::Spawn(snakeMap))
{
}


void SnakeGame::Update(int input)
{
    if (mState == GameState::PLAYING)
    {
        switch (input)
        {
        // move left
        case KEY_4: 
            mSnake.ChangeDirection({-1, 0});
            break;
        // move up
        case KEY_2:
            mSnake.ChangeDirection({0, -1});
            break;
        // move down
        case KEY_8: 
            mSnake.ChangeDirection({0, 1});
            break;
        // move right
        case KEY_6:
            mSnake.ChangeDirection({1, 0});
            break;
        // pause game
        case PLAY_PAUSE_KEY:
            mState = GameState::PAUSE;
            break;
        // quit game and go menu
        case POWER_KEY:
            mState = GameState::GO_MENU;
            break;
        }

        switch (mSnake.GetNextMovementType(mApple, snakeMap))
        {
        case MoveType::E:
            mSnake.Move(snakeMap);
            break;
        case MoveType::A:
            mSnake.Eat(mApple, snakeMap);
            mApple = Apple::Spawn(snakeMap);
            break;
        // If it's a collision ==> GAME OVER!!!!
        case MoveType::B:
            mState = GameState::FINISHED;
            break;
        }

        Draw();
    }

    else if (mState == GameState::PAUSE)
    {
        DrawPauseScreen();
        if (input == PLAY_PAUSE_KEY)
            mState = GameState::PLAYING;
    }
    else if (mState == GameState::FINISHED)
    {
        DrawGameOver(mSnake.GetScore());
        if (input == PLAY_PAUSE_KEY)
            mState = GameState::GO_MENU;
    }
}

void SnakeGame::Draw() const
{
    u8g2.firstPage();
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
