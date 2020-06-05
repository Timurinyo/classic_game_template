#include <examples/basic/pch.h>

#include <examples/basic/game_grid.h>

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

    auto tiledMap = cgt::LoadTiledMap("assets/examples/maps/sample_iso.tmx");
    cgt::Tilemap tiledMapRenderer(tiledMap, *render, "assets/examples/maps");
    GameGrid gameGrid(tiledMap);

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
            ImGui::End();
        }

        renderQueue.Reset();
        renderQueue.clearColor = glm::vec4(1.0f, 0.3f, 1.0f, 1.0f);

        tiledMapRenderer.Render(renderQueue);

        renderStats = render->Submit(renderQueue, camera);
    }

    tmx_map_free(tiledMap);
    return 0;
}