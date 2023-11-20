#include "light.h"
#include "rhi/buffer.h"
#include "rhi/context.h"
#include "rhi/descriptor.h"
#include "view.h"

namespace scene
{
Result Light::updateUI(scene::View* view)
{
    static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::ROTATE);
    static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);

    ImGui::Text("Directional Light");

    if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
        mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
    ImGui::SameLine();
    if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
        mCurrentGizmoOperation = ImGuizmo::ROTATE;
    ImGui::SameLine();
    if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
        mCurrentGizmoOperation = ImGuizmo::SCALE;

    ImGuizmo::DecomposeMatrixToComponents(&transform[0][0], &position.x, &vRotate.x, &vScale.x);
    ImGui::InputFloat3("Tr", &position.x);
    ImGui::InputFloat3("Rt", &vRotate.x);
    ImGui::InputFloat3("Sc", &vScale.x);
    ImGuizmo::RecomposeMatrixFromComponents(&position.x, &vRotate.x, &vScale.x, &transform[0][0]);

    if (mCurrentGizmoOperation != ImGuizmo::SCALE)
    {
        if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
            mCurrentGizmoMode = ImGuizmo::LOCAL;
        ImGui::SameLine();
        if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
            mCurrentGizmoMode = ImGuizmo::WORLD;
    }

    bool changed = false;

    ImGuiIO& io = ImGui::GetIO();
    ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
    changed |= ImGuizmo::Manipulate(&view->getViewMatrix()[0][0], &view->getProjectionMatrix()[0][0],
                                    mCurrentGizmoOperation, mCurrentGizmoMode, &transform[0][0], NULL, NULL);

    if (changed)
    {
        dirty = true;
    }

    return Result::Continue;
}

bool DirectionalLight::updateLightData(LightData& lightData, glm::mat4& lightMatrix)
{
    // if (dirty)
    {
        // auto& m = get();

        // auto inv = glm::inverse(m);
        //  glm::vec3 direction = glm::normalize(glm::mat3(m) * glm::vec3(0.0f, 0.0f, 1.0f));
        //  glm::vec3 position = glm::vec3(m[3][0], m[3][1], m[3][2]);
        glm::vec3 lookat = glm::vec3(0, 0, 0);

        glm::vec3 direction = glm::normalize(lookat - position);
        lightData.set_light_type(LightType::LIGHT_TYPE_DIRECTIONAL);
        lightData.set_light_position(position);
        lightData.set_light_direction(direction);
        lightData.set_light_color(color);

        glm::mat4 viewMatrix = glm::lookAt(position, position + direction, glm::vec3(0.0f, 1.0f, 0.0f));
        // glm::mat4 viewMatrix = glm::lookAt(position, lookat, glm::vec3(0.0f, 1.0f, 0.0f));
        lightMatrix = projection * viewMatrix;

        dirty = false;

        return true;
    }

    return false;
}

void DirectionalLight::setProjection(float fov, float aspect, float near, float far)
{
    // projection = glm::ortho(-10.f, 10.f, -10.f, 10.f, near, far);
    projection = glm::perspective(glm::radians(fov), aspect, near, far);
    projection[1][1] *= -1;
    dirty = true;
}

Lights::Lights() : uniformBuffer(nullptr), dirty(false)
{
    ubo.numLights = 1;
    ubo.lightMatrix = glm::mat4(1.0f);

    for (int i = 0; i < NUM_LIGHTS; i++)
    {
        ubo.light[i] = LightData();
    }
}

Result Lights::init(platform::Context* platformContext)
{
    rhi::Context* context = reinterpret_cast<rhi::Context*>(platformContext);

    uniformBuffer = context->allocateUniformBuffer(uniformDataSize, &ubo);

    return Result::Continue;
}

void Lights::terminate(platform::Context* platformContext)
{
}

void Lights::setLightPosition(uint32_t index, const glm::vec3& position)
{
    ASSERT(index < NUM_LIGHTS);
    ubo.light[index].set_light_position(position);
    dirty = true;
}

void Lights::setLightColor(uint32_t index, const glm::vec3& color)
{
    ASSERT(index < NUM_LIGHTS);
    ubo.light[index].set_light_color(color);
    dirty = true;
}

void Lights::setLightRadius(uint32_t index, const float& radius)
{
    ASSERT(index < NUM_LIGHTS);
    ubo.light[index].set_light_radius(radius);
    dirty = true;
}

Result Lights::updateUniformBuffer(platform::Context* platformContext)
{
    // updateView();

    std::lock_guard<std::mutex> lock(mutex);

    dirty |= directionalLight.updateLightData(ubo.light[0], ubo.lightMatrix);

    for (auto& light : pointLights)
    {
    }
    if (dirty)
    {
        if (false)
        {
            LOGD("Debug light ubo, numLights %u, uniformDataSize %zu", ubo.numLights, uniformDataSize);

            for (uint32_t i = 0; i < NUM_LIGHTS; i++)
            {
                auto light = ubo.light[i];
                LOGD("Light%u", i);
                LOGD("%f %f %f %f", light.data0.x, light.data0.y, light.data0.z, light.data0.w);
                LOGD("%f %f %f %f", light.data1.x, light.data1.y, light.data1.z, light.data1.w);
                LOGD("%f %f %f %f", light.data2.x, light.data2.y, light.data2.z, light.data2.w);
                LOGD("%f %f %f %f", light.data3.x, light.data3.y, light.data3.z, light.data3.w);
            }
        }
        rhi::Context* context = reinterpret_cast<rhi::Context*>(platformContext);
        try(uniformBuffer->update(context, uniformDataSize, &ubo));
        dirty = false;
    }

    return Result::Continue;
}

rhi::SubAllocatedBuffer* Lights::getUniformBuffer()
{
    return uniformBuffer;
}

void Lights::setLightNumber(uint32_t val)
{
    ubo.numLights = val;
    dirty = true;
}

Result Lights::updateUI(scene::View* view)
{
    try(directionalLight.updateUI(view));

    return Result::Continue;
}
} // namespace scene
