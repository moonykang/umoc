#pragma once
#include "object.h"

namespace model
{
namespace predefined
{
class Quad : public Object
{
  public:
    Quad();

    Result loadVertexBuffer(platform::Context* context, float uvScale);

    Result loadIndexBuffer(platform::Context* context);
};
} // namespace predefined
} // namespace model