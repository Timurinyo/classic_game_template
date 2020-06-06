#pragma once

struct GameTile
{
    enum class Type
    {
        Undefined,
        Start,
        Goal,
        Ground,
        Crack,
        Latch,
        Spikes,
        Ball,
        Stair,
        Noise,
        Water,
        Pickup,
    };

    static constexpr u32 MAX_VARIANTS = 10;

    Type type = Type::Undefined;
    u32 variant = 0;

    static GameTile FromTMXTile(tmx_tile& tile);
};