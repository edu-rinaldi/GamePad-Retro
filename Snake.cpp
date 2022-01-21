// Include header file
#include "Snake.h"

/*
    Snake constructor;
    Takes as argument:
        - snake start position
        - snake start direction
    Set initial speed to 1 and score to 0
*/
Snake::Snake(const vec2i &startPosition, const vec2i &startDirection) : mSpeed(1), mScore(0)
{
    // Add start position to a list of positions
    mPositions.Add(startPosition);
    // Set snake current direction
    mDirection = startDirection;
}

// Change snake direction
void Snake::ChangeDirection(const vec2i &newDirection)
{
    // Check if is a valid change of direction
    // e.g.: If snake is going right it cannot go left, otherwise it will eat himself causing gameover
    if ((mDirection + newDirection) != vec2i{0, 0})
        mDirection = newDirection;
}

/*
    Returns a MoveType object, which is an enum of 3 values:
        - A (stands for apple): if the next move is eating an apple
        - B (stands for body collision): if the next move implies colliding with itself or a wall 
        - E (stands for empty): if the next move is in an empty valid cell
*/
MoveType Snake::GetNextMovementType(const Apple &apple, const Map &snakeMap)
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

/* 
    Move snake in it's current mDirection;
    It's very easy the idea behind this function:
    at every frame we remove the last body position and we add the next position at the beginning of the list
    (like a queue)
*/
void Snake::Move(const Map &snakeMap)
{
    mPositions.Insert(GetNextPosition(snakeMap));
    mPositions.RemoveLast();
}

// Eat apple ==> snake body size increases
void Snake::Eat(const Apple &apple, const Map &snakeMap)
{
    // Play sound if speaker enabled
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

// Construct an apple object at a given position
Apple Apple::Spawn(const vec2i &position) { return Apple(position); }

// SNAKEGAME Implementation

/* 
    SnakeGame constructor implementation;
    Takes as argument the game map;
    Set current state as PLAYING and create Snake and Apple objects
*/
SnakeGame::SnakeGame(const Map &snakeMap) : 
    Game(GameState::PLAYING), 
    mSnakeMap(snakeMap), 
    mSnake(Snake({10, 10}, {1, 0})), 
    mApple(Apple::Spawn(snakeMap))
{
}


void SnakeGame::Update(int input)
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
        case KEY_2:
            mSnake.ChangeDirection({0, -1});
            break;
        // 8 ==> move down
        case KEY_8: 
            mSnake.ChangeDirection({0, 1});
            break;
        // 6 ==> move right
        case KEY_6:
            mSnake.ChangeDirection({1, 0});
            break;
        // Play/Pause ==> pause game
        case PLAY_PAUSE_KEY:
            mState = GameState::PAUSE;
            break;
        // Powe key ==> quit game and go menu
        case POWER_KEY:
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
            mState = GameState::FINISHED;
            break;
        }

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

// Draw snake game scene
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
