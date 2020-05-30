#include "cstdio"

#include <engine/system.h>
#include <engine/external_libs.h>
#include <engine/window.h>

#include <render_core/i_render_context.h>

int GameMain()
{
    auto windowCfg = cgt::WindowConfig();
    windowCfg.title = "Basic Example";
    auto window = cgt::Window::BuildWithConfig(windowCfg);

    auto renderCfg = cgt::render::RenderConfig(window);
    auto render = cgt::render::IRenderContext::BuildWithConfig(renderCfg);

    SDL_Event event {};
    bool quitRequested = false;
    while (!quitRequested)
    {
        while (window->PollEvent(event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                quitRequested = true;
                break;
            }
        }

        render->Submit();
    }

    return 0;
}