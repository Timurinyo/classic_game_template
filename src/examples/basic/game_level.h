#pragma once

#include <examples/basic/game_grid.h>
#include <examples/basic/tileset.h>

class GameLevel
{
public:
    void Load(cgt::render::IRenderContext& render, const char* path, const char* basePath);

    std::string description;

    std::unique_ptr<Tileset> tileset;
    std::unique_ptr<GameGrid> gameGrid;
    std::unique_ptr<GameGrid> pickupGrid;
};