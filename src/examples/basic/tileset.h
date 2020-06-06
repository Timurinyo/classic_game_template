#pragma once

#include <examples/basic/game_tile.h>
#include <examples/basic/game_grid.h>

class Tileset
{
public:
    static std::unique_ptr<Tileset> LoadFrom(cgt::render::IRenderContext& render, tmx_map* map, tmx_tileset* tileset, const char* basePath);

    void RenderGameGrid(GameGrid& grid, cgt::render::RenderQueue& queue);

private:
    cgt::render::TextureHandle m_Texture;

    typedef std::vector<cgt::render::SpriteDrawRequest> SpriteVariants;
    std::unordered_map<GameTile::Type, SpriteVariants> m_Sprites;
};