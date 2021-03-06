#include <examples/basic/pch.h>

#include <examples/basic/game_grid.h>

GameGrid::GameGrid(tmx_map* map, tmx_layer* layer, bool initialyDiscovered)
    : m_Width(map->width)
    , m_Height(map->height)
    , m_InitialyDiscovered(initialyDiscovered)
{
    m_Grid.resize(m_Width * m_Height);

    for (u32 y = 0; y < map->height; ++y)
    {
        for (u32 x = 0; x < map->width; ++x)
        {
            usize idx = y * map->width + x;
            auto gid = (layer->content.gids[idx]) & TMX_FLIP_BITS_REMOVAL;
            tmx_tile* tile = map->tiles[gid];
            if (tile)
            {
                this->At(x, y) = GameTile::FromTMXTile(*tile);
                if (initialyDiscovered)
                {
                    this->At(x, y).discovered = true;
                }

                if (this->At(x, y).type == GameTile::Type::Start)
                {
                    m_StartTileCoords = glm::vec2(x, y);
                }
            }
        }
    }
}

GameTile& GameGrid::At(u32 x, u32 y)
{
    CGT_ASSERT(x < m_Width && y < m_Height);

    return m_Grid[y * m_Width + x];
}

void GameGrid::UndiscoverAllTiles()
{
    if (m_InitialyDiscovered)
    {
        return;
    }

    for(u32 y = 0; y < m_Height; ++y)
    {
        for(u32 x = 0; x < m_Width; ++x)
        {
            this->At(x,y).discovered = false;
        }
    }
}
