#pragma once

#include <examples/basic/game_tile.h>

class GameGrid
{
public:
    GameGrid(tmx_map* map, tmx_layer* layer, bool initialyDiscovered);

    GameTile& At(u32 x, u32 y);

    u32 Width() { return m_Width; }
    u32 Height() { return m_Height; }

    glm::vec2 GetStartTileCoords() const { return m_StartTileCoords; }

    void UndiscoverAllTiles();

private:
    u32 m_Width;
    u32 m_Height;

    std::vector<GameTile> m_Grid;

    glm::vec2 m_StartTileCoords;

    bool m_InitialyDiscovered = false;
};