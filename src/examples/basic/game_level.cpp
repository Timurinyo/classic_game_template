#include <examples/basic/pch.h>

#include <examples/basic/game_level.h>

void GameLevel::Load(cgt::render::IRenderContext& render, const char* path, const char* basePath)
{
    tmx_map* map = cgt::LoadTiledMap(path);

    description.clear();
    tmx_property* descriptionProperty = tmx_get_property(map->properties, "DESCRIPTION");
    if (descriptionProperty)
    {
        CGT_ASSERT(descriptionProperty->type == tmx_property_type::PT_STRING)
        description = descriptionProperty->value.string;
    }

    for (tmx_layer* layer = map->ly_head; layer; layer = layer->next)
    {
        if (strcmp(layer->name, "BASE") == 0)
        {
            tmx_property* discoveredProperty = tmx_get_property(layer->properties, "DISCOVERED");
            bool isMapDiscovered = false; 
            if (discoveredProperty)
            {
                isMapDiscovered = discoveredProperty->value.boolean;
            }

            gameGrid = std::make_unique<GameGrid>(map, layer, isMapDiscovered);
        }
        else if (strcmp(layer->name, "PICKUPS") == 0)
        {
            tmx_property* discoveredProperty = tmx_get_property(layer->properties,"DISCOVERED");
            bool isMapDiscovered = false;
            if (discoveredProperty)
            {
                isMapDiscovered = discoveredProperty->value.boolean;
            }

            pickupGrid = std::make_unique<GameGrid>(map, layer, isMapDiscovered);
        }
    }
    CGT_ASSERT_ALWAYS(gameGrid.get() && pickupGrid.get());

    tileset = Tileset::LoadFrom(render, map, map->ts_head->tileset, basePath);


    tmx_map_free(map);
}
