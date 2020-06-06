#pragma once

#include <engine/system.h>
#include <engine/external_libs.h>

#include <render_core/i_render_context.h>
#include <render_core/render_queue.h>

#include "CommandQueue.h"
#include "game_grid.h"

class GameGrid;

enum DirectionID
{
    S = 0,
    SW,
    W,
    WN,
    N,
    NE,
    E,
    ES,
    Count
};

enum class PlayerStateID
{
    Idle = 0,
    Rotating,
    Moving,
    Dying,
    ReachedGoal,

    Count
};


class BasicPlayer
{
public:
    BasicPlayer(const char* parentFolderPath, cgt::render::IRenderContext& render)
        : m_CoordsCurrent(0,0)
    {
        InitTextures(parentFolderPath, render);
        InitDirectionsMap();
    };

    void Render(cgt::render::RenderQueue& queue);

    void RenderHat(cgt::render::RenderQueue& queue);
    void RenderAvatar(cgt::render::RenderQueue &queue);

    void Update(float dt);

    void MoveForward(float dt);

    void Rotate(float dt);

    void Execute(CommandID command, GameGrid& grid);

    PlayerStateID GetPlayerState() const { return m_PlayerState; }

    glm::vec2 GetPosition() const { return m_CoordsCurrent; };

    void SetPosition(glm::vec2 position) { m_CoordsCurrent = position; };

    void Spawn(GameGrid& gameGrid);

private:
    typedef std::unordered_map<DirectionID, cgt::render::TextureHandle> DirectionTexture;

    DirectionTexture m_AvatarTextures;

    DirectionTexture m_MagicHatTextures;
    DirectionTexture m_CrownTextures;

    DirectionTexture* m_CurrentHatTextures = &m_MagicHatTextures;

    glm::vec2 m_CoordsCurrent;
    glm::vec2 m_CoordsPrev;
    glm::vec2 m_CoordsNext;

    DirectionID m_DirectionCurrent = DirectionID::SW;
    DirectionID m_DirectionPrev = DirectionID::S;
    DirectionID m_DirectionNext = DirectionID::S;

    std::vector<glm::vec2> m_DirectionsMap;

    float m_ForwardStepSize = 1;
    float m_RotationStep = 2;

    float m_MoveTimer, m_RotateTimer;

    const float m_TimePerMove = 1.f;
    const float m_TimePerRotation = 1.f;

    PlayerStateID m_PlayerState = PlayerStateID::Idle;

    bool m_IsPlayerSpawned = false;

    void InitTextures(const char* path, cgt::render::IRenderContext& render);
    void InitDirectionsMap();

    GameTile::Type m_NextTileType = GameTile::Type::Undefined;
};

const char* PlayerStateToString(PlayerStateID state);