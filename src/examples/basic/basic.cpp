#include <examples/basic/pch.h>

#include <examples/basic/game_grid.h>
#include <examples/basic/tileset.h>
#include <examples/basic/game_level.h>

#include "CommandQueue.h"
#include "Interface/CommandsUI.h"

#include <examples/basic/BasicPlayer.h>
#include <examples/basic/PoorsManStateMachine.h>

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

void HandleCameraControl(cgt::render::CameraSimpleOrtho &camera, const float dt)
{
    glm::vec2 cameraMoveInput(0.0f);
    const u8* keyboard = SDL_GetKeyboardState(nullptr);
    if(keyboard[SDL_SCANCODE_A])
    {
        cameraMoveInput.x -= 1.0f;
    }
    if(keyboard[SDL_SCANCODE_D])
    {
        cameraMoveInput.x += 1.0f;
    }
    if(keyboard[SDL_SCANCODE_S])
    {
        cameraMoveInput.y -= 1.0f;
    }
    if(keyboard[SDL_SCANCODE_W])
    {
        cameraMoveInput.y += 1.0f;
    }

    const float cameraMoveInputMagnitudeSqr = glm::dot(cameraMoveInput,cameraMoveInput);
    if(cameraMoveInputMagnitudeSqr > 0.0f)
    {
        cameraMoveInput = cameraMoveInput / sqrt(cameraMoveInputMagnitudeSqr);
        const glm::vec2 isometricCorrection(1.0f,2.0f);
        cameraMoveInput = cameraMoveInput * isometricCorrection;

        const glm::mat4 isometricRotation = glm::rotate(glm::mat4(1.0f),glm::radians(45.0f),glm::vec3(0.0f,0.0f,1.0f));
        cameraMoveInput = isometricRotation * glm::vec4(cameraMoveInput,0.0f,0.0f);

        const float cameraMoveSpeed = 5.0f;
        camera.position += cameraMoveInput * cameraMoveSpeed * dt;
    }
}

void ImGuiRenderStats(const float dt,cgt::render::RenderStats &renderStats)
{
    ImGui::SetNextWindowSize({200,80},ImGuiCond_FirstUseEver);
    ImGui::Begin("Render Stats");
    ImGui::Text("Frame time: %.2fms",dt * 1000.0f);
    ImGui::Text("Sprites: %d",renderStats.spriteCount);
    ImGui::Text("Drawcalls: %d",renderStats.drawcallCount);
    ImGui::End();
}

