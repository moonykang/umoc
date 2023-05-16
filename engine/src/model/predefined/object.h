#pragma once
#include "../object.h"
#include "common/util.h"

namespace model
{
namespace predefined
{
class Object : public model::Object
{
  public:
    Object(uint32_t vertexCount, uint32_t indexCount);

    virtual Result loadVertexBuffer(platform::Context* context) = 0;

    virtual Result loadIndexBuffer(platform::Context* context) = 0;

    Result loadMesh();

  protected:
    uint32_t vertexCount = 0;
    uint32_t indexCount = 0;
};
} // namespace predefined
} // namespace model