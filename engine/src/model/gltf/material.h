#pragma once
#include "model/material.h"

namespace model
{
namespace gltf
{

struct MaterialUniformBlock
{
    glm::vec4 materialFactors; // x: alphaCutoff, y: metallicFactor, z: roughnessFactor, w: reserved
    glm::vec4 baseColorFactor;
    AlphaMode alphaMode; // align?

    MaterialUniformBlock();
};

class Material : public model::Material
{
  public:
    Result init(platform::Context* context) override;

    Result update(platform::Context* context) override;

    void setAlphaCutoff(float v);

    void setMetallicFactor(float v);

    void setRoughnessFactor(float v);

    void setBaseColorFactor(glm::vec4 colorFactor);

    void setAlphaMode(AlphaMode alphaMode);

  private:
    MaterialUniformBlock ubo;
};
} // namespace gltf
} // namespace model