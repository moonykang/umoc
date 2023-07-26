#pragma once
#include "common/transform.h"
#include "common/util.h"
#include <mutex>

namespace rhi
{
class UniformBuffer;
class DescriptorSet;
} // namespace rhi

namespace platform
{
class Context;
}

namespace scene
{
class View
{
  public:
    enum class Type
    {
        LookAt,
        FirstPerson
    };

    View();

    ~View();

    Result init(platform::Context* context);

    void terminate(platform::Context* context);

    void setView(glm::vec3 position, glm::vec3 rotation);

    void updateView();

    void updateViewMatrix();

    void setPerspective(float fov, float ratio, float minDepth, float maxDepth);

    Result updateUniformBuffer(platform::Context* context);

    rhi::UniformBuffer* getUniformBuffer();

    void rotate(glm::vec3 delta);

    void translate(glm::vec3 delta);

    void setType(Type type);

  private:
    Type type;
    std::mutex mutex;
    bool dirty;
    struct UniformBufferObject
    {
        ALIGNED(16)
        glm::mat4 view_inverse;
        ALIGNED(16)
        glm::mat4 proj_inverse;
        ALIGNED(16)
        glm::mat4 view_proj_inverse;
        ALIGNED(16)
        glm::mat4 prev_view_proj;
        ALIGNED(16)
        glm::mat4 view_proj;
        ALIGNED(16)
        glm::mat4 view;
        ALIGNED(16)
        glm::mat4 proj;
        ALIGNED(16)
        glm::vec4 view_pos;
        float nearPlane;
        float farPlane;
    } ubo;

    rhi::UniformBuffer* uniformBuffer;

    static constexpr size_t uniformDataSize = sizeof(UniformBufferObject);

    // input
    glm::vec3 position;
    glm::vec3 rotation;
    glm::mat4 projection;

  public:
    void handle_key_W(bool pressed);
    void handle_key_S(bool pressed);
    void handle_key_A(bool pressed);
    void handle_key_D(bool pressed);

    void handle_mouse_move(float x, float y);
    void handle_mouse_LB(bool pressed);
    void handle_mouse_RB(bool pressed);

  private:
    float rotationSpeed = 0.3f;
    float moveSpped = 0.1f;

    struct
    {
        bool left = false;
        bool right = false;
        bool up = false;
        bool down = false;

        bool any()
        {
            return left || right || up || down;
        }
    } keyInput;

    glm::vec2 mouseCursorPos;
    struct
    {
        bool left = false;
        bool right = false;
        bool middle = false;
    } mouseButtonInput;
};
} // namespace scene