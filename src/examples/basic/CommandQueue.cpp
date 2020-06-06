#include "CommandQueue.h"
#include "BasicPlayer.h"

CommandQueue::CommandQueue(int size /*= 5*/)
    : m_Size(size)
{
    Erase();
}

const Command& CommandQueue::GetCurrent()
{
    if (m_CommandIndx < m_Commands.size())
    {
        return m_Commands[m_CommandIndx];
    }

    m_CurrentState = State::Finished;
    return m_NullCommand;
}

const Command& CommandQueue::Get(int indx) const
{
    assert(indx < m_Size&& indx >= 0);
    if (indx < m_Size&& indx >= 0)
    {
        return m_Commands[indx];
    }
    return m_NullCommand;
}

void CommandQueue::Set(int indx, const Command& command)
{
    assert(indx < m_Size && indx >= 0);
    if (indx < m_Size && indx >= 0)
    {
        m_Commands[indx] = command;
    }
}

void CommandQueue::Erase()
{
    m_Commands = std::vector<Command>(m_Size, m_NullCommand);
    m_CommandIndx = 0;

    m_CurrentState = State::Stop;
}

void CommandQueue::Reset()
{
    m_CommandIndx = 0;
    for (Command& command : m_Commands)
    {
        command.Reset();
    }

    m_CurrentState = State::Stop;
}


void CommandQueue::StepForward(GameTile::Type previousViewedTile, GameTile::Type currentViewedTile)
{
    ++m_CommandIndx;

    if (m_CommandIndx < m_Commands.size() && m_CommandIndx > 0)
    {
        while (m_Commands[m_CommandIndx].ID == CommandID::Repeat || m_Commands[m_CommandIndx].ID == CommandID::DoWhile)
        {
            Command& currentCommand = m_Commands[m_CommandIndx];
            if (m_CommandIndx == 0) return;

            switch (currentCommand.ID)
            {
                case CommandID::Repeat:
                {
                    if (--currentCommand.CurrentRepeats > 0)
                    {
                        --m_CommandIndx;
                        if (m_Commands[m_CommandIndx].ID == CommandID::Repeat)
                        {
                            ++m_Commands[m_CommandIndx].CurrentRepeats;
                        }
                    }
                    else
                    {
                        ++m_CommandIndx;
                        currentCommand.Reset();
                    }
                    break;
                }
                case CommandID::DoWhile:
                {
                    if (currentCommand.LastMemorizedTile == GameTile::Type::Undefined)
                    {
                        currentCommand.LastMemorizedTile = previousViewedTile;
                    }
                    else if (currentViewedTile != GameTile::Type::Goal && currentViewedTile != currentCommand.LastMemorizedTile)
                    {
                        ++m_CommandIndx;
                        currentCommand.Reset();
                    }
                    else
                    {
                        --m_CommandIndx;
                    }
                    break;
                }
                default:
                    break;
            }

            
        }
    }
}
