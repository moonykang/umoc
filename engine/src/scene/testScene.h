#include "scene.h"
#include <vector>

namespace model
{
class VertexInput;
}

namespace scene
{
class TestScene : public SceneInfo
{
  public:
    Result init(platform::Context* context) override;

    Result load(platform::Context* context) override;

    Result udpate(platform::Context* context) override;

    void terminate(platform::Context* context) override;

  public:
    model::VertexInput* quad;
};
} // namespace scene