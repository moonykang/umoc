#pragma once

#include <cstdlib>

namespace model
{
using MaterialFlags = uint32_t;
class MaterialFlag
{
  public:
    enum Value : uint32_t
    {
        NONE = 0,
        BaseColorTexture = 0x00000001,
        MetalicRoughnessTexture = 0x00000002,
        NormalTexture = 0x00000004,
        OcclusionTexture = 0x00000008,
        EmissiveTexture = 0x00000010,
        SpecularGlossinessTexture = 0x00000020,
        DiffuseTexture = 0x00000040,
        All = 0xFFFFFFFF
    };

    MaterialFlag() = default;

    constexpr MaterialFlag(Value value) : value(value)
    {
    }

    constexpr MaterialFlags operator|(MaterialFlag& other) const
    {
        return value | other.value;
    }

    uint32_t get()
    {
        return value;
    }

  private:
    Value value;
};

using GltfLoadingFlags = uint32_t;
class GltfLoadingFlag
{
  public:
    enum Value : uint32_t
    {
        None = 0x00000000,
        PreTransformVertices = 0x00000001,
        PreMultiplyVertexColors = 0x00000002,
        FlipY = 0x00000004,
        DontLoadImages = 0x00000008
    };

    GltfLoadingFlag() = default;

    constexpr GltfLoadingFlag(Value value) : value(value)
    {
    }

    constexpr GltfLoadingFlags operator|(GltfLoadingFlag& other) const
    {
        return value | other.value;
    }

    uint32_t get()
    {
        return value;
    }

  private:
    Value value;
};

enum AlphaMode
{
    Mask,
    Blend,
    Opaque,
};

} // namespace model