#include <examples/basic/BasicPlayer.h>
#include <examples/basic/utils.h>

#include <examples/basic/game_grid.h>

void BasicPlayer::Render(cgt::render::RenderQueue& queue)
{
    if (!m_IsPlayerSpawned)
    {
        return;
    }
    cgt::render::TextureHandle texture;
    texture = m_Textures[m_DirectionCurrent];

    cgt::render::SpriteDrawRequest sprite;
    sprite.position = m_CoordsCurrent;
    sprite.rotation = 45.f;
    sprite.texture = std::move(texture);
    sprite.scale = { 1.f, 1.536 };
    queue.sprites.emplace_back(std::move(sprite));
}

void BasicPlayer::Update(float dt)
{
    switch(m_PlayerState)
    {
    case PlayerStateID::Idle:
        break;
    case PlayerStateID::Rotating:
    {
        m_RotateTimer += dt;

        float newPlayerDirection; 

        if (((m_DirectionNext - m_DirectionPrev) > DirectionID::Count/2))
        {
            int directionNextTemp = m_DirectionNext - DirectionID::Count;
            newPlayerDirection = LerpFloat(m_DirectionPrev, directionNextTemp, m_RotateTimer / m_TimePerRotation);
            if (newPlayerDirection < 0)
            {
                newPlayerDirection += DirectionID::Count;
            }
        }
        else if ((m_DirectionNext - m_DirectionPrev) < -DirectionID::Count/2)
        {
            int directionPreviousTemp = m_DirectionPrev - DirectionID::Count;
            newPlayerDirection = LerpFloat(directionPreviousTemp, m_DirectionNext, m_RotateTimer / m_TimePerRotation);
            if(newPlayerDirection < 0)
            {
                newPlayerDirection += DirectionID::Count;
            }
        }
        else
        {
            newPlayerDirection = LerpFloat(m_DirectionPrev, m_DirectionNext, m_RotateTimer / m_TimePerRotation);
        }

        m_DirectionCurrent = static_cast<DirectionID>(std::floor(newPlayerDirection));

        if (m_RotateTimer > m_TimePerRotation)
        {
            m_RotateTimer = 0;
            m_PlayerState = PlayerStateID::Idle;
        }
        break;
    }
    case PlayerStateID::Moving:
    {
        m_MoveTimer += dt;

        m_CoordsCurrent = LerpVec2(m_CoordsPrev, m_CoordsNext, m_MoveTimer / m_TimePerMove);

        if (m_MoveTimer > m_TimePerMove)
        {
            m_MoveTimer = 0;
            m_PlayerState = PlayerStateID::Idle;
        }
        break;
    }
    default:
        break;
    }

}

void BasicPlayer::Execute(CommandID command, GameGrid& grid)
{   
    m_MoveTimer = 0;
    m_RotateTimer = 0;

    switch(command)
    {
    case CommandID::MoveForward:
    {
        m_PlayerState = PlayerStateID::Moving;
            
        m_CoordsPrev = m_CoordsCurrent;
        m_CoordsNext = m_CoordsCurrent + m_DirectionsMap[m_DirectionCurrent];

        //TODO: react on grid.At(m_CoordsNext). Could be Drawning or Confusion

        break;
    }
    case CommandID::TurnLeft:
    {
        m_PlayerState = PlayerStateID::Rotating;

        m_DirectionPrev = m_DirectionCurrent;
        m_DirectionNext = static_cast<DirectionID>(m_DirectionCurrent - m_RotationStep);
        if (m_DirectionNext < 0)
        {
            m_DirectionNext = static_cast<DirectionID>(DirectionID::Count + m_DirectionNext);
        }
        break;
    }
    case CommandID::TurnRight:
    {
        m_PlayerState = PlayerStateID::Rotating;

        m_DirectionPrev = m_DirectionCurrent;
        m_DirectionNext = static_cast<DirectionID>(m_DirectionCurrent + m_RotationStep);
        if (m_DirectionNext > DirectionID::Count - 1)
        {
            m_DirectionNext = static_cast<DirectionID>(-(DirectionID::Count - m_DirectionNext));
        }
        break;
    }
    default:
        break;
    }
}

void BasicPlayer::Spawn(GameGrid& gameGrid)
{
    glm::vec2 tempCoords = gameGrid.GetStartTileCoords();

    m_CoordsCurrent = glm::vec2(tempCoords.x, tempCoords.y * -1.f);

    m_IsPlayerSpawned = true;
}

void BasicPlayer::InitTextures(const char* path, cgt::render::IRenderContext& render)
{
    for (int i = 0; i < DirectionID::Count; i++)
    {
        std::string fullImagePath = fmt::format("{}_000{}.png", path, i);
        m_Textures[static_cast<DirectionID>(i)] = render.LoadTexture(fullImagePath.c_str());
    }
}

void BasicPlayer::InitDirectionsMap()
{
    m_DirectionsMap = {
        glm::vec2(1.f,-1.f),
        glm::vec2(0.f,-1.f),
        glm::vec2(-1.f,-1.f),
        glm::vec2(-1.f,0.f),
        glm::vec2(-1.f,1.f),
        glm::vec2(0.f,1.f),
        glm::vec2(1.f,1.f),
        glm::vec2(1.f,0.f),
    };
}
