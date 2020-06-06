#pragma once

#include <vector>
#include <map>

#include <examples/basic/CommandQueue.h>

#include <engine/external_libs.h>
#include <engine/system.h>
#include <render_core/i_render_context.h>

#include <imgui.h>


struct CommandUIProps
{
    cgt::render::TextureHandle Texture;
    ImVec2 UV0 = { 0, 0 };
    ImVec2 UV1 = { 1, 1 };
    const char* Description; //To show as tooltips
    ImVec2 Size = { 64, 64 };
    // tint/border color
};


class CommandsUI
{
public:
    CommandsUI(std::shared_ptr<cgt::render::IRenderContext> render, CommandQueue* queue, u32 windowWidth, u32 windowHeight);
    void Tick(const float dt);

private:
    void DrawCommandQueue();
    void DrawCommandSelectButtons();
    void DrawCommandImage(const Command& command, bool isHighlighted = false);
    bool DrawCommandImageButton(const Command& command);

    CommandUIProps GetCommandUIProps(const Command& command) const;

    std::vector<Command> m_AvailableCommands = {};

    CommandQueue* m_CommandQueue = nullptr;

    std::shared_ptr<cgt::render::IRenderContext> m_Render = nullptr;

    std::unordered_map<CommandID, CommandUIProps> m_CommandUIPropsMap;

    const ImVec2 m_WindowSize;
};
