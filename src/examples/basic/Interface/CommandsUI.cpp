#include "CommandsUI.h"
#include <imgui.h>


CommandsUI::CommandsUI(std::shared_ptr<cgt::render::IRenderContext> render, CommandQueue* queue)
    : m_CommandQueue(queue)
    , m_Render(render)
{
    cgt::render::TextureHandle movesTexture = m_Render->LoadTexture("assets/examples/textures/Moves.png");

    m_AvailableCommands = {
        { CommandID::MoveForward, "Move Forward" },
        { CommandID::TurnLeft, "Turn Left" },
        { CommandID::TurnRight, "Turn Right" }
    };

    m_CommandUIPropsMap = {
        {CommandID::None, {movesTexture, {0.6, 0}, {0.8, 0.5}}},
        {CommandID::MoveForward, {movesTexture, {0, 0}, {0.2, 0.5}}},
        {CommandID::TurnLeft, {movesTexture, {0.4, 0}, {0.6, 0.5}}},
        {CommandID::TurnRight, {movesTexture, {0.2, 0}, {0.4, 0.5}}},
    };
}

void CommandsUI::Tick(const float dt)
{
    ImGui::SetNextWindowSize({ 600, 200 }, ImGuiCond_FirstUseEver);
    ImGui::Begin("Commands");

    DrawCommandQueue();
    ImGui::Separator();
    DrawCommandSelectButtons();

    if (ImGui::Button("Step"))
    {
        m_CommandQueue->StepForward();
    }

    ImGui::End();
}

void CommandsUI::DrawCommandQueue()
{
    const Command& current = m_CommandQueue->GetCurrent();
    for(int i = 0; i < m_CommandQueue->GetAll().size(); ++i)
    {
        const Command command = m_CommandQueue->Get(i);

        ImGui::PushID(i);

        DrawCommandImage(command.ID);
        if (i < m_CommandQueue->GetAll().size() - 1)
        {
            ImGui::SameLine();
        }

        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Command_ptr"))
            {
                const Command* payloadCommand = (const Command*)payload->Data;
                m_CommandQueue->Set(i, *payloadCommand);
            }

            ImGui::EndDragDropTarget();
        }

        ImGui::PopID();
    }
}

void CommandsUI::DrawCommandSelectButtons()
{
    for (int i = 0; i < m_AvailableCommands.size(); ++i)
    {
        const Command& command = m_AvailableCommands[i];

        ImGui::PushID(0xf0000 | i);
        if (DrawCommandImageButton(command.ID))
        {
            m_CommandQueue->Set(0, command);
        }

        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
        {
            ImGui::SetDragDropPayload("Command_ptr", &command, sizeof(command));
            DrawCommandImage(command.ID);
            ImGui::EndDragDropSource();
        }

        if (i < m_AvailableCommands.size() - 1)
        {
            ImGui::SameLine();
        }

        ImGui::PopID();
    }

    if (ImGui::Button("Clear Command Queue"))
    {
        m_CommandQueue->Reset();
    }
}

void CommandsUI::DrawCommandImage(CommandID id)
{
    const CommandUIProps& props = GetCommandUIProps(id);
    m_Render->ImGuiImage(props.Texture, props.Size, props.UV0, props.UV1);
}

bool CommandsUI::DrawCommandImageButton(CommandID id)
{
    const CommandUIProps& props = GetCommandUIProps(id);
    return m_Render->ImGuiImageButton(props.Texture, props.Size, props.UV0, props.UV1);
}
