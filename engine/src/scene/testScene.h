#include "scene.h"

namespace scene
{
class TestScene : public Scene
{
  public:
    Result init(platform::Conetxt* context) override;

    Result load(platform::Conetxt* context) override;

    Result udpate(platform::Conetxt* context) override;

    void terminate(platform::Conetxt* context) override;

  private:
};
} // namespace scene