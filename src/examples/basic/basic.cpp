#include <examples/basic/pch.h>

#include <examples/basic/game_grid.h>
#include <examples/basic/tileset.h>

#include "CommandQueue.h"
#include "Interface/CommandsUI.h"

#include <examples/basic/BasicPlayer.h>

void ImguiDebugRenderPlayerStats(BasicPlayer& player, GameGrid& gameGrid, CommandQueue& commandQueue)
{
    const glm::vec2 playerPosition = player.GetPosition();
    const GameTile currentTile = gameGrid.At(playerPosition.x,-playerPosition.y);

    ImGui::SetNextWindowSize({300,100},ImGuiCond_FirstUseEver);
    ImGui::Begin("Player Stats");

    ImGui::Text("x: %f y: %f",playerPosition.x,playerPosition.y);
    ImGui::Text("Player State: %s",PlayerStateToString(player.GetPlayerState()));
    ImGui::Text("Current tile: %d",static_cast<int>(currentTile.type));

    if(player.GetPlayerState() == PlayerStateID::Idle && currentTile.type == GameTile::Type::Goal)
    {
        ImGui::Text("You Won!");
    }

    if(player.GetPlayerState() == PlayerStateID::Dying && ImGui::Button("Respawn"))
    {
        commandQueue.Reset();
        player.Spawn(gameGrid);
    }

    ImGui::End();
}

int GameMain()
{
    auto windowCfg = cgt::WindowConfig();
    windowCfg.title = "Basic Example";
    auto window = cgt::Window::BuildWithConfig(windowCfg);

    auto renderCfg = cgt::render::RenderConfig(window);
    auto render = cgt::render::IRenderContext::BuildWithConfig(renderCfg);

    cgt::render::RenderQueue renderQueue;
    cgt::render::CameraSimpleOrtho camera;
    camera.windowWidth = window->GetWidth();
    camera.windowHeight = window->GetHeight();
    camera.pixelsPerUnit = 87.0f;



    bool shouldGo = false;
    BasicPlayer player("assets/examples/textures/player", *render);

    CommandQueue commandQueue;
    CommandsUI commandsInterface(render, &commandQueue);

    std::tuple<const char*, bool> levelList[] =
        {
            { "assets/examples/maps/level_00.tmx", true },
            { "assets/examples/maps/sample_iso.tmx", false },
        };
    usize currentLevelIdx = 0;

    std::unique_ptr<GameGrid> gameGrid;
    std::unique_ptr<GameGrid> pickupGrid;
    std::unique_ptr<Tileset> tileset;
    bool shouldReloadLevel = true;

    cgt::Clock frameClock;
    SDL_Event event {};
    cgt::render::RenderStats renderStats{};
    bool quitRequested = false;
    while (!quitRequested)
    {
        const float dt = frameClock.Tick();

        render->NewFrame();
        window->NewFrame();

        while (window->PollEvent(event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                quitRequested = true;
                break;
            }
        }

        renderQueue.Reset();
        renderQueue.clearColor = glm::vec4(.3f, .3f, .4f, 1.0f);

        {
            if (shouldReloadLevel)
            {
                shouldReloadLevel = false;

                auto currentLevelPath = std::get<0>(levelList[currentLevelIdx]);
                auto currentLevelDiscovered = std::get<1>(levelList[currentLevelIdx]);

                auto tiledMap = cgt::LoadTiledMap(currentLevelPath);

                for (tmx_layer* layer = tiledMap->ly_head; layer; layer = layer->next)
                {
                    if (strcmp(layer->name, "BASE") == 0)
                    {
                        gameGrid = std::make_unique<GameGrid>(tiledMap, layer, currentLevelDiscovered);
                    }
                    else if (strcmp(layer->name, "PICKUPS") == 0)
                    {
                        pickupGrid = std::make_unique<GameGrid>(tiledMap, layer, currentLevelDiscovered);
                    }
                }
                CGT_ASSERT_ALWAYS(gameGrid.get() && pickupGrid.get());

                tileset = Tileset::LoadFrom(*render, tiledMap, tiledMap->ts_head->tileset, "assets/examples/maps");

                player.Spawn(*gameGrid);

                tmx_map_free(tiledMap);
            }

            glm::vec2 cameraMoveInput(0.0f);
            const u8* keyboard = SDL_GetKeyboardState(nullptr);
            if (keyboard[SDL_SCANCODE_A])
            {
                cameraMoveInput.x -= 1.0f;
            }
            if (keyboard[SDL_SCANCODE_D])
            {
                cameraMoveInput.x += 1.0f;
            }
            if (keyboard[SDL_SCANCODE_S])
            {
                cameraMoveInput.y -= 1.0f;
            }
            if (keyboard[SDL_SCANCODE_W])
            {
                cameraMoveInput.y += 1.0f;
            }

            const float cameraMoveInputMagnitudeSqr = glm::dot(cameraMoveInput, cameraMoveInput);
            if (cameraMoveInputMagnitudeSqr > 0.0f)
            {
                cameraMoveInput = cameraMoveInput / sqrt(cameraMoveInputMagnitudeSqr);
                const glm::vec2 isometricCorrection(1.0f, 2.0f);
                cameraMoveInput = cameraMoveInput * isometricCorrection;

                const glm::mat4 isometricRotation = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
                cameraMoveInput = isometricRotation * glm::vec4(cameraMoveInput, 0.0f, 0.0f);

                const float cameraMoveSpeed = 5.0f;
                camera.position += cameraMoveInput * cameraMoveSpeed * dt;
            }

            {
                ImGui::SetNextWindowSize({200, 80}, ImGuiCond_FirstUseEver);
                ImGui::Begin("Render Stats");
                ImGui::Text("Frame time: %.2fms", dt * 1000.0f);
                ImGui::Text("Sprites: %d", renderStats.spriteCount);
                ImGui::Text("Drawcalls: %d", renderStats.drawcallCount);
                //ImGui::Text("Drawcalls: %d", renderStats.drawcallCount);
                ImGui::End();
            }

            commandsInterface.Tick(dt);

            tileset->RenderGameGrid(*gameGrid, renderQueue);
            tileset->RenderGameGrid(*pickupGrid, renderQueue);

            const Command currentCommand = commandQueue.GetCurrent();

            if (player.GetPlayerState() == PlayerStateID::Idle && commandQueue.GetState() == State::Execution)
            {
                GameTile::Type previousViewedTile = player.GetViewedTile(*gameGrid);
                player.Execute(currentCommand.ID, *gameGrid);
                GameTile::Type currentViewedTile = player.GetViewedTileNext(*gameGrid);
                commandQueue.StepForward(previousViewedTile, currentViewedTile);
            }
            else if(commandQueue.GetState() == State::Finished && player.GetPlayerState() != PlayerStateID::ReachedGoal)
            {
                commandQueue.Reset();
                commandQueue.SetState(State::Execution);

                currentLevelIdx = ++currentLevelIdx % SDL_arraysize(levelList);
                shouldReloadLevel = true;
            }
            else if (player.GetPlayerState() == PlayerStateID::Dead || commandQueue.GetState() == State::NeedRestart)
            {
                commandQueue.Reset();
                player.Spawn(*gameGrid);
                gameGrid->UndiscoverAllTiles();
            }

            player.Update(dt);

            if (player.GetPlayerState() == PlayerStateID::Dying)
            {
                commandQueue.Reset();
            }

            player.Render(renderQueue);

            ImguiDebugRenderPlayerStats(player, *gameGrid, commandQueue);
        }

        renderStats = render->Submit(renderQueue, camera);
    }

    return 0;
}