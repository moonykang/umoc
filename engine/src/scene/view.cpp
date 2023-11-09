#include "view.h"
#include "rhi/buffer.h"
#include "rhi/context.h"
#include "rhi/defines.h"
#include "rhi/descriptor.h"

namespace scene
{
View::View() : type(Type::FirstPerson), dirty(true), uniformBuffer(nullptr)
{
}

View::~View()
{
}

Result View::init(platform::Context* platformContext)
{
    rhi::Context* context = reinterpret_cast<rhi::Context*>(platformContext);

    uniformBuffer = context->allocateUniformBuffer(uniformDataSize, &ubo);

    return Result::Continue;
}

void View::terminate(platform::Context* platformContext)
{
}

void View::setView(glm::vec3 position, glm::vec3 rotation)
{
    this->position = position;
    this->rotation = rotation;
}

void View::updateView()
{
    if (keyInput.any() || dirty)
    {
        glm::vec3 camFront;
        camFront.x = -cos(glm::radians(rotation.x)) * sin(glm::radians(rotation.y));
        camFront.y = sin(glm::radians(rotation.x));
        camFront.z = cos(glm::radians(rotation.x)) * cos(glm::radians(rotation.y));
        camFront = glm::normalize(camFront);

        if (keyInput.up)
        {
            position += camFront * moveSpped;
        }

        if (keyInput.down)
        {
            position -= camFront * moveSpped;
        }

        if (keyInput.left)
        {
            position -= glm::normalize(glm::cross(camFront, glm::vec3(0.0f, 1.0f, 0.0f))) * moveSpped;
        }

        if (keyInput.right)
        {
            position += glm::normalize(glm::cross(camFront, glm::vec3(0.0f, 1.0f, 0.0f))) * moveSpped;
        }

        updateViewMatrix();
    }
}

void View::updateViewMatrix()
{
    glm::mat4 rotM = glm::mat4(1.0f);
    glm::mat4 transM;

    rotM = glm::rotate(rotM, glm::radians(rotation.x * (flipY ? -1.0f : 1.0f)), glm::vec3(1.0f, 0.0f, 0.0f));
    rotM = glm::rotate(rotM, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    rotM = glm::rotate(rotM, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

    glm::vec3 translation = position;
    if (flipY) {
        translation.y *= -1.0f;
    }
    transM = glm::translate(glm::mat4(1.0f), translation);

    switch (type)
    {
    case Type::FirstPerson:
        ubo.view = rotM * transM;
        break;
    case Type::LookAt:
        ubo.view = transM * rotM;
        break;
    }

    ubo.prev_view_proj = ubo.view_proj;
    ubo.view_inverse = glm::inverse(ubo.view);
    ubo.proj_inverse = glm::inverse(ubo.proj);
    ubo.view_proj = ubo.proj * ubo.view;
    ubo.view_proj_inverse = glm::inverse(ubo.view_proj);
    ubo.view_pos = glm::vec4(position, 1.f);

    {
        std::lock_guard<std::mutex> lock(mutex);
        dirty = true;
    }
}

void View::setPerspective(float fov, float ratio, float minDepth, float maxDepth)
{
    ubo.proj = glm::perspective(glm::radians(fov), ratio, minDepth, maxDepth);
    ubo.nearPlane = minDepth;
    ubo.farPlane = maxDepth;

    if (flipY)
    {
        ubo.proj[1][1] *= -1.f;
    }

    // lock
    {
        std::lock_guard<std::mutex> lock(mutex);
        dirty = true;
    }
}

Result View::updateUniformBuffer(platform::Context* platformContext)
{
    updateView();

    std::lock_guard<std::mutex> lock(mutex);

    if (dirty)
    {
        rhi::Context* context = reinterpret_cast<rhi::Context*>(platformContext);
        try(uniformBuffer->update(context, uniformDataSize, &ubo));
        dirty = false;
    }

    return Result::Continue;
}

rhi::SubAllocatedBuffer* View::getUniformBuffer()
{
    return uniformBuffer;
}

void View::rotate(glm::vec3 delta)
{
    this->rotation += delta;
    updateViewMatrix();
}

void View::translate(glm::vec3 delta)
{
    this->position += delta;
    updateViewMatrix();
}

void View::handle_key_W(bool pressed)
{
    keyInput.up = pressed;
}
void View::handle_key_S(bool pressed)
{
    keyInput.down = pressed;
}
void View::handle_key_A(bool pressed)
{
    keyInput.left = pressed;
}
void View::handle_key_D(bool pressed)
{
    keyInput.right = pressed;
}

void View::handle_mouse_move(float x, float y)
{
    int32_t dx = static_cast<int32_t>(mouseCursorPos.x - x);
    int32_t dy = static_cast<int32_t>(mouseCursorPos.y - y);

    if (mouseButtonInput.right)
    {
        rotate(glm::vec3(dy * rotationSpeed, -dx * rotationSpeed, 0.f));
    }

    mouseCursorPos = glm::vec2(x, y);
}

void View::handle_mouse_LB(bool pressed)
{
    mouseButtonInput.left = pressed;
}

void View::handle_mouse_RB(bool pressed)
{
    mouseButtonInput.right = pressed;
}

void View::setType(Type type)
{
    this->type = type;
}
} // namespace scene
