#include "CommandQueue.h"

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

    m_CurrentState = State::Stop;
}