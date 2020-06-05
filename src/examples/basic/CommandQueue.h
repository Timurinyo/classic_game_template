#pragma once

#include <vector>
#include <memory>

#include <engine/external_libs.h>
#include <engine/system.h>

enum class CommandID
{
    None,
    MoveForward,
    TurnLeft,
    TurnRight,
};

struct Command
{
    CommandID ID;
    const char* DisplayName;

    // add some functor that gets called whenever the command is executed
};

class CommandQueue
{
public:
    CommandQueue(int size = 5);
    const std::vector<Command>& GetAll() const { return m_Commands; }
    const Command& GetCurrent() const;
    const Command& Get(int indx) const;

    void Set(int indx, const Command& command);
    void Reset();

    void StepForward() { ++m_CommandIndx; }

private:
    int m_CommandIndx = 0;

    const int m_Size;
    std::vector<Command> m_Commands;

    const Command m_NullCommand = { CommandID::None, "NULL" };
};
