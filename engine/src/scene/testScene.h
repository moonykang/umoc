#include "scene.h"
#include <vector>

namespace model
{
class VertexInput;
class Instance;
} // namespace model

namespace rhi
{
class UniformBuffer;
class DescriptorSet;
} // namespace rhi

namespace scene
{
class TestScene : public SceneInfo
{
  public:
    Result postInit(platform::Context* context) override;

    Result load(platform::Context* context) override;

    Result udpate(platform::Context* context) override;

    void preTerminate(platform::Context* context) override;

    void postTerminate(platform::Context* context) override;

  public:
    model::VertexInput* quad;
    model::Instance* instance;
};
} // namespace scene