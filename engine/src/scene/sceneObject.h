#pragma once
#include <vector>

namespace model
{
class Object;
}

namespace scene
{
enum class ObjectCategory
{
    Opaque,
    Transparent
};

class SceneObject
{
  public:
  private:
    std::vector<model::Object> opaques;
};
} // namespace scene