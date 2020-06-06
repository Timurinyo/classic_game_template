#include <examples/basic/pch.h>

#include <examples/basic/game_grid.h>

GameGrid::GameGrid(tmx_map* map, tmx_layer* layer)
    : m_Width(map->width)
    , m_Height(map->height)
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
