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
        { CommandID::TurnRight, "Turn Right" },
        { CommandID::Repeat, "x2", 2, 2 },
        { CommandID::Repeat, "x3", 3, 3 },
        { CommandID::Repeat, "x4", 4, 4 },
        { CommandID::Repeat, "x5", 5, 5 },
    };

    m_CommandUIPropsMap = {
        { CommandID::None,        { movesTexture, {0.6, 0.0}, {0.8, 0.5} } },
        { CommandID::MoveForward, { movesTexture, {0.0, 0.0}, {0.2, 0.5} } },
        { CommandID::TurnLeft,    { movesTexture, {0.4, 0.0}, {0.6, 0.5} } },
        { CommandID::TurnRight,   { movesTexture, {0.2, 0.0}, {0.4, 0.5} } },
        { CommandID::Repeat,      { movesTexture, {0.0, 0.5}, {0.2, 1.0} } },
    };
}

void CommandsUI::Tick(const float dt)
{
    ImGui::SetNextWindowSize({ 600, 250 }, ImGuiCond_FirstUseEver);
    ImGui::Begin("Commands");

    DrawCommandQueue();
    ImGui::Separator();
    DrawCommandSelectButtons();

    if (ImGui::Button("Step"))
    {
        m_CommandQueue->StepForward();
    }

    if (ImGui::Button("Start!"))
    {
        m_CommandQueue->SetState(State::Execution);
    }

    if (ImGui::Button("Stop"))
    {
        m_CommandQueue->SetState(State::NeedRestart);
    }

    if(ImGui::Button("Pause"))
    {
        m_CommandQueue->SetState(State::Pause);
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

        DrawCommandImage(command);
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

    ImGui::Text("Current command idx: %d", m_CommandQueue->GetCurrentIdx());
}

void CommandsUI::DrawCommandSelectButtons()
{
    for (int i = 0; i < m_AvailableCommands.size(); ++i)
    {
        const Command& command = m_AvailableCommands[i];

        ImGui::PushID(0xf0000 | i);
        DrawCommandImageButton(command);

        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
        {
            ImGui::SetDragDropPayload("Command_ptr", &command, sizeof(command));
            DrawCommandImage(command);
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
        m_CommandQueue->Erase();
    }
}

void CommandsUI::DrawCommandImage(const Command& command)
{
    const CommandUIProps& props = GetCommandUIProps(command);
    m_Render->ImGuiImage(props.Texture, props.Size, props.UV0, props.UV1);
}

bool CommandsUI::DrawCommandImageButton(const Command& command)
{
    const CommandUIProps& props = GetCommandUIProps(command);
    return m_Render->ImGuiImageButton(props.Texture, props.Size, props.UV0, props.UV1);
}

CommandUIProps CommandsUI::GetCommandUIProps(const Command& command) const
{
    const CommandUIProps& props = m_CommandUIPropsMap.at(command.ID);
    if (command.ID != CommandID::Repeat)
    {
        return props;
    }
    float uvDeltaX = props.UV1.x - props.UV0.x;
    if (!command.CurrentRepeats)
    {
        return { props.Texture, {0.8, 0.0}, {1.0, 0.5}, props.Size };
    }
    ImVec2 UV0(uvDeltaX * (command.CurrentRepeats - 1), props.UV0.y);
    ImVec2 UV1(uvDeltaX *  command.CurrentRepeats     , props.UV1.y);
    return { props.Texture, UV0, UV1, props.Size };
}
