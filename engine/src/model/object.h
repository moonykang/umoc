#pragma once

namespace rhi
{
class Context;
class VertexBuffer;
class IndexBuffer;
} // namespace rhi

namespace model
{
class Object
{
  public:
    Object();

    ~Object() = default;

    void init(rhi::Context* context);

    void destroy(rhi::Context* context);

    virtual void preBuild(rhi::Context* context) = 0;

    virtual void draw(rhi::Context* context) = 0;

  protected:
    // rhi::VertexBuffer* vertexBuffer;
    // rhi::IndexBuffer* indexBuffer;
};
} // namespace model