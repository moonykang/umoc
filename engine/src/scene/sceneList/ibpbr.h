#include "../scene.h"

namespace model
{
class VertexInput;
class Instance;
} // namespace model

namespace rhi
{
class UniformBuffer;
class DescriptorSet;
class Texture;
} // namespace rhi

namespace scene
{
class IBPBRScene : public SceneInfo
{
  public:
    Result load(platform::Context* context) override;

    Result udpate(platform::Context* context) override;
};
} // namespace scene