#include <examples/basic/BasicPlayer.h>
#include <examples/basic/utils.h>

#include <examples/basic/game_grid.h>

void BasicPlayer::Render(cgt::render::RenderQueue& queue)
{
    if(!m_IsPlayerSpawned)
    {
        return;
    }

    RenderAvatar(queue);
    RenderHat(queue);
    return;
}

void BasicPlayer::RenderHat(cgt::render::RenderQueue& queue)
{
    if(m_PlayerState == PlayerStateID::Dying)
    {
        return;
    }

    cgt::render::SpriteDrawRequest sprite;
    sprite.rotation = 45.f;

    cgt::render::TextureHandle hatTexture;

    switch(m_CurrentHatType)
    {
    case HatType::WizardHat:
    {
        hatTexture = m_MagicHatTextures[m_DirectionCurrent];
        sprite.position = m_CoordsCurrent + glm::vec2(-0.9, 0.9) - m_DirectionsMap[m_DirectionCurrent] * glm::vec2(0.15, 0.15);
        sprite.scale = {1.0f,1.536};
        break;
    }
    case HatType::Crown:
    {
        hatTexture = m_CrownTextures[m_DirectionCurrent];
        sprite.position = m_CoordsCurrent + glm::vec2(-1.0, 1.0);
        sprite.scale ={0.8f,1.22};
        break;
    }
    default:
        break;
    }

    sprite.texture = std::move(hatTexture);
    queue.sprites.emplace_back(std::move(sprite));
}

void BasicPlayer::RenderAvatar(cgt::render::RenderQueue &queue)
{
    cgt::render::TextureHandle avatarTexture;
    avatarTexture = m_AvatarTextures[m_DirectionCurrent];

    cgt::render::SpriteDrawRequest sprite;
    sprite.position = m_CoordsCurrent + glm::vec2(-0.3, 0.3);
    sprite.rotation = m_CurrentAngle;

    switch(m_PlayerState)
    {
    case PlayerStateID::Dying:
    {
        sprite.colorTint = glm::vec4(1,0.3,0.3,1);
        break;
    }
    case PlayerStateID::ReachedGoal:
    {
        m_CurrentHatType = HatType::Crown;
        break;
    }
    default:
        break;
    }

    sprite.texture = std::move(avatarTexture);
    sprite.scale = {1.f,1.536};
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
        Rotate(dt);
        break;
    }
    case PlayerStateID::Moving:
    {
        MoveForward(dt);
        break;
    }
    case PlayerStateID::Dying:
    {
        Die(dt);
        break;
    }
    default:
        break;
    }

}

void BasicPlayer::MoveForward(float dt)
{
    m_MoveTimer += dt;

    m_CoordsCurrent = LerpVec2(m_CoordsPrev, m_CoordsNext, m_MoveTimer / m_TimePerMove);

    if (m_MoveTimer > m_TimePerMove)
    {
        m_MoveTimer = 0;
        m_PlayerState = PlayerStateID::Idle;

        switch(m_NextTileType)
        {
        case GameTile::Type::Goal:
        {
            m_PlayerState = PlayerStateID::ReachedGoal;
            break;
        }
        case GameTile::Type::Water:
        {
            m_PlayerState = PlayerStateID::Dying;
            break;
        }
        default:
            break;
        }
    }
}

void BasicPlayer::Rotate(float dt)
{
    m_RotateTimer += dt;

    float newPlayerDirection;

    if(((m_DirectionNext - m_DirectionPrev) > DirectionID::Count/2))
    {
        int directionNextTemp = m_DirectionNext - DirectionID::Count;
        newPlayerDirection = LerpFloat(m_DirectionPrev,directionNextTemp,m_RotateTimer / m_TimePerRotation);
        if(newPlayerDirection < 0)
        {
            newPlayerDirection += DirectionID::Count;
        }
    }
    else if((m_DirectionNext - m_DirectionPrev) < -DirectionID::Count/2)
    {
        int directionPreviousTemp = m_DirectionPrev - DirectionID::Count;
        newPlayerDirection = LerpFloat(directionPreviousTemp,m_DirectionNext,m_RotateTimer / m_TimePerRotation);
        if(newPlayerDirection < 0)
        {
            newPlayerDirection += DirectionID::Count;
        }
    }
    else
    {
        newPlayerDirection = LerpFloat(m_DirectionPrev,m_DirectionNext,m_RotateTimer / m_TimePerRotation);
    }

    m_DirectionCurrent = static_cast<DirectionID>(std::floor(newPlayerDirection));

    if(m_RotateTimer > m_TimePerRotation)
    {
        m_RotateTimer = 0;
        m_PlayerState = PlayerStateID::Idle;
    }
}

void BasicPlayer::Die(const float dt)
{
    m_DieTimer += dt;
    m_CurrentAngle = LerpFloat(45.f, 90.f, m_DieTimer / m_TimePerDeath);

    if (m_DieTimer > m_TimePerDeath)
    {
        m_DieTimer = 0;
        m_PlayerState = PlayerStateID::Dead;
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

        m_NextTileType = grid.At(m_CoordsNext.x, -m_CoordsNext.y).type;

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

    m_PlayerState = PlayerStateID::Idle;

    m_CurrentAngle = m_CurrentAngleDefault;

    m_DirectionCurrent = m_DirectionDefault;

}

void BasicPlayer::InitTextures(const char* parentFolderPath, cgt::render::IRenderContext& render)
{
    for (int i = 0; i < DirectionID::Count; i++)
    {
        std::string avatarImagePath = fmt::format("{}/char01_000{}.png", parentFolderPath, i);
        std::string hatImagePath = fmt::format("{}/WizardHat000{}.png", parentFolderPath, i);
        std::string crownImagePath = fmt::format("{}/crown000{}.png", parentFolderPath, i);

        m_AvatarTextures[static_cast<DirectionID>(i)] = render.LoadTexture(avatarImagePath.c_str());
        m_MagicHatTextures[static_cast<DirectionID>(i)] = render.LoadTexture(hatImagePath.c_str());
        m_CrownTextures[static_cast<DirectionID>(i)] = render.LoadTexture(crownImagePath.c_str());
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


const char* PlayerStateToString(PlayerStateID state)
{
#define StateToStr(state) case state: return #state
    switch (state)
    {
        StateToStr(PlayerStateID::Idle);
        StateToStr(PlayerStateID::Moving);
        StateToStr(PlayerStateID::Rotating);
        StateToStr(PlayerStateID::Dying);
        default: return "Undefined";
    }
#undef StateToStr
}