void ImGuiShowLevelWelcomeText(std::shared_ptr<cgt::Window> window, std::string& currentLevelDescription, GameStateKeeper& gameStateKeeper)
{
    const int imguiWindowWidth = 300;
    const int imguiWindowHeight = 150;
    ImGui::SetNextWindowSize({imguiWindowWidth,imguiWindowHeight});//, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos({static_cast<float>(window->GetWidth() * 0.5 - imguiWindowWidth * 0.5),static_cast<float>(window->GetHeight() * 0.5 - imguiWindowHeight * 0.5)});
    bool showWindow = true;
    ImGui::Begin("Level description",&showWindow,ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
    ImGui::PushTextWrapPos(ImGui::GetFontSize() * 20.f);
    ImGui::Text(currentLevelDescription.c_str());
    ImGui::PopTextWrapPos();
    if(ImGui::Button("Start!"))
    {
        gameStateKeeper.SetState(GameState::InGame);
    }
    ImGui::End();
}

void ImGuiShowDrownMessage(std::shared_ptr<cgt::Window> window, GameStateKeeper& gameStateKeeper)
{
    const int imguiWindowWidth = 300;
    const int imguiWindowHeight = 150;
    ImGui::SetNextWindowSize({imguiWindowWidth,imguiWindowHeight});
    ImGui::SetNextWindowPos({static_cast<float>(window->GetWidth() * 0.5 - imguiWindowWidth * 0.5),static_cast<float>(window->GetHeight() * 0.5 - imguiWindowHeight * 0.5)});
    bool showWindow = true;
    ImGui::Begin("Level description",&showWindow,ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
    ImGui::PushTextWrapPos(ImGui::GetFontSize() * 20.f);
    ImGui::Text("Our witch can't swim! Try to avoid water.");
    ImGui::PopTextWrapPos();
    if(ImGui::Button("OK!"))
    {
        gameStateKeeper.SetState(GameState::InGame);
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

    auto mixOpenResult = Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 2048);
    CGT_ASSERT_MSG(mixOpenResult >= 0, "Failed to initialize SDL mixer!");

    auto* music = cgt::LoadMusic("assets/examples/sounds/music.wav");
    Mix_VolumeMusic(80);
    Mix_PlayMusic(music, -1);

    cgt::render::RenderQueue renderQueue;
    cgt::render::CameraSimpleOrtho camera;
    camera.windowWidth = window->GetWidth();
    camera.windowHeight = window->GetHeight();
    camera.pixelsPerUnit = 87.0f;

    GameLevel level;
    bool shouldLoadLevel = true;
    const char* levelList[] =
        {
            "assets/examples/maps/level_01.tmx",
            "assets/examples/maps/level_02.tmx",
            "assets/examples/maps/level_03.tmx",
            "assets/examples/maps/level_00.tmx",
        };
    usize currentLevelIdx = 0;

    BasicPlayer player("assets/examples/textures/player", *render);

    GameStateKeeper gameStateKeeper;

    CommandQueue commandQueue;
    CommandsUI commandsInterface(render, &commandQueue, window->GetWidth(), window->GetHeight());


    cgt::Clock frameClock;
    SDL_Event event {};
    cgt::render::RenderStats renderStats{};
    bool quitRequested = false;
    while (!quitRequested)
    {
        if (shouldLoadLevel)
        {
            shouldLoadLevel = false;
            auto levelPath = levelList[currentLevelIdx];
            level.Load(*render, levelPath, "assets/examples/maps");

            player.Spawn(*level.gameGrid);
            commandQueue.Reset();
            gameStateKeeper.SetState(GameState::LevelStart);
        }

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

        HandleCameraControl(camera, dt);

#ifdef DEBUG
        ImGuiRenderStats(dt, renderStats);
#endif // DEBUG


        commandsInterface.Tick(dt);

        switch(gameStateKeeper.GetState())
        {
        case GameState::LevelStart:
        {
            ImGuiShowLevelWelcomeText(window, level.description, gameStateKeeper);
            break;
        }
        case GameState::Drown:
        {
            ImGuiShowDrownMessage(window, gameStateKeeper);
            break;
        }
        default:
            break;
        }

        renderQueue.Reset();
        renderQueue.clearColor = glm::vec4(1.0f, 0.3f, 1.0f, 1.0f);

        level.tileset->RenderGameGrid(*level.gameGrid, renderQueue);
        level.tileset->RenderGameGrid(*level.pickupGrid, renderQueue);

        const Command currentCommand = commandQueue.GetCurrent();

        if (player.GetPlayerState() == PlayerStateID::Idle && commandQueue.GetState() == State::Execution)
        {
            GameTile::Type previousViewedTile = player.GetViewedTile(*level.gameGrid);
            player.Execute(currentCommand.ID, *level.gameGrid);
            GameTile::Type currentViewedTile = player.GetViewedTileNext(*level.gameGrid);
            commandQueue.StepForward(previousViewedTile, currentViewedTile);
        }
        else if(commandQueue.GetState() == State::Finished && player.GetPlayerState() != PlayerStateID::ReachedGoal)
        {
            commandQueue.Reset();
            commandQueue.SetState(State::Execution);
        }
        else if (player.GetPlayerState() == PlayerStateID::Dead)
        {
            commandQueue.Reset();
            player.Spawn(*level.gameGrid);
            level.gameGrid->UndiscoverAllTiles();
            gameStateKeeper.SetState(GameState::Drown);
        }
        else if (commandQueue.GetState() == State::NeedRestart)
        {
            commandQueue.Reset();
            player.Spawn(*level.gameGrid);
            level.gameGrid->UndiscoverAllTiles();
        }
        else if (player.GetPlayerState() == PlayerStateID::ReachedGoal)
        {
            const int imguiWindowWidth = 180;
            const int imguiWindowHeight = 60;
            ImGui::SetNextWindowSize({imguiWindowWidth, imguiWindowHeight});//, ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowPos({static_cast<float>(window->GetWidth() * 0.5 - imguiWindowWidth * 0.5), static_cast<float>(window->GetHeight() * 0.5 - imguiWindowHeight * 0.5)});
            bool showWindow = true;
            ImGui::Begin("You passed the level!", &showWindow, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
            // TODO: add info on blocks used
            if (ImGui::Button("Next!"))
            {
                shouldLoadLevel = true;
                currentLevelIdx = ++currentLevelIdx % SDL_arraysize(levelList);
            }
            ImGui::End();
        }

        player.Update(dt);

        if (player.GetPlayerState() == PlayerStateID::Dying)
        {
            commandQueue.Reset();
        }

        player.Render(renderQueue);

#ifdef DEBUG
        ImguiDebugRenderPlayerStats(player, *level.gameGrid, commandQueue);
#endif // DEBUG


        renderStats = render->Submit(renderQueue, camera);
    }

    Mix_FreeMusic(music);

    return 0;
}