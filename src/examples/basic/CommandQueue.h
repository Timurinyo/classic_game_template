#pragma once

#include <vector>
#include <memory>

#include <engine/external_libs.h>
#include <engine/system.h>

#include "game_tile.h"

enum class State
{
    Execution,
    Pause,
    Stop,
    Finished,
    NeedRestart
};

enum class CommandID
{
    None,
    MoveForward,
    TurnLeft,
    TurnRight,
    Repeat,
    DoWhile,
};

struct Command
{
    CommandID ID;
    const char* DisplayName;

    int InitialRepeats = 1;
    int CurrentRepeats = 1;

    GameTile::Type LastMemorizedTile = GameTile::Type::Undefined;

    void Reset() { CurrentRepeats = InitialRepeats; LastMemorizedTile = GameTile::Type::Undefined; }
};

class CommandQueue
{
public:
    CommandQueue(int size = 20);
    const std::vector<Command>& GetAll() const { return m_Commands; }

    const Command& GetCurrent();

    int GetCurrentIdx() const { return m_CommandIndx; }

    const Command& Get(int indx) const;

    void Set(int indx, const Command& command);
    void Erase();

    void Reset();

    void StepForward(GameTile::Type previousViewedTile, GameTile::Type currentViewedTile);

    State GetState() const { return m_CurrentState; };
    void SetState(State newState) { m_CurrentState = newState; };

private:
    int m_CommandIndx = 0;

    const int m_Size;
    std::vector<Command> m_Commands;

    const Command m_NullCommand = { CommandID::None, "NULL" };

    State m_CurrentState = State::Stop;
};
