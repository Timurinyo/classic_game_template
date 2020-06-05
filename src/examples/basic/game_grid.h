#pragma once

struct GameTile
{
    enum class Type
    {
        Undefined,
        Start,
        Goal,
        Ground,
        Water,
    };

    Type type = Type::Undefined;

    static GameTile FromTMXTile(tmx_tile& tile);
};

class GameGrid
{
public:
    GameGrid(tmx_map* map);

    GameTile& At(u32 x, u32 y);

    u32 Width() { return m_Width; }
    u32 Height() { return m_Height; }
private:
    u32 m_Width;
    u32 m_Height;

    std::vector<GameTile> m_Grid;
};