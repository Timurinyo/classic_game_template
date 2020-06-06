#include "CommandQueue.h"

CommandQueue::CommandQueue(int size /*= 5*/)
    : m_Size(size)
{
    Reset();
}

const Command& CommandQueue::GetCurrent() const
{
    if (m_CommandIndx < m_Commands.size())
    {
        return m_Commands[m_CommandIndx];
    }
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

void CommandQueue::Reset()
{
    m_Commands = std::vector<Command>(m_Size, m_NullCommand);
    m_CommandIndx = 0;
}

void CommandQueue::StepForward()
{
    ++m_CommandIndx;

    if (m_CommandIndx < m_Commands.size() && m_CommandIndx > 0)
    {
        while (m_Commands[m_CommandIndx].ID == CommandID::Repeat)
        {
            Command& currentCommand = m_Commands[m_CommandIndx];
            if (m_CommandIndx == 0) return;

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
        }
        
    }
}
