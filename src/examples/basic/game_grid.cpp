#include <examples/basic/pch.h>

#include <examples/basic/game_grid.h>

GameTile GameTile::FromTMXTile(tmx_tile& tile)
{
    GameTile newTile;
    auto match = [](const char* a, const char* b)
    {
        return
            a != nullptr &&
            b != nullptr &&
            strcmp(a, b) == 0;
    };

    if (match(tile.type, "START")) {
        newTile.type = Type::Start;
    } else if (match(tile.type, "GOAL")) {
        newTile.type = Type::Goal;
    } else if (match(tile.type, "GROUND")) {
        newTile.type = Type::Ground;
    } else if (match(tile.type, "WATER")) {
        newTile.type = Type::Water;
    } else if (tile.type != nullptr) {
        CGT_PANIC("Unsupported tile type: {}", tile.type);
    }

    return newTile;
}

GameGrid::GameGrid(tmx_map* map)
    : m_Width(map->width)
    , m_Height(map->height)
{
    m_Grid.resize(m_Width * m_Height);

    for (auto* layer = map->ly_head; layer != nullptr; layer = layer->next)
    {
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
}

GameTile& GameGrid::At(u32 x, u32 y)
{
    CGT_ASSERT(x < m_Width && y < m_Height);

    return m_Grid[y * m_Width + x];
}
