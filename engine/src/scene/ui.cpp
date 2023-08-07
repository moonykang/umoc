#include "ui.h"
#include "platform/input.h"
#include "rhi/context.h"
#include "scene/light.h"
#include "scene/scene.h"
#include "scene/view.h"

namespace scene
{
UI::UI() : enabled(true)
{
}

Result UI::startRender(platform::Context* platformContext, SceneInfo* sceneInfo)
{
    rhi::Context* context = platformContext->getRHI();

    auto sceneLights = sceneInfo->getLight();
    auto sceneView = sceneInfo->getView();

    try(renderSceneView(sceneView));
    try(renderSceneLight(sceneLights));

    auto frameTimer = platformContext->getTimer().getCurrentFrameTimer();

    auto input = platformContext->getWindow()->getInput();
    auto mousePos = input->getPosition();
    auto mouseStatus = input->getMouseStatus();

    ImGuiIO& io = ImGui::GetIO();
    io.DeltaTime = frameTimer;
    io.MousePos = ImVec2(mousePos.first, mousePos.second);
    io.MouseDown[0] = mouseStatus.left;
    io.MouseDown[1] = mouseStatus.right;
    io.MouseDown[2] = mouseStatus.middle;

    ImGui::NewFrame();

    ImGui::SetWindowPos(ImVec2(20, 20), ImGuiCond_::ImGuiCond_FirstUseEver);
    ImGui::SetWindowSize(ImVec2(300, 300), ImGuiCond_::ImGuiCond_Always);
    ImGui::TextUnformatted("Hello");
    ImGui::TextUnformatted("WTF");
    ImGui::Text("Camera");

    {
        ImGui::Begin("Another window");
        ImGui::Text("Hello from another window!");
        ImGui::End();
    }
    ImGui::ShowDemoWindow();

    return Result::Continue;
}

Result UI::renderSceneLight(Lights* lights)
{

    return Result::Continue;
}

Result UI::renderSceneView(View* view)
{

    return Result::Continue;
}

Result UI::endRender()
{
    ImGui::Render();
    return Result::Continue;
}
} // namespace scene