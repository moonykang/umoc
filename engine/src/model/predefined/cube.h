#pragma once
#include "object.h"

namespace model
{
namespace predefined
{
class Cube : public Object
{
  public:
    Cube();

    Result loadVertexBuffer(platform::Context* context);

    Result loadIndexBuffer(platform::Context* context);
};
} // namespace predefined
} // namespace model