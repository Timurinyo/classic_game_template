#pragma once

#include <engine/system.h>
#include <engine/external_libs.h>

#include <render_core/i_render_context.h>
#include <render_core/render_queue.h>

enum class CommandID
{
    None,
    MoveForward,
    TurnLeft,
    TurnRight,
};

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
    Count
};


class BasicPlayer
{
public:
    BasicPlayer(const char* path, cgt::render::IRenderContext& render)
        : m_CoordsCurrent(0,0)
    {
        InitTextures(path, render);
        InitDirectionsMap();
    };

    void Render(cgt::render::RenderQueue& queue);

    void Update(float dt);

    void Execute(CommandID command);

private:
    std::unordered_map<DirectionID, cgt::render::TextureHandle> m_Textures;

    glm::vec2 m_CoordsCurrent;
    glm::vec2 m_CoordsPrev;
    glm::vec2 m_CoordsNext;

    DirectionID m_DirectionCurrent = DirectionID::SW;
    DirectionID m_DirectionPrev = DirectionID::S;
    DirectionID m_DirectionNext = DirectionID::S;

    std::vector<glm::vec2> m_DirectionsMap;

    int m_ForwardStepSize;
    int m_RotationStepSize;

    float m_MoveTimer, m_RotateTimer;

    const float m_TimePerMove = 1.f;
    const float m_TimePerRotation = 1000.f;

    PlayerStateID m_PlayerState = PlayerStateID::Idle;
    //CommandID

    void InitTextures(const char* path, cgt::render::IRenderContext& render);
    void InitDirectionsMap();
};