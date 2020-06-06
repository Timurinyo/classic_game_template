#include <examples/basic/pch.h>

#include <examples/basic/tileset.h>

std::unique_ptr<Tileset> Tileset::LoadFrom(cgt::render::IRenderContext& render, tmx_map* map, tmx_tileset* tileset, const char* basePath)
{
    auto newTileset = std::make_unique<Tileset>();

    const std::string texturePath = fmt::format("{}/{}", basePath, tileset->image->source);
    newTileset->m_Texture = render.LoadTexture(texturePath.c_str());

    for (u32 i = 0; i < tileset->tilecount; ++i)
    {
        tmx_tile& tile = tileset->tiles[i];
        auto gameTile = GameTile::FromTMXTile(tile);
        auto& variants = newTileset->m_Sprites[gameTile.type];
        if (variants.empty())
        {
            variants.resize(GameTile::MAX_VARIANTS);
        }

        cgt::render::SpriteDrawRequest sprite;
        sprite.texture = newTileset->m_Texture;
        
        const u32 minX = tile.ul_x;
        const u32 minY = tile.ul_y;
        const u32 maxX = minX + tileset->tile_width;
        const u32 maxY = minY + tileset->tile_height;
        glm::vec2 uvMin(
            (float)minX / tileset->image->width,
            (float)minY / tileset->image->height);
        glm::vec2 uvMax(
            (float)maxX / tileset->image->width,
            (float)maxY / tileset->image->height);

        sprite.uvMin = uvMin;
        sprite.uvMax = uvMax;

        variants[gameTile.variant] = sprite;
    }

    return newTileset;
}

void Tileset::RenderGameGrid(GameGrid& grid, cgt::render::RenderQueue& queue)
{
    for (u32 y = 0; y < grid.Height(); ++y)
    {
        for (u32 x = 0; x < grid.Width(); ++x)
        {
            GameTile& tile = grid.At(x, y);
            if (tile.type == GameTile::Type::Undefined)
            {
                continue;;
            }

            auto variantsIter = m_Sprites.find(tile.type);
            CGT_ASSERT(variantsIter != m_Sprites.end());
            cgt::render::SpriteDrawRequest sprite = variantsIter->second[tile.variant];
            sprite.position = glm::vec2(x, y * -1.0f);
            if (!tile.discovered)
            {
                sprite.colorTint = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            }
            queue.sprites.emplace_back(std::move(sprite));
        }
    }
}
