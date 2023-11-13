#pragma once
#include "object.h"

namespace model
{
namespace predefined
{
class Sphere : public Object
{
  public:
    Sphere();

    Result loadVertexBuffer(platform::Context* context, float uvScale);

    Result loadIndexBuffer(platform::Context* context);
};
} // namespace predefined
} // namespace model