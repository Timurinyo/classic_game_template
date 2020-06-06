enum class GameState
{
    Start,
    InGame,
    FinishedLevel,
};

class GameStateKeeper
{
public:
    void SetState(GameState newState) { m_CurrentState = newState; }

    GameState GetState() const { return m_CurrentState; }

private:
    GameState m_CurrentState = GameState::Start;
};