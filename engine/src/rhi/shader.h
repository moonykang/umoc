#pragma once
#include "common/memorybuffer.h"
#include <string>

namespace rhi
{

enum VertexAttribute
{
    Position = 1,
    Normal = 2,
    Uv = 4,
    Color = 8
};
using VertexAttributes = uint8_t;

class ShaderBase
{
  public:
    ShaderBase(std::string name) : name(name)
    {
    }

    ~ShaderBase() = default;

    inline size_t getHash()
    {
        return hash;
    }

  private:
    std::string name;
    VertexAttributes vertexAttributes;
    /*
    name
    attribute layout (pos / normal / uv / color / ...)
    ub layout
    out layout
     */
    size_t hash;
    MemoryBuffer code;
};
} // namespace rhi