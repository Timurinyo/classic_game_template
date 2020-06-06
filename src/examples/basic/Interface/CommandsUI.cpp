#include "CommandsUI.h"
#include <imgui.h>


CommandsUI::CommandsUI(std::shared_ptr<cgt::render::IRenderContext> render,
    CommandQueue* queue, u32 windowWidth, u32 windowHeight)
    : m_CommandQueue(queue)
    , m_Render(render)
    , m_WindowSize(windowWidth, windowHeight)
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
        { CommandID::DoWhile, "Do While" },
        { CommandID::None, "Do Nothing" }
    };

    m_CommandUIPropsMap = {
        { CommandID::None,        { movesTexture, {0.3, 0.00}, {0.4, 0.25}, "Do nothing..." } },
        { CommandID::MoveForward, { movesTexture, {0.0, 0.00}, {0.1, 0.25}, "Move one cell forward" } },
        { CommandID::TurnLeft,    { movesTexture, {0.2, 0.00}, {0.3, 0.25}, "Turn counterclockwise 90 degrees" } },
        { CommandID::TurnRight,   { movesTexture, {0.1, 0.00}, {0.2, 0.25}, "Turn clockwise 90 degrees" } },
        { CommandID::Repeat,      { movesTexture, {0.0, 0.25}, {0.1, 0.50}, "Repeat previous command X times" } },
        { CommandID::DoWhile,     { movesTexture, {0.5, 0.00}, {0.6, 0.25}, "Repeat previous command until tile in front changes" } } 
    };
}

void CommandsUI::Tick(const float dt)
{
    DrawCommandQueue();
    DrawCommandSelectButtons();
    
    bool windowOpen = true;
    ImGui::SetNextWindowPos({ 90, 90 });
    ImGui::SetNextWindowSize({ 310, 22 });
    ImGui::Begin("Controls", &windowOpen,
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoMove
    );

    if (ImGui::Button("Clear Command Queue"))
    {
        m_CommandQueue->Erase();
    }
    ImGui::SameLine();

    if (ImGui::Button("Start!"))
    {
        m_CommandQueue->SetState(State::Execution);
    }
    ImGui::SameLine();

    if (ImGui::Button("Stop"))
    {
        m_CommandQueue->SetState(State::NeedRestart);
    }
    ImGui::SameLine();

    if(ImGui::Button("Pause"))
    {
        m_CommandQueue->SetState(State::Pause);
    }
    ImGui::SameLine();

    ImGui::End();
}

void CommandsUI::DrawCommandQueue()
{
    bool windowOpen = true;
    float queueWidth = 90;
    ImGui::SetNextWindowSize({ queueWidth, m_WindowSize.y });
    ImGui::SetNextWindowPos({ 0, 0 });
    ImGui::Begin("Command Queue", &windowOpen,
        ImGuiWindowFlags_AlwaysVerticalScrollbar |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize
    );
    bool shouldHighlight = false;
    for(int i = 0; i < m_CommandQueue->GetAll().size(); ++i)
    {
        const Command command = m_CommandQueue->Get(i);

        ImGui::PushID(i);

        if (i == m_CommandQueue->GetCurrentIdx())
        {
            shouldHighlight = true;
        }
        else if (command.ID != CommandID::Repeat && command.ID != CommandID::DoWhile)
        {
            shouldHighlight = false;
        }

        DrawCommandImage(command, shouldHighlight);
        ImGui::NewLine();

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
    ImGui::End();
}

void CommandsUI::DrawCommandSelectButtons()
{
    bool windowOpen = true;
    ImGui::SetNextWindowSize({ static_cast<float>(82 * m_AvailableCommands.size()), 90 });
    ImGui::SetNextWindowPos({ 90, 0 });
    ImGui::Begin("Commands", &windowOpen,
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_AlwaysAutoResize
    );
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
        ImGui::SameLine();

        ImGui::PopID();
    }

    ImGui::End();
}

void CommandsUI::DrawCommandImage(const Command& command, bool isHighlighted)
{
    const CommandUIProps& props = GetCommandUIProps(command);
    ImVec2 UV0 = props.UV0;
    ImVec2 UV1 = props.UV1;
    if (isHighlighted)
    {
        UV0.y += 0.5;
        UV1.y += 0.5;
    }
    m_Render->ImGuiImage(props.Texture, props.Size, UV0, UV1, ImVec4(1, 1, 1, 1), "Drag&Drop commands here");
}

bool CommandsUI::DrawCommandImageButton(const Command& command)
{
    const CommandUIProps& props = GetCommandUIProps(command);
    return m_Render->ImGuiImageButton(props.Texture, props.Size, props.UV0, props.UV1, props.Description);
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
        return { props.Texture, {0.4, 0.25}, {0.5, 0.50}, props.Description, props.Size };
    }
    ImVec2 UV0(uvDeltaX * (command.CurrentRepeats - 1), props.UV0.y);
    ImVec2 UV1(uvDeltaX *  command.CurrentRepeats     , props.UV1.y);
    return { props.Texture, UV0, UV1, props.Description, props.Size };
}
