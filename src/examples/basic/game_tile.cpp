#include <examples/basic/pch.h>

#include <examples/basic/game_tile.h>

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

    tmx_property* variantProp = tmx_get_property(tile.properties, "VARIANT");
    if (variantProp)
    {
        CGT_ASSERT(variantProp->type == tmx_property_type::PT_INT);
        newTile.variant = variantProp->value.integer;
        CGT_ASSERT(newTile.variant < MAX_VARIANTS);
    }

    if (match(tile.type, "START")) {
        newTile.type = Type::Start;
    } else if (match(tile.type, "GOAL")) {
        newTile.type = Type::Goal;
    } else if (match(tile.type, "GROUND")) {
        newTile.type = Type::Ground;
    } else if (match(tile.type, "CRACK")) {
        newTile.type = Type::Crack;
    } else if (match(tile.type, "WATER")) {
        newTile.type = Type::Water;
    } else if (match(tile.type, "SPIKES")) {
        newTile.type = Type::Spikes;
    } else if (match(tile.type, "BALL")) {
        newTile.type = Type::Ball;
    } else if (match(tile.type, "STAIR")) {
        newTile.type = Type::Stair;
    } else if (match(tile.type, "NOISE")) {
        newTile.type = Type::Noise;
    } else if (match(tile.type, "PICKUP")) {
        newTile.type = Type::Pickup;
    } else if (match(tile.type, "LATCH")) {
        newTile.type = Type::Latch;
    } else if (tile.type != nullptr) {
        CGT_PANIC("Unsupported tile type: {}", tile.type);
    }

    return newTile;
}